# TASK-011 完整 MVP 任务簇与独立 GAS 模式

- 英文原文：TASK-011-Full-MVP-Task-Cluster.md
- 文档 ID：TASK-011
- 版本：0.1
- 状态：In Progress
- Primary Role / Agent：Integration Coordinator / 当前文档主 Agent
- 上级里程碑：完整可玩 MVP
- 工作区：当前 Local 检出；仅文档；不建立独立 worktree
- 授权：用户于 2026-09-08 要求编写此任务簇，并确认 Encounter 与 ACT 都使用 GAS，但具有不同的模式体系和配表。

## 目标

交付 TASK-012 至 TASK-034 双语契约、ADR-0004、决策登记表，以及同步的生产和架构入口，覆盖完整七天 MVP。本任务是此前建议新增的 24 项中的第 1 项；运行时实现由子任务交付。

已接受的 MVP 包含七时代章节结构中的一个七天时代、约八个共享沙盘节点、两张 A／两张 B／两张 C 与基础战斗卡及一个无奖励失败定义、资源操作、合成、世界变化和统治者响应、完整 Encounter 与 ACT 战斗路线、安全边界持久化，以及两种已确认的整局胜利。

## 必读

阅读 AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、Docs/Agent/Workflow.md、Docs/Agent/CodexSetup.md、Docs/Production/TaskTemplate.md、Docs/Rules 四份规则、五份已接受的英文策划基线、ProgramArchitecture.md、DataDrivenArchitecture.md、ModuleMap.md、RuntimeFlow.md、SaveSchema.md、PadmaGameplay README/ACTDevelopmentContract，以及相关 TASK-003 至 TASK-010。英文是规范来源。

## 决策状态

- 两种模式都采用 Unreal 原生 GAS，各自保有独立目录、行结构、执行体系、时钟、模式状态、校验器和模式专属技能／效果。
- 共享身份或卡库映射，不表示共享一个技能定义、AttributeSet、冷却、序列或存活技能实例。
- 同卡效果／数值的独立程度、初版美术验收与临时数值配置权限正在向用户确认，见决策登记表。
- 战斗交付顺序仍为 Encounter 优先；TASK-007 至 TASK-010 保留固定夹具范围；首次随机功能之前需要 TASK-005。
- 未决玩法含义由用户决定。任务文档可以完整交付，而运行时实施仍受前置条件约束。

## 范围与独占写集

本任务只允许修改以下 Markdown 配对：

- Docs/Production/Tasks/TASK-011-Full-MVP-Task-Cluster.md 及 .zh-CN.md 伴随文件。
- 下方交付矩阵列出的 TASK-012 至 TASK-034 的全部配对。
- Docs/Production/MVPDecisionRegister.md 及 .zh-CN.md。
- Docs/Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md 及 .zh-CN.md。
- Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.md 及 .zh-CN.md，仅增加明确的局部取代说明。
- Docs/Rules/Combat.md 及 .zh-CN.md。
- Docs/Design/EN/03_CombatAIPlanner.md 与 Docs/Design/ZH/03_CombatAIPlanner.md。
- Docs/Architecture/ProgramArchitecture.md、DataDrivenArchitecture.md、ModuleMap.md、RuntimeFlow.md、SaveSchema.md，各自连同 .zh-CN.md 伴随文件。
- Docs/Architecture/Modules/PadmaGameplay/README.md、ACTDevelopmentContract.md，各自连同 .zh-CN.md 伴随文件。
- Docs/Production/Roadmap.md、Docs/00_INDEX.md、Docs/ProjectState.md、Docs/Changelog.md，各自连同 .zh-CN.md 伴随文件。

Source、Content、Config、构建文件、TASK-005 至 TASK-010、生成目录、.agents 和 .codex 不在写集内。预检时另一 Integration Coordinator 正在收尾 TASK-007 并更新共享状态；该写入者结束前不修改 ProjectState 和 Changelog，之后重读并保留其修改。本任务不包含运行时工作、合并、推送、标签或发布。

## 交付矩阵与依赖顺序

在编写契约时填入任务矩阵和可执行顺序。任务编号是身份，并不保证数字顺序就是依赖顺序。

## 子任务执行契约

每个子任务列出一名 Primary Agent、狭窄功能路径、前置条件、决策门槛、强类型交接、可观察验收、检查、恢复方式和至多两个学习目标。子任务以 Backlog 开始，直到前置成果和相关决策已记录；晋级状态不能补猜规则。本次请求批准文档创建，不代表运行时已完成。

按当前运行方式，每个实现任务采用一个 Local 功能分支。UE 构建／Editor／PIE 只有一条执行通道。共享头文件、模式结构、模块 README、地图、构建设置和最终装配修改串行进行。只读专家可在有界工作包下与主写入者同时审阅；此任务簇不授权第二名写入者。

每个功能随实现交付最小视图／日志证据；若拥有战斗可修改的本局状态，必须参与快照导出／恢复。后期集成只连接已批准公开契约，将功能缺陷退回所有者。TASK-034 不负责修改子任务源代码。

## 委派与评审

Docs/Architecture/AGENTS.md 要求架构和受影响规则评审。主 Agent 起草时，只读架构 Agent 检查模式分离和局部取代；另一个只读 Reviewer 检查完整契约、依赖图、精确路径、决策门槛和中英文一致性。两者均无写权限。

## 验收标准

- [ ] TASK-011 至 TASK-034 都有同步双语契约、具体验收与归属。
- [ ] 两种模式使用 GAS，且不强制共享表、效果配置、状态机或运行时实例。
- [ ] 每个已接受 MVP 要求都映射到实现任务与最终验收。
- [ ] 依赖无环，运行时门槛标明决策所有者。
- [ ] 本任务没有覆盖并发的 TASK-007／008／配置工作。
- [ ] 相关规则、架构、ADR、索引、路线图和状态一致。
- [ ] 文档配对、链接／任务图、严格项目验证和范围内空白检查通过。
- [ ] 独立评审无未解决的 P0–P2 问题。

## 验证计划

运行 Scripts/AuditDocs.ps1、Scripts/ValidateProject.ps1 -Strict、任务链接／依赖／写集审计，以及限定范围的 git diff --check。AuditDocs 只检查伴随文件存在性，不验证链接或翻译含义；后两者单独检查。仅文档修改无需 UE 编译或 PIE。

## 学习契约

| 目标 | 证据 | 状态 |
|---|---|---|
| 共享基础设施与独立模式契约 | 预测修改 ACT 专属行会否影响 Encounter，检查契约并解释一种禁止依赖。 | Not started |
| 按依赖交付 | 沿前置条件追踪一个可玩里程碑，指出一项待用户决策。 | Not started |

用户练习：选一个任务，追踪其输入、拥有的状态和验收现象。迁移问题：未来 FPS 如何复用身份而不继承 ACT 序列？用户证据单独记录；写完文档不代表学习已达成。

## 风险与恢复

主要风险是隐性跨模式耦合、依赖环、提前设计过多结构和共享状态写入冲突，须在实现前通过文档解决。恢复时检查当前归属，只回退本任务获准的文档修改，并保留其他文件。

## 完成报告

- 最终状态：In Progress。
- 修改文件与独立评审：待完成。
- 运行时／UE 检查：未运行；本任务仅文档。
- Agent 产出证据：正在起草契约。
- 用户学习证据：尚未提供。
- 集成：工作区文档交付；没有合并或远端操作。
