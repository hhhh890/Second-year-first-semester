# Changelog

本文档记录本次 Bonus Lab 中新增和修改代码的原因，方便团队成员对照仓库理解实现。

## 1. 总体目标

本实验的主要瓶颈有两个：

1. 查询优化器原本强制所有查询走 `SeqScan`，即使表上已经创建了索引，也不会使用索引。
2. B+ 树索引模块的核心函数仍是空实现，无法支撑 `CREATE INDEX`、`IndexScan` 和索引维护。
3. `UPDATE` / `DELETE` 会先扫描定位记录，再执行写操作；扫描阶段拿 S 锁，写阶段再升级为 X 锁，容易触发当前 wait-die 表级锁实现中的升级冲突。

本次优化围绕这三点展开：

- 补全 B+ 树索引的基本功能。
- 让查询优化器在合适场景下选择 `IndexScan`。
- 对写操作的定位扫描提前拿 X 锁，减少锁升级导致的 abort。

## 2. `src/index/ix_index_handle.cpp`

### 修改原因

该文件是 B+ 树索引的核心实现文件。原代码中大量函数仍然是 `Todo`，例如：

- `IxNodeHandle::lower_bound`
- `IxNodeHandle::upper_bound`
- `IxNodeHandle::leaf_lookup`
- `IxNodeHandle::internal_lookup`
- `IxNodeHandle::insert`
- `IxIndexHandle::find_leaf_page`
- `IxIndexHandle::insert_entry`
- `IxIndexHandle::delete_entry`
- `IxIndexHandle::split`
- `IxIndexHandle::coalesce_or_redistribute`

如果这些函数不实现，索引文件虽然可以被创建，但无法正确插入、查找或删除 key，查询优化器也不能安全启用索引扫描。

### 主要修改

实现了节点内二分查找：

- `lower_bound(target)` 返回第一个 `>= target` 的位置。
- `upper_bound(target)` 返回第一个 `> target` 的位置。

实现了叶子节点查找：

- `leaf_lookup` 先用 `lower_bound` 定位 key。
- 如果当前位置的 key 等于目标 key，就返回对应的 `Rid`。

实现了内部节点查找：

- 本框架的内部节点约定是：`key[i]` 表示第 `i` 个 child 子树中的最小 key。
- 因此查找子节点时使用 `upper_bound(key) - 1`。
- 如果目标 key 比第一个 key 还小，就进入第 0 个 child。

实现了插入逻辑：

- 先定位目标叶子节点。
- 在叶子节点中按 key 顺序插入。
- 如果节点满了，就调用 `split` 分裂节点。
- 分裂后将新节点的第一个 key 插入父节点。
- 如果根节点分裂，则创建新的根节点。

实现了删除逻辑：

- 先定位目标叶子节点并删除 key。
- 如果删除后节点大小仍合法，只维护父节点 key。
- 如果节点过小，优先向兄弟节点借一个 key。
- 如果兄弟节点也不够借，则合并两个节点。
- 合并后递归修复父节点。
- 如果根节点只剩一个 child，则压缩根节点。

修复了索引页号分配：

- 原构造函数用当前 fd 的 page counter 推断下一页，重新打开索引文件后可能从错误页号继续分配。
- 修改为使用索引文件头 `file_hdr_->num_pages_` 作为下一页号。
- 这样新建 B+ 树节点不会覆盖已有的 header page 或 root page。

修复了若干资源释放问题：

- `fetch_node` 返回的是新分配的 `IxNodeHandle`。
- 使用后需要同时 `unpin_page` 和 `delete`。
- 对 `get_rid`、`leaf_end`、`erase_leaf` 等路径补充了释放逻辑。

### 为什么这样实现

官方测试会检查 B+ 树的结构不变量，包括：

- 叶子节点之间的前驱 / 后继指针是否一致。
- 内部节点的 key 是否等于对应 child 的 first key。
- `lower_bound` / `upper_bound` / `IxScan` 的顺序是否和真实有序集合一致。

所以实现时必须遵守本框架的内部节点表示方式，而不是照搬其他教材中“第 0 个 key 无效”的 B+ 树写法。

## 3. `src/index/ix_scan.cpp`

### 修改原因

`IxScan::next()` 原来每次调用都会 `fetch_node`，但没有释放对应的 page handle。

在长扫描中，这会导致 buffer pool 中大量页面一直处于 pinned 状态，严重时会导致后续页面无法被替换。

### 主要修改

在 `IxScan::next()` 结尾增加：

- `unpin_page(node->get_page_id(), false)`
- `delete node`

