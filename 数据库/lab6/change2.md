# change2.md — 行级锁 + Savepoint 部分回滚

本文档记录实现 README 第 38–39 行两种优化方向的全部改动：
- 第 38 行：表级锁 → 行级锁（多粒度锁）
- 第 39 行：write_set 物理回滚 → 更高级的 undo 操作（Savepoint 部分回滚）

两种优化互相兼容：行级锁减少锁冲突频率，savepoint 减少冲突后的回滚代价。

---

## 第一部分：行级锁（README 第 38 行）

### 1.1 优化目标

基线使用表级 S/X 锁 + wait-die：任意两个写事务即使访问同一张表的不同行，也必须串行执行（因为都要拿表级 X 锁）。这是 abort 率高达 85–89% 的根本原因之一。

行级锁优化：写事务只锁自己实际访问的行，不同行上的写操作可以并发执行。通过表级意向锁（IS/IX）实现多粒度锁协议。

### 1.2 设计思路

多粒度锁协议：

| 操作 | 表级锁 | 行级锁 |
|------|--------|--------|
| SELECT（逐行读） | IS | S on each row |
| INSERT | IX | X on new row |
| UPDATE | IX | X on each updated row |
| DELETE | IX | X on each deleted row |
| SELECT 整表（理论） | S | — |
| 写整表（理论） | X | — |

表级意向锁兼容性矩阵：

```
        IS  IX  S   X
   IS   ✓   ✓   ✓   ✗
   IX   ✓   ✓   ✗   ✗
   S    ✓   ✗   ✓   ✗
   X    ✗   ✗   ✗   ✗
```

关键点：
- **IS + IX 兼容**：读某些行的事务和写另一些行的事务可以并发
- **IX + IX 兼容**：两个写不同行的事务可以并发
- **IS/IX 与 S/X 不兼容**：有人逐行加锁时，不能再锁整表

### 1.3 修改文件清单

| 文件 | 改动 |
|------|------|
| `src/transaction/concurrency/lock_manager.h` | 更新注释，IS/IX/行锁接口从 no-op 变为真正生效 |
| `src/transaction/concurrency/lock_manager.cpp` | 全面重写：实现 IS/IX/S/X/SIX 兼容矩阵、行级 S/X 锁、锁升级逻辑 |
| `src/execution/executor_seq_scan.h` | `lock_shared_on_table` → `lock_IS_on_table` + 逐行 `lock_shared_on_record` |
| `src/execution/executor_index_scan.h` | 同上 |
| `src/execution/executor_insert.h` | `lock_exclusive_on_table` → `lock_IX_on_table` + `lock_exclusive_on_record` |
| `src/execution/executor_update.h` | `lock_exclusive_on_table` → `lock_IX_on_table` + 逐行 `lock_exclusive_on_record` |
| `src/execution/executor_delete.h` | 同上 |

### 1.4 详细改动

#### 1.4.1 `src/transaction/concurrency/lock_manager.h`

更新类注释为多粒度锁协议说明。IS/IX/行锁接口保持声明不变（原本就是声明了的），但语义从 no-op 变为真正生效。

```cpp
// ===== 表级锁（含意向锁）=====
bool lock_shared_on_table(Transaction* txn, int tab_fd);
bool lock_exclusive_on_table(Transaction* txn, int tab_fd);
bool lock_IS_on_table(Transaction* txn, int tab_fd);
bool lock_IX_on_table(Transaction* txn, int tab_fd);

// ===== 行级锁 =====
bool lock_shared_on_record(Transaction* txn, const Rid& rid, int tab_fd);
bool lock_exclusive_on_record(Transaction* txn, const Rid& rid, int tab_fd);
```

#### 1.4.2 `src/transaction/concurrency/lock_manager.cpp`

全面重写，核心变更：

**1. 新增 `is_compatible()` 函数**：按兼容矩阵判断新请求是否与当前 group_lock_mode 兼容。

