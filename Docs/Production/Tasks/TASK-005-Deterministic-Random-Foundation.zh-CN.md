# TASK-005 确定性随机基础

- 英文原文（Agent 阅读）：`TASK-005-Deterministic-Random-Foundation.md`
- 文档 ID：`TASK-005`
- 版本：`0.1`
- 状态：`Review`
- 所属里程碑或集成 Goal：`M1 Core rules / 第一个实现切片`
- Primary Role：`Module Agent`
- Primary Agent：`module_worker`；只有本契约批准为 `Ready` 后才分配
- 分支/worktree：`feature/TASK-005-deterministic-random`，Local feature branch；当前一次一个功能/一个写入者策略下不使用独立 Worktree
- 任务模式：有边界的 Core 实现与自动化测试切片；不实现玩法公式或内容

## Goal

在现有 `DreamOfPadma` 模块内实现最小、与表现层无关的确定性随机上下文和追踪边界，并加入项目第一批自有 Unreal 自动化测试。证明相同 master seed、命名 stream、draw position 和样例身份能够复现相同序列与 trace，同时命名 stream 彼此隔离。不实现合成、日历、资源、存档、网络或玩法概率语义。

可观察结果是一份可编译的 Core 切片和可重复的自动化证据，后续规则服务可以在不依赖 UI、Actor、地图或万能 Manager 的情况下消费它。

## 为什么现在做

TASK-003 已关闭，并冻结了确定性计算元数据，包括 seed/stream/position 以及在需要时使用的输入身份。架构和数值基线要求结果可复现，但仓库还没有项目自有的玩法自动化测试。一个窄小的确定性 fixture 是风险最低的第一个实现切片，可以服务后续合成、战斗、AI、日历和回放工作，同时不必选择它们的开放规则。

## 首先阅读

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Production/Tasks/TASK-003-Core-Contracts.md`
- `Docs/Rules/Synthesis.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/SaveSchema.md`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaTests/README.md`
- `Source/AGENTS.md`
- `.codex/agents/module-worker.toml`

## 决策状态

### 已冻结输入

- Core 契约保持独立于 UMG、Niagara、具体 Actor、地图、输入设备和表现状态。
- 随机计算记录 master/run seed、命名 stream、事件或 command position、在需要时的输入定义身份/版本，以及 Debug/Test 构建中的 roll 证据。
- 使用相同 seed、stream、position 和输入重复同一确定性 fixture 时，必须得到相同结果；候选选择与成功/失败属于未来分开的随机步骤。
- 实现留在当前生成的 `DreamOfPadma` 模块内。不授权独立 UE 模块、插件、GAS 依赖、资产或中心配置变更。
- 第一批测试必须消费生产边界，不得创建第二套会漂移的假规则实现。

### 延期或开放输入

- 最终稳定 ID 命名空间、生产内容命名以及导入/版本策略。
- 长期随机算法/版本保证、跨引擎版本回放兼容性和存档序列化形状。
- 玩法分布、roll 数量、范围、概率夹断、合成候选过滤、Softmax、成功公式、Sigma 后果、战斗公式、AI 策略以及日历/资源策略。
- 网络权威、预览/执行集成、持久化、遥测格式以及任何按规则分配 stream 的策略。

这些项目仍由相应策划、数值、架构或用户 Owner 负责。如果本切片要通过验收必须做出其中一个决定，就停止并返回该决策；不得添加默认值。

## 范围

### 允许路径和写入集合

`module_worker` Primary Agent 只能编辑以下路径：

- `Source/DreamOfPadma/Public/Core/Random/PadmaDeterministicRandom.h`
- `Source/DreamOfPadma/Private/Core/Random/PadmaDeterministicRandom.cpp`
- `Source/DreamOfPadma/Private/Tests/Core/PadmaDeterministicRandomTests.cpp`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-005-Deterministic-Random-Foundation.md`
- `Docs/Production/Tasks/TASK-005-Deterministic-Random-Foundation.zh-CN.md`

任务创建和关闭时的集成负责人专属记录路径：

- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`

Primary Agent 不得编辑以上四个记录路径。

### 不在范围内

