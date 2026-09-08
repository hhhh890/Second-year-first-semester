/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "lock_manager.h"
#include "transaction/txn_defs.h"

/**
 * @brief 表级 S/X 锁 + wait-die 死锁预防（回退到基线版本）
 *
 * 设计取舍：
 *   - 只做表锁，不做行锁：对 TPC-C 来说粒度仍然偏粗，但已经能让"读读并发、不同表读写并发"
 *   - 只支持 S（共享）/ X（独占）两种模式
 *   - 锁升级：持 S 的事务再请求 X 时，如果只有自己持 S 则升级，否则 abort
 *   - 死锁预防：wait-die（老事务可以等，年轻事务直接 abort）
 *   - 严格 2PL：commit/abort 时由 TransactionManager 统一释放
 */

bool LockManager::lock_shared_on_table(Transaction* txn, int tab_fd) {
    if (txn == nullptr) return true;
    if (txn->get_state() == TransactionState::SHRINKING) {
        throw TransactionAbortException(txn->get_transaction_id(),
                                        AbortReason::LOCK_ON_SHIRINKING);
    }

    LockDataId id(tab_fd, LockDataType::TABLE);
    std::unique_lock<std::mutex> lk(latch_);
    auto &q = lock_table_[id];

    // 幂等检查：已持有 S 或 X 则直接返回
    for (auto &req : q.request_queue_) {
        if (req.txn_id_ == txn->get_transaction_id() && req.granted_) {
            return true;  // 已持 S 或 X，幂等
        }
    }

    // wait-die：S 与 S 兼容，与 X 不兼容
    while (q.group_lock_mode_ != GroupLockMode::NON_LOCK &&
           q.group_lock_mode_ != GroupLockMode::S) {
        // 当前有 X 锁，不兼容新的 S
        bool can_wait = false;
        for (auto &req : q.request_queue_) {
            if (req.granted_ && req.txn_id_ != txn->get_transaction_id()) {
                if (txn->get_start_ts() < req.start_ts_) {
                    can_wait = true;  // 自己更老，可以等
                }
            }
        }
        if (!can_wait) {
            throw TransactionAbortException(txn->get_transaction_id(),
                                            AbortReason::DEADLOCK_PREVENTION);
        }
        q.cv_.wait(lk);
    }

    // 授予 S 锁
    q.request_queue_.emplace_back(txn->get_transaction_id(), txn->get_start_ts(), LockMode::SHARED);
    q.request_queue_.back().granted_ = true;
    q.group_lock_mode_ = GroupLockMode::S;
    txn->get_lock_set()->insert(id);
    return true;
}

bool LockManager::lock_exclusive_on_table(Transaction* txn, int tab_fd) {
    if (txn == nullptr) return true;
    if (txn->get_state() == TransactionState::SHRINKING) {
        throw TransactionAbortException(txn->get_transaction_id(),
                                        AbortReason::LOCK_ON_SHIRINKING);
    }

    LockDataId id(tab_fd, LockDataType::TABLE);
    std::unique_lock<std::mutex> lk(latch_);
    auto &q = lock_table_[id];

    // 幂等检查：已持有 X 则直接返回
    for (auto &req : q.request_queue_) {
        if (req.txn_id_ == txn->get_transaction_id() && req.granted_) {
            if (req.lock_mode_ == LockMode::EXLUCSIVE) return true;
            // 持 S 升级为 X
            bool only_me = true;
            for (auto &r : q.request_queue_) {
                if (r.granted_ && r.txn_id_ != txn->get_transaction_id()) {
                    only_me = false;
                    break;
                }
            }
            if (only_me) {
                req.lock_mode_ = LockMode::EXLUCSIVE;
                q.group_lock_mode_ = GroupLockMode::X;
                return true;
            }
            throw TransactionAbortException(txn->get_transaction_id(),
                                            AbortReason::DEADLOCK_PREVENTION);
        }
    }

    // wait-die：X 与任何非空组模式都不兼容
    while (q.group_lock_mode_ != GroupLockMode::NON_LOCK) {
        bool can_wait = false;
        for (auto &req : q.request_queue_) {
            if (req.granted_ && req.txn_id_ != txn->get_transaction_id()) {
                if (txn->get_start_ts() < req.start_ts_) {
                    can_wait = true;
                }
            }
        }
        if (!can_wait) {
            throw TransactionAbortException(txn->get_transaction_id(),
                                            AbortReason::DEADLOCK_PREVENTION);
        }
        q.cv_.wait(lk);
    }

    // 授予 X 锁
    q.request_queue_.emplace_back(txn->get_transaction_id(), txn->get_start_ts(), LockMode::EXLUCSIVE);
    q.request_queue_.back().granted_ = true;
    q.group_lock_mode_ = GroupLockMode::X;
    txn->get_lock_set()->insert(id);
    return true;
}

bool LockManager::lock_IS_on_table(Transaction* txn, int tab_fd) {
    // 基线：IS 锁等同于 S 锁（兼容性相同）
    return lock_shared_on_table(txn, tab_fd);
}

bool LockManager::lock_IX_on_table(Transaction* txn, int tab_fd) {
    // 基线：IX 锁等同于 X 锁（兼容性相同）
    return lock_exclusive_on_table(txn, tab_fd);
}

bool LockManager::lock_shared_on_record(Transaction* txn, const Rid& rid, int tab_fd) {
    // 基线：行级锁为 no-op
    return true;
}

bool LockManager::lock_exclusive_on_record(Transaction* txn, const Rid& rid, int tab_fd) {
    // 基线：行级锁为 no-op
    return true;
}

bool LockManager::unlock(Transaction* txn, LockDataId lock_data_id) {
    if (txn == nullptr) return true;

    std::unique_lock<std::mutex> lk(latch_);
    auto it = lock_table_.find(lock_data_id);
    if (it == lock_table_.end()) return true;
    auto &q = it->second;

    // 删除该事务的所有请求
    for (auto rit = q.request_queue_.begin(); rit != q.request_queue_.end();) {
        if (rit->txn_id_ == txn->get_transaction_id()) {
            rit = q.request_queue_.erase(rit);
        } else {
            ++rit;
        }
    }

    // 重算 group_lock_mode_
    q.group_lock_mode_ = GroupLockMode::NON_LOCK;
    for (auto &req : q.request_queue_) {
        if (req.granted_) {
            if (req.lock_mode_ == LockMode::EXLUCSIVE) {
                q.group_lock_mode_ = GroupLockMode::X;
            } else {
                q.group_lock_mode_ = GroupLockMode::S;
            }
        }
    }

    // 唤醒等待者
    q.cv_.notify_all();
    return true;
}