**2. 新增 `upgrade_group_mode()` 函数**：计算 group_lock_mode 在新请求加入后的升级值（如 IS+IX→IX, IX+S→SIX）。

**3. `lock_IS_on_table`**：调用 `lock_table(txn, tab_fd, INTENTION_SHARED)`，走统一的表级加锁逻辑。

**4. `lock_IX_on_table`**：调用 `lock_table(txn, tab_fd, INTENTION_EXCLUSIVE)`。

**5. `lock_shared_on_record`**：对 `(tab_fd, rid, RECORD)` 加 S 锁，走 wait-die 逻辑。行级 S 锁与行级 IS/S 兼容，与 IX/X/SIX 不兼容。

**6. `lock_exclusive_on_record`**：对 `(tab_fd, rid, RECORD)` 加 X 锁，走 wait-die 逻辑。行级 X 锁与任何非空组模式都不兼容。支持 S→X 升级。

**7. `lock_table()`**：扩展为支持 IS/IX/S/X/SIX 五种模式。锁升级逻辑覆盖所有组合（IS→IX, IS→S, IS→X, S→X, S→SIX, IX→X, IX→SIX 等）。

**8. `unlock()`**：统一处理表锁和行锁的释放。删除该事务的请求后，遍历所有剩余已授予请求重算 group_lock_mode_。

#### 1.4.3 `src/execution/executor_seq_scan.h`

**beginTuple()**：
```cpp
// 旧：lock_shared_on_table → 表级 S 锁
// 新：lock_IS_on_table → 意向共享 + 逐行 lock_shared_on_record
context_->lock_mgr_->lock_IS_on_table(context_->txn_, fh_->GetFd());
// ... 扫描到第一个满足条件的行 ...
context_->lock_mgr_->lock_shared_on_record(context_->txn_, rid_, fh_->GetFd());
```

**nextTuple()**：每推进到下一个满足条件的行时，对该行加 S 锁。

#### 1.4.4 `src/execution/executor_index_scan.h`

同 SeqScanExecutor：`lock_IS_on_table` + 逐行 `lock_shared_on_record`。

#### 1.4.5 `src/execution/executor_insert.h`

```cpp
// 旧：lock_exclusive_on_table → 表级 X 锁（阻止一切并发）
// 新：lock_IX_on_table → 意向排他 + 对新行 lock_exclusive_on_record
context_->lock_mgr_->lock_IX_on_table(context_->txn_, fh_->GetFd());
// ... 插入记录 ...
context_->lock_mgr_->lock_exclusive_on_record(context_->txn_, rid_, fh_->GetFd());
```

IX 与 IX 兼容，所以不同事务插入同一张表的不同行可以并发。

#### 1.4.6 `src/execution/executor_update.h`

```cpp
// 旧：lock_exclusive_on_table → 表级 X 锁
// 新：lock_IX_on_table + 逐行 lock_exclusive_on_record
context_->lock_mgr_->lock_IX_on_table(context_->txn_, fh_->GetFd());
for (auto &rid : rids_) {
    context_->lock_mgr_->lock_exclusive_on_record(context_->txn_, rid, fh_->GetFd());
    // ... 更新记录 ...
}
```

两个事务更新同一张表的不同行可以并发（IX+IX 兼容，不同行的 X 锁互不冲突）。

#### 1.4.7 `src/execution/executor_delete.h`

同 UpdateExecutor：`lock_IX_on_table` + 逐行 `lock_exclusive_on_record`。

### 1.5 行级锁的性能影响

**TPC-C 场景分析**：

| 事务类型 | 访问的表 | 基线（表级X锁） | 优化后（行级X锁） |
|----------|----------|----------------|-------------------|
| NewOrder | stock, district, orders, new_orders | 4个表级X锁，与其他写事务完全互斥 | 只锁 5-15 行，不同仓库/区域可并发 |
| Payment | customer, district, history | 3个表级X锁 | 只锁 2-3 行 |
| Delivery | orders, new_orders, order_line | 3个表级X锁 | 只锁若干行 |