- `DreamOfPadma.Build.cs`、`.uproject`、新 UE 模块、插件、GAS、Config、地图、`.uasset`、生成目录或外部原型内容。
- 上述三个精确 Core/random 路径之外的任何源码文件。
- 日历转换、资源值或账本、合成候选/成功规则、战斗/AI roll、卡牌系统、存档/加载、回放、网络、UI、输入、Actor 或表现。
- 选择生产稳定 ID 命名空间、跨版本回放保证、产品随机分布、玩法概率或内容默认值。
- 改变 TASK-001 至 TASK-004、已接受的规范规则或任何 Open/Deferred 含义。
- Primary Agent 合并、打标签、推送、发布或修改共享 ProjectState/index。

## 依赖与集成顺序

- 必须先完成：`TASK-003` 已在提交 `45061f1` 中关闭；已集成的 ACT 边界位于 `b2f652b`，不影响本切片。
- 直接输入：首先阅读中列出的 Core 契约、确定性计算架构、合成规则、数值可复现指引以及模块/测试边界。
- 当前实现依赖：使用现有 `DreamOfPadma` 模块及其已批准依赖。不应也不得修改 Build.cs 或插件。
- 集成顺序：本双语任务契约必须先经过 Review 并批准为 `Ready`，Primary Agent 才能开始。实现分支要独立 Review；之后由 Integration Coordinator 更新共享索引/状态并集成结果。
- 使用顺序：后续合成、战斗、AI、日历、资源、存档、回放和网络任务，只能通过经过 Review 的公共契约消费本切片。公共契约变化必须返回串行集成 Review。
- 共享冲突：公共 Core 契约、`DreamOfPadma.Build.cs`、模块依赖、`Docs/00_INDEX*`、`Docs/ProjectState*`、地图、二进制资源以及 UE build/Editor/PIE 通道继续由 Integration Coordinator 串行维护。

## 独立 Worktree 决定

**初次执行不使用 Worktree。** 这是当前策略下一个功能、一个写入者的有边界切片。从获批的 `main` 状态创建 Local 分支 `feature/TASK-005-deterministic-random`。只有明确引入独立并发工作且写入集合仍然互斥时，才重新评估 Worktree。

## 委派计划

Primary Agent 对整个实现 Goal 负责。它可以请求有限的只读帮助，但不授权第二位写入者。

| Role | 工作包 | 权限 | 预期输出 |
|---|---|---|---|
| `reviewer` | 检查实际实现 diff、测试、Core 边界、未决规则保留情况，以及双语任务/模块文档。 | 只读 | P0-P3 问题、检查、剩余风险和 `Block`、`Conditional pass` 或 `Pass`。 |
| `architect` | 仅当实现需要新的公共 schema、模块依赖、存档形状或算法版本决定时介入。 | 只读 | 边界/ADR 建议；不提交补丁。 |

## 验收标准

- [ ] 精确实现和测试文件在现有 `DreamOfPadma` 模块内编译通过，不创建新模块或依赖。
- [ ] 公共随机上下文/trace 边界与表现层无关，不使用具体 Actor 或地图状态，也不引入万能 Manager 或隐藏的全局可变状态。
- [ ] 在同一获批构建/运行时中，两个新的上下文使用相同 master seed、命名 stream、draw position 和命名 fixture 输入时，产生相同结果序列和 trace。
- [ ] 命名 stream 相互隔离：消费一个 stream 不会改变同一 fixture 下另一个 stream 的结果或 position。
- [ ] Debug/Test trace 证据包含 seed、stream 身份、position、提供时的 fixture/输入身份，以及复现 fixture 所需的 roll 身份和值。
- [ ] 自动化测试使用生产边界、能被项目测试命令发现并通过，而且不加载地图或 Widget。
- [ ] Core 模块 README pair 和 TASK pair 保持同步，记录实现证据但不改变规范玩法含义。
- [ ] 没有 Open/Deferred 决策被转换为产品默认值，实现没有修改 Primary Agent 写入集合之外的文件。
- [ ] 独立 Reviewer 返回 `Pass`，所有必要检查通过，Integration Coordinator 在 ProjectState 中记录结果。

## 测试

本切片要求的自动化覆盖：

| 测试 | 必须观察到 | 约束 |
|---|---|---|
| 相同 fixture 回放 | 两个新的上下文使用相同 master seed、命名 stream、position 和 fixture 输入时，值与 trace 字段完全一致。 | 不断言产品概率或合成结果。 |
| 命名 stream 隔离 | 对 stream A 的抽取不会改变同一 fixture 下 stream B 的值或 position。 | 不使用全局随机单例，也不耦合无关 stream。 |
| Position 与 trace | 每次有意抽取只让记录的 position 前进一次，并输出可复现的 roll 身份和值证据。 | trace 是证据，不是玩法规则。 |
| 生产边界 | 测试调用未来服务使用的 Core 公共接口。 | 不得在测试中复制随机算法。 |

