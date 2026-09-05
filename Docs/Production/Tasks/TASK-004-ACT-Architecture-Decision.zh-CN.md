# TASK-004 ACT 架构决策集成

- 英文原文（Agent 阅读）：`TASK-004-ACT-Architecture-Decision.md`
- 文档 ID：`TASK-004`
- 版本：`0.1`
- 状态：`Done`
- 上级里程碑或集成 Goal：`M1 Gameplay 架构 / 未来 ACT 边界`
- Primary Role：`Integration Coordinator`
- Primary Agent：当前 Integration Coordinator
- 分支/worktree：`main`，由集成负责人串行维护的文档变更；不使用独立 worktree
- 任务模式：仅集成和 Review 架构决策；不实现代码、资产、插件或依赖

## Goal

把现有 ACT 架构与 ADR 草稿变成一项可追踪、双语、经过 Review 的架构决策：使用本 TASK 接管《未来 ACT 开发 Agent 契约》和 ADR-0002，同步 PadmaGameplay 模块文档、文档索引和项目状态，保留全部 Open/Deferred 玩法决定，完成独立只读 Review，并且只提交本任务的文档范围。

可观察结果是一份完成集成的文档基线：明确 Encounter 优先交付、未来 ACT 所有权、C++/数据驱动权威边界以及 GAS/插件门禁，但不授权实现 ACT。

## 为什么现在做

ACT 契约和 ADR 已经存在于工作区，也已从模块文档链接，但尚未被 TASK 契约接管、尚未作为完成的集成工作记录进 `ProjectState.md`、尚未完成独立 Review，也尚未提交。若已接受的架构只存在于未跟踪工作区中，未来 Agent 的启动依据和决策来源就会含糊不清。

## 首先阅读

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Rules/Combat.md`
- `Docs/Design/EN/03_CombatAIPlanner.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md`
- `Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.md`

## 决策状态

### 已冻结输入

- Encounter 是当前玩法实现优先项；ACT 仍是后续 RealTimeAction 路线。
- ACT 卡牌仓库已接受的 MVP 行为保持为：`Tab`、背景虚化、1/10 世界时间、拒绝移动/攻击输入、保留场景惯性，以及每页五个可见卡槽。
- 未来 ACT 架构采用 C++ 为主、数据驱动的方向。权威玩法不能转移到 UI、动画 Notify、Gameplay Cue、INI 或 Data-only Blueprint。
- Unreal 内置 GAS 与未来方向兼容，但本任务不启用 GAS。外部玩法插件需要独立 ADR。
- 外部 Combat 原型仅是参考证据，不是 Padma 规则、依赖、资产或默认数值的来源。
- 本架构决策只有文档权威。未来 ACT 实现需要自己的已批准 `Ready` TASK，并且必须满足 Encounter 前置条件。

### 延期或开放输入

- 第一个 ACT 垂直切片、确切技能、Ability System Component 所有权、授予/移除策略和网络基线。
- 最终 Gameplay Tag 分类、Attribute Set、伤害/Effect 数值所有权、叠层、阶段名称、取消矩阵、打断优先级和输入缓冲语义。
- 确切 `CharacterACTCatalog`、DataTable Row、`AbilityDefinition` 和 `ACTSequence` 模式；导入/版本策略；`Scope` 含义；载荷生命周期。
- ACT 持续/切换行为、资源消耗/冷却、页面边界、总页数、卡牌到技能合法性、Motion Warping 细节，以及策划基线中已有的全部 Open/Deferred 项。
- 这些决定仍归相应策划、系统、数值、架构或用户 Owner。本任务不得自行补全。

## 范围

### 允许路径和写入集合

- `Docs/Production/Tasks/TASK-004-ACT-Architecture-Decision.md`
- `Docs/Production/Tasks/TASK-004-ACT-Architecture-Decision.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.zh-CN.md`
- `Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.md`
- `Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.zh-CN.md`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`

### 不在范围内

- 任何 C++、Blueprint、地图、`.uasset`、Config、Build、插件、引擎、生成文件、测试实现、GAS 依赖或 ACT 运行时实现。
- 改变已接受玩法含义，或解决任何 Open、Proposed、待确认、仅供试玩验证或 Deferred 项。
- 实现 Encounter、ACT、卡牌生命周期、战斗公式、技能、动画、输入、UI、网络、存档、导入或内容资产。
- 改变 TASK-001、TASK-002 或 TASK-003 的状态或语义。
- 提交无关工作区变更、合并、创建标签、推送或发布。

## 依赖与集成顺序

- 必须先完成：提交 `cbb4da4` 中已接受的 MVP 策划与架构基线；TASK-002 已为 `Done`。
- 直接授权：用户在 2026-09-05 要求用正式双语架构决策 TASK 接管 ACT/ADR 变更、补充项目状态、完成 Review 并提交。
- 使用方：未来 Encounter 规划、未来 ACT TASK、PadmaGameplay、PadmaGame、PadmaUI、PadmaEditor、PadmaTests 和架构 Reviewer。
- 集成顺序：本项文档决策可以在 TASK-003 关闭前集成，但不能绕过 TASK-003，也不授权 Implementation Agent。TASK-003 仍是后续 Core 实现 TASK 的串行 Core 契约门槛。
- 共享冲突：索引和 ProjectState 包含既有无关工作区变更。必须保留这些变更，并且只暂存 TASK-004 所有的 hunk。所有共享状态写入仍由 Integration Coordinator 串行执行。