### 为什么这样实现

`IxScan` 只是读取索引叶子节点，不修改页面，所以 `unpin_page` 的 dirty 参数为 `false`。

## 4. `src/index/ix_defs.h`

### 修改原因

`IxFileHdr::deserialize` 中有一行调试输出：

```cpp
std::cout << col_num_ << "\n";
```

这会污染 benchmark 输出，也会影响自动化脚本解析结果。

### 主要修改

删除该调试输出。

## 5. `src/optimizer/planner.cpp`

### 修改原因

原查询优化器中有一个硬编码开关：

```cpp
constexpr bool force_seq_scan = true;
```

它会强制所有查询使用 `SeqScan`。即使表上存在索引，也不会生成 `IndexScan` 执行计划。

这正是 TPC-C 性能很差的主要原因之一。

### 主要修改

移除了强制顺序扫描逻辑。

新增索引选择规则：

- 遍历当前表的所有索引。
- 检查 WHERE 条件中是否包含索引列的等值左前缀。
- 如果等值前缀后的第一列出现 `<`、`<=`、`>`、`>=` 范围条件，也允许选择该索引。
- 选择“等值前缀最长、范围条件次之”的索引。

例如表上有索引：

```text
stock(s_w_id, s_i_id)
```

以下查询可以使用该索引：

```sql
SELECT s_quantity FROM stock WHERE s_w_id = 1 AND s_i_id = 100;
```

以下查询也可以使用该索引：

```sql
SELECT s_quantity FROM stock WHERE s_w_id = 1;
```

第二个查询只匹配了索引左前缀 `s_w_id`，但仍然可以通过索引定位到一个较小范围，而不是扫描整张表。

以下范围查询也可以使用单列索引：

```sql
SELECT * FROM concurrency_test WHERE id > 4 AND id < 20;
```

这个改动用于完善 B+ 树范围扫描能力。之前这类查询会退回 `SeqScan`，输出顺序按物理插入顺序排列；改为 `IndexScan` 后，结果按索引 key 顺序返回，与 `phantom_read_test_4` 的标准答案一致。

### 为什么这样实现

TPC-C 中很多查询都是复合索引上的点查或前缀查询，例如：

- `district(d_w_id, d_id)`
- `customer(c_w_id, c_d_id, c_id)`
- `stock(s_w_id, s_i_id)`
- `orders(o_w_id, o_d_id, o_id)`
- `new_orders(no_w_id, no_d_id, no_o_id)`

使用“最长等值左前缀”规则，可以覆盖这些主要查询路径，同时实现复杂度较低。

补充范围条件后，优化器可以覆盖更多 B+ 树天然适合的查询，例如单列范围查找和“复合索引等值前缀 + 下一列范围”的查找。实现上仍然保持保守：不会使用跳过左前缀的索引列，也不会把 `!=` 当作可用索引条件。

## 6. `src/execution/executor_index_scan.h`

### 修改原因一：复合索引范围边界错误

`IndexScanExecutor` 会根据 WHERE 条件构造索引扫描的 lower key 和 upper key。

原代码对缺失的索引列使用：

- 下界填充 `0x00`
- 上界填充 `0xFF`

这对字符串字段基本可行，但对整数不正确。

例如 `INT` 的 `0xFFFFFFFF` 会被解释为 `-1`，不是最大整数。这样在扫描：

```sql
WHERE s_w_id = 1
```

并使用索引：

```text
stock(s_w_id, s_i_id)
```

时，upper key 可能变成 `(1, -1)`，导致正数 `s_i_id` 都扫不到。

### 主要修改一

按列类型填充范围边界：

- `INT` 下界使用 `INT_MIN`，上界使用 `INT_MAX`。
- `FLOAT` 下界使用负的最大 float，上界使用最大 float。
- `CHAR` 继续使用 `0x00` 和 `0xFF`。

这样复合索引的前缀扫描才能正确覆盖完整范围。

### 修改原因二：写扫描提前拿 X 锁

为了减少 `UPDATE` / `DELETE` 中的锁升级冲突，`IndexScanExecutor` 增加了一个 `exclusive_lock_` 标志。

### 主要修改二

扫描开始时：

- 如果是普通查询，拿 S 锁。
- 如果是写操作定位扫描，直接拿 X 锁。

这样后续真正执行 UPDATE/DELETE 时再次申请 X 锁，会被 lock manager 当作重复请求，不会再触发 S 到 X 的升级冲突。

## 7. `src/execution/executor_seq_scan.h`

### 修改原因