从仓库根目录运行：

- `Scripts/RunTests.ps1 -EngineRoot "<approved UE root>" -TestFilter "Project.Padma.Core.Random"`
- `Scripts/ValidateProject.ps1 -Strict`
- `Scripts/AuditDocs.ps1`
- `git diff --check`

在接受自动化结果前，UE Editor Target 必须编译通过。本有边界切片不要求玩法、合成、存档、网络、PIE 或打包测试。

## 验证方案

### 自动检查

- 将实际修改路径与七个 Primary Agent 路径比较；确认四个 Integration Coordinator 专属记录路径未被 Worker 修改。
- 运行指定自动化过滤器并记录报告路径/输出。
- 集成时运行严格项目验证、双语文档审计和工作区/暂存区空白检查。

### 手工检查

- 同时阅读公共头文件和测试 fixture，确认测试证明的是可复现性而不是某个实现细节。
- 确认随机状态不存放在 DataTable、Data Asset、INI、Actor、Widget 或万能 Manager 中。
- 确认 trace 足以复现 fixture，但没有声称跨版本回放，也没有决定玩法公式。

### 必需证据

- Editor 编译结果和过滤后的自动化测试输出。
- 实际修改路径列表和暂存 diff。
- 独立 Reviewer 报告。
- 更新后的任务完成报告以及 Coordinator 的 ProjectState/index 证据。

## 学习契约

| 目标 | 所需证据 | 状态 |
|---|---|---|
| 确定性 fixture 推理 | 用户预测哪些 seed/stream/position/input 字段必须一致，预测重复抽取后的 trace position，然后根据测试输出解释一次不一致。 | `Not started` |
| Core 边界所有权 | 用户说明随机服务为什么属于 Core，以及为什么排除 UI、Actor、地图和全局 Manager 状态。 | `Not started` |

- 学习者动手练习：实现 Agent 开始前，预测两个相同 fixture 和一个交错 stream fixture 的结果/trace 不变量；测试通过后，把预测与自动化输出比较。
- 迁移问题：未来合成需要新的 roll 或 trace 字段时，本任务能提供哪些部分，哪些玩法含义必须留在另一份已批准任务或决策中？

## 风险与恢复

- 实现可能意外承诺跨引擎版本回放稳定性。恢复：把验收限制在获批的构建/运行时；若持久化需要更多能力，另建版本决策。
- Worker 可能自行发明稳定 ID 命名空间、分布、概率或存档形状。恢复：停止并把问题返回给指定 Owner；只使用本任务的不透明 fixture。
- 随机状态可能泄漏到全局或表现层对象。恢复：拒绝 diff，让状态留在显式 Core 上下文/服务中。
- 可能出现新依赖或模块拆分的便利方案。恢复：停止，并在改变它之前要求架构/依赖任务和 ADR。
- 安全回退：revert 实现提交，或把分支返回到最后获批的集成提交；不得删除无关工作区变更。

## 完成报告

- 最终状态：`Review`；双语契约已创建并等待用户批准，尚未开始实现。
- Primary Agent 和 Role：等待批准 / `module_worker`。
- 修改文件：TASK-005 pair、双语索引和双语 ProjectState 路由已记录在任务创建集成变更中；实现文件保持未修改。
- 验收证据：双语 TASK pair 已包含 Goal、精确写入集合、非目标、依赖、测试、学习目标以及 Local/不使用 Worktree 的执行形态；两个索引链接和 ProjectState 路由均已存在；没有实现文件变化。
- 已运行检查及结果：`Scripts/AuditDocs.ps1` 通过 110 个 Markdown 文件和 55 对语言文件；`Scripts/ValidateProject.ps1 -Strict` 通过；`git diff --check` 和精确创建范围检查通过。
- 未运行检查及原因：编译、自动化、玩法、PIE、Cook 和打包检查等 Primary Agent 获批后再执行。
- 已解决或接受的 Review 问题：等待实现 Review。
- 剩余风险和未决问题：**延期或开放输入**中的全部项目按设计保持未决。
- 已更新的中英文文档：任务创建阶段进行中；实现必须同步更新模块 README 两个文件。
- Agent 产生的学习证据：等待。
- 用户产生的学习证据：等待 fixture 预测和复述。
- 集成提交或交接引用：等待用户批准和后续实现集成。
