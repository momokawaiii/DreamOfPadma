# TASK-006 可玩原型垂直切片集群

- 英文原文（Agent 阅读）：`TASK-006-Playable-Prototype-Cluster.md`
- 文档 ID：`TASK-006`
- 版本：`0.1`
- 状态：`Done`
- 上级里程碑或集成 Goal：`达到原型功能水平 / 固定 fixture 垂直切片`
- Primary Role：`Integration Coordinator`
- Primary Agent：本串行契约设计 Goal 由 `Integration Coordinator` 负责；子任务实现 Agent 只能在批准后分配
- 分支/worktree：集成负责人工作区；不使用独立 Worktree
- 任务模式：仅双语架构决策和下游 TASK 集群设计；不实现源代码、资源、配置或测试

## Goal

创建并审核一个双语任务集群，使项目能够达到 `E:\2026ue\padma` 所展示的最小功能水平：进入游戏、选择一个 SLG 地块、进入其对应的局部地图、显示一张角色卡、出牌召唤角色，并查看角色技能列表。

本任务的可观察结果是一个可供批准为 Ready 的集群契约和 ADR，而不是运行时代码。集群刻意使用一个固定 playtest fixture，并保留所有未决玩法规则。

## 为什么现在做

默认地图 PIE 启动已经验收，而当前项目仍只有生成的运行时模块。用户希望在完整 M1 随机/测试基础之前先看到可玩的表现路径。参考项目可以提供交互和表现参考，但不是 DreamOfPadma 的架构来源。

## 首先阅读

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/ModuleMap.md`
- `Docs/Architecture/SaveSchema.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaTests/README.md`
- `Docs/Production/Tasks/TASK-003-Core-Contracts.md`
- `Docs/Production/Tasks/TASK-004-ACT-Architecture-Decision.md`
- `Docs/Production/Tasks/TASK-005-Deterministic-Random-Foundation.md`
- 只读参考 `E:\2026ue\padma`

## 决策状态

### 已冻结输入

- 默认地图 PIE 启动已经通过验收。
- 旧原型只作为参考；干净的 DreamOfPadma 项目才是实现来源。
- 稳定身份与显示名、资源路径、Actor 指针和地图坐标分离。
- 状态变化遵循 `command -> validation -> authoritative state change -> typed result/event`。
- UI 展示状态并发送指令，不拥有世界或玩法事实。
- 第一切片可以使用一个明确的 fixture 映射和占位表现资源。
- 当前生成的 `DreamOfPadma` 模块继续作为临时实现归属；不授权永久模块拆分、GAS 或外部玩法插件。

### 延期或开放输入

- 通用世界图路线、移动成本、资源成本、归属、变化和敌我共同战斗路线结算。
- 沙盘和局部场景是否共用 World Partition，以及最终关卡流送方案。
- 卡牌生命周期、牌库/抽牌规则、奖励/失败语义、合成、随机结果和存档/回滚实现。
- Encounter 行动条优先级、响应/打断窗口、未决窗口的出牌额度和技能执行语义。
- ABC 卡在局部战斗中的合法性、ACT 实现、GAS 所有权、最终导入管线、正式 ID 命名空间、资源授权和网络权威。
- PIE 内 Debug 面板和大规模自动化 UI 测试。

如果实现任务必须依赖其中某项决定才能验收，必须停止并返回指定 Owner；不得自行增加默认值。

## 范围

### 允许路径和写入集合

本契约设计任务只能修改：

- `Docs/Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.md`
- `Docs/Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.zh-CN.md`
- `Docs/Production/Tasks/TASK-006-Playable-Prototype-Cluster.md`
- `Docs/Production/Tasks/TASK-006-Playable-Prototype-Cluster.zh-CN.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.zh-CN.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.zh-CN.md`
- `Docs/Production/Tasks/TASK-009-Card-Play-and-Character-Summon.md`
- `Docs/Production/Tasks/TASK-009-Card-Play-and-Character-Summon.zh-CN.md`
- `Docs/Production/Tasks/TASK-010-Playable-Prototype-Integration.md`
- `Docs/Production/Tasks/TASK-010-Playable-Prototype-Integration.zh-CN.md`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`