## 委派计划

| Role | 工作包 | 权限 | 预期输出 |
|---|---|---|---|
| `reviewer` | 按本契约、规范架构/规则、Open/Deferred 保留情况、双语一致性和暂存路径隔离，Review 实际 TASK-004 diff。 | 只读 | P0-P3 问题、已执行检查、剩余风险和 `Block`、`Conditional pass` 或 `Pass`。 |

## 验收标准

- [x] 本 TASK 中英文文件的身份、版本、状态、范围、验收和完成含义一致。
- [x] ADR-0002 写明架构 Owner 和 TASK-004，ACT 开发契约写明 TASK-004。
- [x] ACT 契约、ADR 和 PadmaGameplay README 一致要求 Encounter 优先，并要求另行批准 ACT 实现 TASK。
- [x] C++/数据驱动权威边界、内置 GAS 门禁、外部插件 ADR 要求和表现层非权威边界与架构基线一致。
- [x] 所有未决玩法、模式、依赖、网络、存档、输入和内容决定都明确保持 Open 或 Deferred。
- [x] 双语索引链接 TASK-004，双语 ProjectState 记录已接受的纯文档决策和经由 TASK-003 的正确下一门槛。
- [x] TASK-004 提交不包含 TASK-001/TASK-003 文件、源码、资产、配置、Build、插件、生成文件或实现文件。
- [x] 独立只读 Review 返回 `Pass`，或者所有阻塞问题都已修复并复审。
- [x] 文档审计、严格项目验证、空白检查和暂存范围检查通过。

## 验证方案

### 自动检查

- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1`
- `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict`
- `git diff --check` 和 `git diff --cached --check`
- 将 `git diff --cached --name-only` 与十二个允许路径比较。
- 确认没有暂存源码、资产、配置、Build、插件、生成文件、TASK-001 或 TASK-003 文件。

### 手工检查

- 把 ACT/Encounter 表述追溯到 `Combat.md`、已接受战斗策划基线和架构文档。
- 对照每一份发生变化的英文 Markdown 和中文镜像，确认含义一致。
- 确认 `Accepted` 只表示架构文档已经接受，不能被理解成 ACT/GAS 实现权限。
- 独立于 Worker 叙述检查暂存 diff。

### 必需证据

- 审计和严格验证输出。
- 确切暂存路径列表和暂存 diff 摘要。
- 只读 Reviewer 问题和最终验收状态。
- 本次文档集成提交 hash。

## 学习契约

| 目标 | 所需证据 | 状态 |
|---|---|---|
| ADR 权威与 TASK 执行权限的区别 | 用户解释为什么已接受 ADR 在没有另一份 `Ready` TASK 时仍不授权实现。 | `Not started` |
| 共享契约的串行集成 | 用户指出为什么共享索引/状态/架构变更必须先由 Integration Coordinator 串行集成，之后才能启动下游 Primary Agent。 | `Not started` |

- 学习者亲手练习：指出一个未来 ACT 决策应写入 ADR，以及一个有边界的实现结果应写入 TASK。
- 迁移问题：TASK-003 关闭后，在 `module_worker` 可以修改 Core 实现文件前，还必须存在什么契约？

## 风险与恢复

- 混合工作区变更可能被误提交。恢复：按路径和 hunk 暂存、检查暂存 diff，并让无关变更保持未暂存。
- `Accepted` 可能被误认为实现权限。恢复：在双语文档中保留纯文档状态、TASK 门禁和 ACT Agent 停止条件。
- 后续工作可能静默复制原型或启用 GAS/插件。恢复：停止实现，并要求相关 `Ready` TASK、依赖 Review 和 ADR。
- 安全回退：revert TASK-004 集成提交；不得抹除无关工作区变更。

## 完成报告

- 最终状态：`Done`。
- Primary Agent 和 Role：当前 Integration Coordinator / Integration Coordinator。
- 修改文件：以上十二个允许的 Markdown 路径。
- 验收证据：暂存集合与十二个允许路径完全一致；所有新链接的 TASK、ACT 契约和 ADR 文件都存在；独立复审以无问题的 `Pass` 结束。
- 已运行检查及结果：`AuditDocs.ps1` 检查 108 个 Markdown 文件和 54 对语言文件并通过；`ValidateProject.ps1 -Strict`、`git diff --check`、`git diff --cached --check`、链接目标存在性和暂存范围断言均通过。
- 未运行检查及原因：玩法、编译、PIE、Cook 和打包检查不属于本纯文档任务。
- 已解决或接受的 Review 问题：第一次完成的 Review 将 ProjectState 中英文件里的分支策略改写判定为无关变更并阻塞；该 hunk 已移除，同一位独立 Reviewer 对更新后的暂存 diff 返回 `Pass`。
- 剩余风险和未决问题：**延期或开放输入**中的全部项目按设计保持未决。
- 已更新的中英文文档：是。
- Agent 产生的学习证据：本 TASK 区分了 ADR 权威与实现授权，并记录共享契约的串行集成边界。
- 用户产生的学习证据：尚未记录。
- 集成提交或交接引用：本次有范围的 TASK-004 文档提交；最终 hash 由 Integration Coordinator 在交接中报告。
