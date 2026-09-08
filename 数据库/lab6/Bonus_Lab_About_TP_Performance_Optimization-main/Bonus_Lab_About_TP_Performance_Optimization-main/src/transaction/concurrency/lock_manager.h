/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2. ... */

#pragma once

#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <list>
#include "transaction/transaction.h"

static const std::string GroupLockModeStr[10] = {"NON_LOCK", "IS", "IX", "S", "X", "SIX"};

/**
 * @brief 多粒度锁（表级意向锁 + 行级锁）+ wait-die 死锁预防
 *
 * 锁模式：
 *   - 表级意向锁：IS（意向共享）、IX（意向排他）、S（共享整表）、X（独占整表）、SIX
 *   - 行级锁：S（共享行）、X（排他行）
 *
 * 兼容性矩阵（表级）：
 *        IS  IX  S   X
 *   IS   ✓   ✓   ✓   ✗
 *   IX   ✓   ✓   ✗   ✗
 *   S    ✓   ✗   ✓   ✗
 *   X    ✗   ✗   ✗   ✗
 *
 * 协议：
 *   - 读某行：IS on table + S on row
 *   - 写某行：IX on table + X on row
 *   - 读整表：S on table
 *   - 写整表：X on table
 *
 * 死锁预防：wait-die（老事务可以等，年轻事务直接 abort）
 * 严格 2PL：commit/abort 时由 TransactionManager 统一释放
 */
class LockManager {
public:
    enum class LockMode { SHARED, EXLUCSIVE, INTENTION_SHARED, INTENTION_EXCLUSIVE, S_IX };
    enum class GroupLockMode { NON_LOCK, IS, IX, S, X, SIX };

    class LockRequest {
    public:
        LockRequest(txn_id_t txn_id, timestamp_t ts, LockMode lock_mode)
            : txn_id_(txn_id), start_ts_(ts), lock_mode_(lock_mode), granted_(false) {}
        txn_id_t txn_id_;
        timestamp_t start_ts_;       // 用于 wait-die
        LockMode lock_mode_;
        bool granted_;
    };

    class LockRequestQueue {
    public:
        std::list<LockRequest> request_queue_;
        std::condition_variable cv_;
        // 当前已授予的锁的"组模式"：NON_LOCK / S / X
        GroupLockMode group_lock_mode_ = GroupLockMode::NON_LOCK;
    };

public:
    LockManager() {}
    ~LockManager() {}

    // ===== 表级锁（含意向锁）=====
    bool lock_shared_on_table(Transaction* txn, int tab_fd);
    bool lock_exclusive_on_table(Transaction* txn, int tab_fd);
    bool lock_IS_on_table(Transaction* txn, int tab_fd);
    bool lock_IX_on_table(Transaction* txn, int tab_fd);

    // ===== 行级锁 =====
    bool lock_shared_on_record(Transaction* txn, const Rid& rid, int tab_fd);
    bool lock_exclusive_on_record(Transaction* txn, const Rid& rid, int tab_fd);

    // ===== 释放：被 TransactionManager 在 commit/abort 末尾调用 =====
    bool unlock(Transaction* txn, LockDataId lock_data_id);

    // ===== 兼容旧接口：现已 no-op，事务并发由 lock_table_ 管 =====
    void global_lock()   { /* no-op */ }
    void global_unlock() { /* no-op */ }

private:
    // 内部：表级加锁通用逻辑（支持 IS/IX/S/X/SIX）
    bool lock_table(Transaction* txn, int tab_fd, LockMode mode);

    std::mutex latch_;                                          // 保护 lock_table_ 自身
    std::unordered_map<LockDataId, LockRequestQueue> lock_table_;
};
