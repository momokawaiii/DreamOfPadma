# 多 Agent 端到端工作流示例

- 英文原文（Agent 阅读）：`Docs/Agent/WorkflowExample.md`

## 示例状态

这是一个用于说明 M1 的假设场景，不是活动 Backlog，也不代表已经授权创建 `TASK-003` 至 `TASK-006` 或编写代码。真正开工时必须重新验证确切路径和契约。

## 场景

集成结果是：**第一批核心规则基础能够在不依赖表现层的情况下表示日历推进、资源事务和可复现随机结果**。

这个结果太大，不适合交给一个没有拆分的 Agent；但必须先确定共享契约，才能安全并行实现。

## 阶段 1：集成契约 Goal

用户在保存项目的直接 checkout 中开启一个 Codex 任务，并赋予它集成协调者角色。

```text
Goal：准备并集成 M1 核心规则基础。
不要在这个 checkout 中实现模块代码。
先创建双语任务契约，冻结共享 ID/值类型和依赖方向，识别写入集合冲突，
只把可独立合并的工作路由到项目 worktree。使用 $padma-task-runner，
并要求合并前执行 $padma-review。
```

协调者可以提出这样的任务契约：

| 假设任务 | Goal | 执行形态 |
|---|---|---|
| `TASK-003 Core Contracts` | 冻结下游需要的共享 ID、值语义、错误/结果形态、所有权和测试要求 | 串行集成工作；共享公共契约 |
| `TASK-004 Calendar Slice` | 按固定的时段/日/时代/chapter 契约推进并复现日历 | TASK-003 后的独立写入 Goal |
| `TASK-005 Resource Ledger Slice` | 应用明确、可检查的资源事务，并按契约拒绝非法操作 | TASK-003 后的独立写入 Goal |
| `TASK-006 Deterministic Random Fixtures` | 从记录的种子和固定样例复现规则结果 | TASK-003 后的独立写入 Goal；可能要先于使用方合并 |

`TASK-003` 必须串行完成，因为所有下游 Goal 都使用它的公共契约。协调者获得系统策划和架构师的只读 Review，记录是否需要 ADR，就影响设计的问题取得用户批准，然后先提交冻结契约，再建立写入 worktree。

## 阶段 2：受控展开并行

`TASK-003` 冻结后，为可独立合并的 Goal 建立不同的 Codex 项目任务/worktree，并让它们都从已冻结的集成提交开始。Codex 管理的 worktree 初始处于 detached `HEAD`；任务结果需要长期作为分支提交或推送前，先使用 **Create branch here**。按初始策略，同时只启动两个写入 Goal：

| Worktree Goal | Primary Role | 独占写入集合 | 只读支持 |
|---|---|---|---|
| 日历切片 | `module_worker` | 日历拥有的实现/测试、模块 README 配对和任务报告 | `system_planner`、`architect`、`learning_tutor` |
| 确定性随机样例 | `module_worker` | 随机/样例拥有的实现/测试、模块 README 配对和任务报告 | `numerical_planner`、`architect`、`reviewer` |

资源账本 Goal 保持 `Ready`，等一个写入槽位空闲。并行是为了减少等待，而不是尽可能多地打开任务。

日历 Primary Agent 的开场提示词可以是：

```text
Primary Role：module_worker
Goal：只满足 Calendar Slice 任务，不做其他工作。
Task contract：Docs/Production/Tasks/TASK-004-Calendar-Slice.md
使用 $padma-task-runner。阅读规范英文时间规则和 PadmaCore 模块文档。
编辑前声明确切写入集合。保持固定日历层级和所有延期内容。
必要时委派只读规则/架构检查。如果必须修改已冻结公共契约，
或另一个 Goal 拥有某文件，就停止。返回测试、任务证据、双语更新、
学习证据和提交哈希。不得合并或推送。
```

## 阶段 3：有限 subagent 工作

写入前，日历 Primary Agent 可以并行运行两个互不依赖的只读工作包：

```text
Role：system_planner
Work package：从已验收 Time 规则推导日历不变量和边界样例。
Inputs：任务、Time 规则、ProjectState 和相关系统策划章节。
Output：带确切引用的不变量表、边界情况和未决内容。
Authority：只读。不得发明延期规则。
```

```text
Role：architect
Work package：按架构基线检查拟议的日历所有权、依赖、确定性状态和
存档/网络接口。
Inputs：任务以及点名的架构/模块文档。
Output：阻塞问题、ADR 需求和最小契约建议。
Authority：只读。不得编辑文件或改变游戏语义。
```

Primary Agent 用仓库证据核对两个结果。若都符合任务，则进入实现。任何一个结果如果揭示必须修改公共契约，Goal 就停止并把问题交回集成协调者；worker 不能临时顺手修改共享头文件。

随后 `learning_tutor` 把课程限制在最多两个目标，例如：

- 值类型日历状态与具体世界 Actor 的差别；
- 为什么带种子的确定性样例能复现规则行为。

用户先预测一次边界转换，在后续实现回合中亲自运行或观察批准的测试，然后解释结果。任务必须把用户证据与 Agent 生成的测试输出分开记录。

## 阶段 4：分支验证与独立 Review

Primary Agent 运行窄范围测试、必要的双语文档审计和 diff 范围检查，并填写完成报告。它只提交本任务的写入集合。

独立只读 `reviewer` 收到：

```text
使用 $padma-review。对照 main Review TASK-004 及其分支。
检查真实 diff 和测试证据，优先检查日历规则一致性、依赖方向、
确定性、存档影响、缺失边界样例、写入集合违规和双语漂移。
先报告问题，不得编辑。
```

只有所有阻塞问题都被修复或由有权负责人明确接受后，分支才能从 `Review` 进入 `Verified`。

## 阶段 5：集成

日历和确定性随机分支同时完成，并不能决定合并顺序。协调者按依赖顺序执行：

1. 确认每个已 Review 的提交哈希和干净 worktree；
2. 如果日历测试使用随机契约，先合并确定性随机样例；
3. 运行共享验证通道；
4. 合并日历；
5. 再次运行共享验证，或执行集成任务规定的组合检查；
6. 只有写入槽位和依赖都明确后才启动资源账本；
7. 更新双语任务状态和 `Docs/ProjectState.md`；
8. 提交集成结果；
9. 向用户展示 diff、检查、风险和学习证据；
10. 只有用户授权后才推送。

如果合并暴露语义冲突，应创建小型集成修复任务，或把归属 Goal 退回 `In Progress`。不得在 `main` 上做没有文档记录的妥协。

## 哪些并行，哪些串行

```text
串行：      冻结契约
               |
并行：      日历 Goal <----> 随机样例 Goal
               |                  |
串行：      独立 Review + 按依赖顺序集成
               |
下一批：    资源账本 Goal + 另一个不重叠 Goal
```

大量阅读的策划、架构检查、测试设计和 Review 很适合 subagent。独立实现 Goal 使用不同 worktree。共享公共模式、地图、`.uasset`、中心配置、迁移、Editor 会话和最终集成保持串行。

## 用户需要参与什么

用户不需要人工转发每一段对话，仓库和提交会携带长期事实。用户在真正重要的决定点介入：

- 批准或修改任务契约与玩法含义；
- 在真实设计选项中做选择；
- 亲自完成计划中的学习练习和复述；
- 判断可玩/手工结果；
- 授权集成推送或里程碑标签。