不是所有 UPDATE/DELETE 都一定能走索引。如果某个写操作仍然使用 `SeqScan` 定位记录，也会遇到同样的 S 到 X 锁升级问题。

### 主要修改

和 `IndexScanExecutor` 一样，`SeqScanExecutor` 也增加了 `exclusive_lock_` 标志。

扫描开始时：

- 普通 SELECT 使用 S 锁。
- UPDATE/DELETE 的定位扫描使用 X 锁。

### 为什么这样实现

这个改动不改变事务语义。写事务本来最终就需要拿 X 锁，只是将拿锁时间提前到定位扫描阶段，避免中途升级失败。

## 8. `src/optimizer/plan.h`

### 修改原因

执行器需要知道当前扫描是普通读扫描，还是 UPDATE/DELETE 的写定位扫描。

### 主要修改

给 `ScanPlan` 增加：

```cpp
bool exclusive_lock_;
```

默认值是 `false`。

普通 SELECT 创建的 `ScanPlan` 不需要额外传参，保持读锁行为。

UPDATE/DELETE 创建的 `ScanPlan` 会传入 `true`，表示扫描阶段直接申请 X 锁。

同时补充了 `plan.h` 依赖的头文件，使其能独立包含 `SmManager`、`Condition`、`Value` 等类型定义。

## 9. `src/portal.h`

### 修改原因

`Portal` 负责把逻辑计划转换为实际 executor。

既然 `ScanPlan` 中新增了 `exclusive_lock_`，这里就需要把该标志传给具体的 `SeqScanExecutor` 或 `IndexScanExecutor`。

### 主要修改

创建扫描 executor 时传入：

```cpp
x->exclusive_lock_
```

这样 planner 中的锁策略才能真正传递到执行层。

## 10. `src/system/sm_manager.cpp`

### 修改原因

GoogleTest 的 B+ 树测试夹具会先调用 `SmManager::create_index(..., nullptr)` 创建索引，然后自己再通过 `IxManager::open_index` 打开同一个索引文件。

原实现中，`create_index` 不区分调用场景，会把新建索引一直保存在 `SmManager::ihs_` 里。这样测试夹具再次打开索引时，底层 `DiskManager` 会认为同一个文件已经打开，报错：

```text
Error: File is opened: table1_col1.idx
```

### 主要修改

`create_index` 现在先使用局部 index handle 完成建索引和批量插入：

- 如果 `context != nullptr`，说明是正常 SQL 执行路径，继续把 index handle 放入 `ihs_`，保持原有运行时行为。
- 如果 `context == nullptr`，说明是测试或工具调用场景，完成创建后立即关闭该 index handle，让调用方可以重新打开。

### 为什么这样实现

这个改动只调整测试夹具和无事务上下文调用时的资源生命周期，不改变正常数据库服务中的索引可用性。TPC-C loader 和 SQL 执行路径仍然会保留打开的索引句柄。

## 11. `src/test/CMakeLists.txt`

### 修改原因

本机 WSL 中已经安装了 GoogleTest 头文件和静态库，但 Ubuntu 的 gtest 静态库布局需要同时链接：

- `gtest_main`
- `gtest`
- `pthread`

原 CMake 只链接 `gtest_main`，会导致 B+ 树测试目标链接失败。

### 主要修改

给所有 gtest 测试目标补充：

```cmake
gtest_main gtest pthread
```

这只是测试构建配置修正，不影响数据库运行时代码。

## 12. B+ 树并发保护补充

### 修改原因

`b_plus_tree_concurrent_test` 会多线程同时插入、删除同一棵 B+ 树。B+ 树的分裂、合并、根节点调整、叶子链维护都属于结构修改操作，如果多个线程同时执行，可能出现：

- 两个线程同时向同一节点插入，节点大小计算失效。
- 一个线程正在分裂节点，另一个线程看到中间状态。
- 删除合并时父节点和兄弟节点指针被并发改写。

初次运行并发测试时，插入阶段触发了节点容量断言，说明索引结构修改需要互斥保护。

### 主要修改

在 `IxIndexHandle::insert_entry` 和 `IxIndexHandle::delete_entry` 入口处增加：

```cpp
std::lock_guard<std::mutex> guard(root_latch_);
```

也就是对整棵 B+ 树的写入结构修改加一把粗粒度互斥锁。

### 为什么这样实现

这是一个优先保证正确性的低风险实现：