在 10 仓库 + 4 线程场景下，NewOrder 事务操作不同仓库的概率很高。行级锁让这些事务完全并发，不再被表级 X 锁阻塞。

预期效果：
- abort 率大幅下降（冲突从"同表"变为"同行"）
- 吞吐量提升（并发度增加）
- 与 savepoint 部分回滚协同：即使发生行级冲突 abort，也只需回滚一条语句

---

## 第二部分：Savepoint 部分回滚（README 第 39 行）

### 2.1 优化目标

即使有了行级锁，仍会发生行级冲突（两个事务写同一行），wait-die 会 abort 较年轻的事务。基线中整个事务被回滚+客户端重发。Savepoint 部分回滚优化让服务器只回滚冲突语句，自动重试，避免完整回滚。

### 2.2 设计思路

```
基线流程：
  语句1 → 语句2 → 语句3 → 语句4(abort) → 完整回滚语句1~4 → 客户端重发语句1~4

优化后流程：
  语句1 → 语句2 → 语句3 → [savepoint] → 语句4(abort)
    → partial_abort(回滚语句4) → 退避等待 → 重试语句4 → 成功 → 语句5...
```

核心原则：
- **只回滚冲突语句**：partial_abort 只回滚 savepoint 之后的写操作，保留之前所有已成功执行的写操作。
- **不释放锁**：partial_abort 不释放事务已持有的锁（包括表级意向锁和行级锁）。重试时锁请求走幂等路径直接返回。
- **事务保持活跃**：partial_abort 不改变事务状态（仍为 GROWING），不从全局事务表移除，事务可继续执行。
- **指数退避**：每次重试前 sleep，给冲突事务时间完成，避免活锁。
- **有限重试**：最多重试 3 次，超过后退化为完整 abort，保证不会无限重试。

### 2.3 修改文件清单

| 文件 | 改动 |
|------|------|
| `src/transaction/transaction.h` | Transaction 类增加 `savepoint_` 字段和 `set_savepoint()`/`get_savepoint()` 方法 |
| `src/transaction/transaction_manager.h` | 声明 `partial_abort()` 方法 |
| `src/transaction/transaction_manager.cpp` | 实现 `partial_abort()`：回滚 write_set 到 savepoint，不释放锁、不改变状态 |
| `src/rmdb.cpp` | 语句执行循环增加 savepoint 重试逻辑；添加 `<chrono>`/`<thread>` 头文件和 `SAVEPOINT_MAX_RETRY` 常量 |

### 2.4 详细改动

#### 2.4.1 `src/transaction/transaction.h`

Transaction 类新增 savepoint 支持：

```cpp
// 公有方法
inline void set_savepoint() { savepoint_ = write_set_->size(); }
inline size_t get_savepoint() { return savepoint_; }

// 私有成员
size_t savepoint_ = 0;  // savepoint 标记 write_set 位置，partial_abort 回滚到此
```

`set_savepoint()` 在每条语句执行前调用，记录当前 `write_set` 的大小。partial_abort 时回滚到这个位置。

#### 2.4.2 `src/transaction/transaction_manager.h`

声明 partial_abort 方法：

```cpp
// 部分回滚：回滚 write_set 中 savepoint 之后的写操作，但不释放锁、不改变事务状态。
// 用于 wait-die 死锁预防触发 abort 时，只回滚当前语句而非整个事务。
void partial_abort(Transaction* txn);
```

#### 2.4.3 `src/transaction/transaction_manager.cpp`

实现 `partial_abort()`，逻辑与 `abort()` 的写操作回滚部分相同，但有关键区别：

| 对比项 | `abort()` | `partial_abort()` |
|--------|-----------|-------------------|
| 回滚范围 | 全部 write_set | savepoint 之后的部分 |
| 释放锁 | 是（表锁+行锁） | 否 |
| 事务状态 | → ABORTED | 不变（保持 GROWING） |
| 从事务表移除 | 是 | 否 |
| delete txn | 是 | 否 |