TASK-006 不允许修改任何 `Source/`、`Content/`、`Config/`、Build、生成目录或测试实现文件。

### 不在范围内

- 任何运行时 C++、蓝图、地图、`.uasset`、输入、UMG、自动化测试或打包实现。
- 修改已接受规则，或决定任何 Open/Proposed/Deferred/playtest-only 内容。
- 批准 TASK-005、启动实现 Agent、分配 Worktree、合并、打标签或推送。

## 依赖与集成顺序

### 子 TASK 集群

| TASK | 结果 | 依赖 | Primary 所有权 |
|---|---|---|---|
| `TASK-007` | 一个 Demo SLG 地块可以被选择，并产生跨场景切换上下文。 | `TASK-006` 批准 | World/Game 组合 |
| `TASK-008` | 一个局部 Encounter 场景加载上下文，拥有 Demo 角色/技能定义，并暴露召唤/只读查询边界。 | `TASK-007` 批准 | Gameplay |
| `TASK-009` | 卡牌和技能面板发送指令；出一张 Demo 卡后召唤角色并显示技能。 | `TASK-008` 批准 | UI 与输入 |
| `TASK-010` | 默认入口和完整手工 PIE 路径完成集成并留下证据。 | `TASK-007` 至 `TASK-009` 独立 Review 通过 | Integration Coordinator |

这些任务现在作为一个集群设计，但初次实现必须串行。它们共享一个生成模块、地图/二进制资源、中心构建配置和一条 UE build/Editor/PIE 通道；本契约不授权并行实现。

`TASK-005 Deterministic Random Foundation` 保持 `Review`，不是本集群的前置，因为本集群没有随机结果。待可玩切片完成，或后续获批功能确实需要确定性随机时，再重新评估。

## 委派计划

不授权可写 subagent。用户批准前，Integration Coordinator 可以请求以下只读检查：

| Role | 工作包 | 权限 | 预期输出 |
|---|---|---|---|
| `architect` | 检查 fixture、切场边界、临时模块选择以及依赖/ADR 影响。 | 只读 | 阻塞项或边界确认。 |
| `level-content-planner` | 检查一个 Demo 节点和一个对应局部场景是否足够，同时不决定正式世界图。 | 只读 | Fixture 与表现范围 Review。 |
| `reviewer` | 检查本契约及子任务的范围、所有权、语言同步和 Open/Deferred 保留。 | 只读 | P0-P3 问题和验收状态。 |

## 验收标准

- [x] ADR-0003 和 TASK-006 存在语义同步的英文/中文配对。
- [x] TASK-007 至 TASK-010 各自有一个可观察 Goal、精确写入集合、非目标、依赖、验收、测试、学习目标和 Worktree 决定。
- [x] 子任务写入集合不重叠于源代码目录、UI 资源目录、地图、中心配置、模块文档或集成状态。
- [x] 固定 fixture 足以展示目标路径，同时不实现通用路线、战斗、随机、存档或卡牌生命周期规则。
- [x] 集群明确保留所有相关 Open/Proposed/Deferred/playtest-only 内容。
- [x] TASK-005 没有被暗中修改或变成前置。
- [x] 索引和 ProjectState 将 ADR-0003 标记为已接受、TASK-006 标记为已完成；TASK-006 没有授权实现 Agent。
- [x] 本设计任务没有修改源代码、资源、配置、Build、生成目录或测试实现文件。

## 验证方案

### 自动检查

- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1`：所有 Markdown 配对和链接通过。
- `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict`：项目结构有效。
- `git diff --check`：没有空白错误。
- 变更路径审计确认只有上方列出的 Markdown 路径发生变化。

### 手工检查

- 从集群 Goal 追踪目标路径到 TASK-007、TASK-008、TASK-009 和 TASK-010。
- 确认 UI 不拥有权威状态，切场上下文使用稳定 ID，而不是 Actor 指针或坐标作为身份。
- 确认参考项目只用于识别可见行为，不作为实现来源。
- 确认 TASK-005 保持独立，且子任务不需要未决规则。

### 必需证据

- 文档审计、严格验证和空白检查输出。
- 精确变更路径列表。
- Coordinator 设计 Review 及其问题/条件。
- 用户批准后，子任务才能设为 `Ready` 或启动实现 Agent。

## 学习契约

| 目标 | 所需证据 | 状态 |
|---|---|---|
| 垂直切片拆分 | 用户能把每个可见步骤映射到一个子任务，并解释为什么任务可以一起设计却要串行实现。 | `Not started` |
| Fixture 与正式规则的区别 | 用户能指出一个 Demo fixture 和一个必须留在实现范围外的 Open/Deferred 规则。 | `Not started` |

- 学习者亲手练习：画出 `地块选择 -> 切场上下文 -> 局部状态 -> 卡牌指令 -> 召唤结果 -> 技能视图`，并标出每一段状态的 Owner。
- 迁移问题：当 Demo 需要第二个地块或第二张卡时，哪些内容应成为数据定义，哪些新行为需要另一个已批准 TASK？

## 风险与恢复

- 集群可能膨胀为完整游戏实现。恢复：保持一个节点、一个场景、一张卡、一个角色和只读技能；扩展另建任务。
- 子任务可能把 fixture 变成正式玩法规则。恢复：退回 Review，标记为 playtest-only，或请求正式设计决策。
- 可能并行修改共享地图、资源或 Build。恢复：停止写入者并保持串行顺序。
- 参考项目资源可能带有不可用依赖或授权不确定性。恢复：使用占位资源，另建明确的资源迁移任务。

## 完成报告

- 最终状态：`Done`；设计 Review 已完成，用户已于 2026-09-07 批准。
- Primary Agent 和 Role：Integration Coordinator / Integration Coordinator。
- 修改文件：ADR-0003 配对、TASK-006 至 TASK-010 配对、索引配对和 ProjectState 配对。
- 验收证据：AuditDocs.ps1 已通过 122 个 Markdown 文件和 61 对语言文件；ValidateProject.ps1 -Strict 已通过；git diff --check 已通过；变更路径审计仅包含计划中的双语文档和 ADR；没有 Source/Content/Config/Build/生成目录/测试实现路径发生变化。
- 已运行检查及结果：pwsh -NoProfile -File E:\2026ue\DreamOfPadma\Scripts\AuditDocs.ps1 通过；pwsh -NoProfile -File E:\2026ue\DreamOfPadma\Scripts\ValidateProject.ps1 -Strict 通过；git -c safe.directory=E:/2026ue/DreamOfPadma -C E:\2026ue\DreamOfPadma diff --check 通过；双语配对、必需章节、占位符和子任务写入集合只读审计通过。
- 未运行检查及原因：运行时、编译、自动化、PIE 和打包不属于本设计任务。
- 已解决或接受的 Review 问题：Coordinator 设计 Review 结论为“有条件 Pass”：集群使用一个固定 fixture，依赖串行，子任务实现路径不重叠，中心文件所有权明确，Open/Deferred 输入得到保留。ADR-0003 已接受，TASK-006 已完成。TASK-007 至 TASK-010 仍需分别放行；TASK-006 未启动实现 Agent。
- 剩余风险和未决问题：**延期或开放输入**中的所有项目按设计保持未决。
- 已更新的中英文文档：设计补丁后已同步。
- Agent 产生的学习证据：仅有契约拆分证据。
- 用户产生的学习证据：等待。
- 集成提交或交接引用：等待用户提交并推送的未提交工作区文档；未启动实现 Agent。