- 插入和删除涉及多页结构修改，统一加锁能避免并发分裂、合并、根调整互相踩状态。
- 查找和扫描仍按原逻辑执行，没有引入新的接口或页面格式。
- 对本实验来说，TPC-C 的主要收益来自“使用索引减少扫描”和“减少锁升级 abort”，粗粒度 B+ 树写锁对吞吐有一定影响，但比并发结构损坏安全得多。

更细粒度的改进方向是实现 B+ 树 page latch coupling，也就是查找路径上逐页加锁、确认 child 安全后释放 parent。但这会明显增加实现复杂度，不适合作为本次 Bonus Lab 的主线。

## 13. 验证情况

由于部分环境缺少 `bison`、`flex`、`readline-dev` 等依赖，完整 CMake 和 TPC-C 尚未在修改当时完成。

已完成的验证包括：

1. 单列索引 smoke test：
   - 插入 1 到 1000。
   - 逐个查找。
   - 全量顺序扫描。
   - 删除 1 到 500。
   - 再次验证已删除和未删除 key。

2. 随机插入删除 smoke test：
   - 随机执行 5000 次插入 / 删除。
   - 使用 `std::set` 作为真值源。
   - 最后逐个查找并扫描比对顺序。

3. 复合索引前缀扫描 smoke test：
   - 构造 `(s_w_id, s_i_id)` 复合索引。
   - 验证只给出 `s_w_id` 条件时，可以正确扫描该前缀下的所有记录。

4. 编译检查：
   - `planner.cpp` 单独编译通过。
   - 包含 `portal.h` 的检查程序编译通过。

依赖补齐后，在本机 WSL 环境中继续完成了以下验证：

5. TPC-C 性能链路：
   - `scripts/01_setup.sh --only=10 --rebuild` 成功，loader 耗时 30 秒，golden 库大小 238M。
   - `scripts/02_run_s10_t4_d60.sh` 成功，tpmC 为 1676.78，TPS 为 62.103。
   - `scripts/01_setup.sh --only=1 --rebuild` 成功，loader 耗时 457 秒，golden 库大小 2.4G。
   - `scripts/03_run_s1_t4_d300.sh` 成功，tpmC 为 61.2741，TPS 为 2.26941。

6. 事务与并发回归：
   - `transaction_test.py`：40/40。
   - `transaction_test_bonus.py`：40/40。
   - `concurrency_test.py`：60/60。
   - `concurrency_test_bonus.py`：初次为 15/20，失败点是 `phantom_read_test_4` 的范围查询输出顺序。
   - 完善范围索引选择后，`concurrency_test_bonus.py` 通过，20/20。

7. GoogleTest B+ 树测试：
   - `b_plus_tree_insert_test`：2/2 通过。
   - `b_plus_tree_delete_test`：3/3 通过。
   - `b_plus_tree_concurrent_test`：2/2 通过。
   - 删除测试依赖 Graphviz 的 `dot` 命令生成可视化图片；Graphviz 安装后，`b_plus_tree_delete_test` 可正常生成图片并通过断言。

## 14. 对性能的预期影响

索引优化的影响：

- 点查从全表扫描变为 B+ 树查找。
- 复合索引前缀查询从全表扫描变为范围扫描。
- TPC-C 中 `district`、`customer`、`stock`、`orders`、`new_orders` 等表访问会明显受益。

锁优化的影响：

- UPDATE/DELETE 避免先拿 S 锁再升级 X 锁。
- 减少 wait-die 策略下由于锁升级冲突导致的 abort。
- 对 Payment、NewOrder、Delivery 等写事务有帮助。

总体预期：

- TPS 和 tpmC 提升。
- abort 数量下降。
- 长尾延迟降低，尤其是原先由大表顺序扫描导致的高 p95 / p99 延迟。

B+ 树写入互斥锁的影响：

- 它会让同一棵索引上的并发插入、删除串行化，因此极端写密集场景下吞吐会低于细粒度 page latch。
- 但它避免了结构损坏和测试崩溃，是本实验中更稳妥的并发正确性方案。
- 报告中可以把它写成“正确性优先的保守实现”，并把 latch coupling 作为后续优化方向。

## 15. 后续建议

依赖安装完成后，建议按以下顺序继续验证：

```bash
cmake -B build -S .
cmake --build build --target rmdb tpcc_loader tpcc_driver b_plus_tree_insert_test b_plus_tree_delete_test -j$(nproc)
./build/bin/b_plus_tree_insert_test
./build/bin/b_plus_tree_delete_test
bash scripts/01_setup.sh --only=10 --rebuild
bash scripts/02_run_s10_t4_d60.sh
```

如果上述测试稳定，再运行：

```bash
bash scripts/03_run_s1_t4_d300.sh
```