回滚逻辑对三种写操作类型均生效：
- INSERT_TUPLE → 删除记录和索引项
- DELETE_TUPLE → 恢复记录和索引项
- UPDATE_TUPLE → 删除当前索引项，恢复旧记录，重建旧索引项

#### 2.4.4 `src/rmdb.cpp`

在语句执行流程中加入 savepoint 重试循环：

```cpp
#define SAVEPOINT_MAX_RETRY 3  // wait-die 部分回滚最大重试次数

// 在 optimizer->plan_query 之后：
context->txn_->set_savepoint();
bool statement_succeeded = false;
for (int retry = 0; retry <= SAVEPOINT_MAX_RETRY && !statement_succeeded; ++retry) {
    try {
        std::shared_ptr<PortalStmt> portalStmt = portal->start(plan, context);
        portal->run(portalStmt, ql_manager.get(), &txn_id, context);
        portal->drop();
        statement_succeeded = true;
    } catch (TransactionAbortException &e) {
        if (e.GetAbortReason() == AbortReason::DEADLOCK_PREVENTION &&
            retry < SAVEPOINT_MAX_RETRY) {
            // 部分回滚到 savepoint
            txn_manager->partial_abort(context->txn_);
            // 指数退避：100us, 200us, 400us
            std::this_thread::sleep_for(
                std::chrono::microseconds(100 << retry));
            continue;
        }
        // 非可重试异常或超过最大重试次数 → 完整 abort
        throw;
    }
}
```

---

## 第三部分：两种优化的协同

### 3.1 协同机制

行级锁和 savepoint 部分回滚是互补的：

1. **行级锁减少冲突概率**：不同事务写不同行可以并发，只有写同行才冲突。在 TPC-C 10 仓库场景下，行级冲突远少于表级冲突。

2. **savepoint 减少冲突代价**：即使发生行级冲突，也只回滚冲突语句而非整个事务。行级锁使得 partial_abort 需要回滚的写操作更少（因为一个语句通常只涉及少量行）。

3. **不释放锁 + 行级锁**：partial_abort 不释放行级锁。重试时，已持有的行级锁直接幂等返回，只有新行的锁才需要竞争。

### 3.2 执行流程示例

```
事务 T1 (NewOrder, 仓库 1):
  [IS on stock] → [S on stock(1,100)] → 读 s_quantity
  [IX on stock] → [X on stock(1,100)] → 更新 s_quantity
  [IX on district] → [X on district(1,1)] → 更新 d_next_o_id
  [IX on orders] → [X on orders(1,1,3001)] → 插入订单
  ...

事务 T2 (NewOrder, 仓库 2):
  [IS on stock] → [S on stock(2,50)] → 读 s_quantity    ← 与 T1 无冲突！
  [IX on stock] → [X on stock(2,50)] → 更新 s_quantity   ← 与 T1 无冲突！
  ...

基线中 T1 和 T2 在 stock 表上就会互相阻塞（表级 X 锁），
行级锁下完全并发。
```

### 3.3 预期性能提升

| 指标 | 基线 | +行级锁 | +行级锁+savepoint |
|------|------|---------|-------------------|
| 锁粒度 | 表级 | 行级 | 行级 |
| 不同行并发 | ✗ | ✓ | ✓ |
| abort 回滚范围 | 整个事务 | 整个事务 | 单条语句 |
| abort 后自动重试 | ✗ | ✗ | ✓（最多3次） |
| 客户端重发 | 必须 | 必须 | 大部分避免 |
| 预期 abort 率 | 85-89% | ~20-40% | ~5-15% |

### 3.4 对客户端透明

两种优化均对客户端（tpcc_driver）透明。从客户端角度看：
- 原本会因表级锁冲突 abort 的事务，现在可能因行级锁无冲突而直接成功
- 原本会因行级锁冲突 abort 的事务，现在可能因 savepoint 重试而成功
- 只有重试 3 次后仍冲突才退化为完整 abort，客户端需重发
