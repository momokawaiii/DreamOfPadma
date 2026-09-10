# TASK-026 统治者世界 AI

- 英文原文：TASK-026-Ruler-World-AI.md
- 文档 ID：TASK-026
- 版本：0.1
- 状态：Backlog
- 上级：[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)
- Primary Role：Ruler AI
- Primary Agent：Ready 时指定唯一所有者；尚无实现写入者
- 分支／worktree：实现预检时指定 Local 功能分支；默认不建立 worktree

## 目标与当前价值

在共享沙盒上交付可解释的统治者巡逻、发现／潜行、增援和反攻策略。 本任务通过一个有界交付结果扩展已评审前置成果，不得把临时夹具变成未批准玩法规则。

## 必读

阅读 AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)、[ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md)、[MVP 决策](../MVPDecisionRegister.zh-CN.md)和 [PadmaGameplay 契约](../../Architecture/Modules/PadmaGameplay/README.zh-CN.md)。另读 `Docs/Design/EN/03_CombatAIPlanner.md`、`Docs/Design/EN/04_LevelContentPlanner.md`、前置任务契约和下列路径适用的 Source／Content／Docs 嵌套规则。Agent 以英文来源为准。

来源：[PDF 卡牌、规则与画面提取](../MVPSourceExtraction.zh-CN.md)。旧契印沙盘召唤效果首版不实现；ACT 卡集缺失不以回合制角色卡补齐。

## 决策与 Ready 条件

- 已冻结：获接受 MVP／规则、独立 Encounter／ACT GAS 体系及配表、独立回合制／ACT 角色卡集合，以及共享身份／槽位但效果归各模式的基础技能卡。
- 已冻结：占位素材由用户导入，正式动画／渲染打磨后做。每个未接受数值在使用前均须由用户逐项确认，测试配置也不例外。
- 决策编号：D03, D13 具体所有者与缺失信息见登记表。
- Backlog 表示已列入规划，并非可直接实施。将门槛答案记录到规范规则／架构来源和本报告，完成前置成果后再设 Ready。技术结构由 Architect 评审；玩法与数值选择由用户决定。
- 若关闭门槛需要修改写集外规范配对或结构 ADR，Coordinator 须在 Ready 前补入精确路径；这是计划内预检，不是任意修改权限。

## 范围与允许路径

- `Source/DreamOfPadma/Public/MVP/Gameplay/RulerAI/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/RulerAI/`
- `Source/DreamOfPadma/Public/MVP/UI/RulerStatus/`
- `Source/DreamOfPadma/Private/MVP/UI/RulerStatus/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-026/`
- `Content/Padma/MVP/Definitions/RulerAI/`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-026-Ruler-World-AI.md`
- `Docs/Production/Tasks/TASK-026-Ruler-World-AI.zh-CN.md`

排除：LLM 驱动 AI、ACT 敌人移动、完整战略规划器及由动画通知拥有的决策。 其他任务源代码／内容、上方未列共享配置、参考原型、生成目录和新独立 UE 模块均不在范围内。已有 Demo 代码通过公开契约消费；适配／迁移须有明确命名的写集。

## 依赖、交接与冲突

- 必要前置：[TASK-005](TASK-005-Deterministic-Random-Foundation.zh-CN.md), [TASK-015](TASK-015-Combat-Calculation-Contracts.zh-CN.md), [TASK-018](TASK-018-Calendar-and-Phase-Scheduler.zh-CN.md), [TASK-019](TASK-019-Resource-and-Global-Ledgers.zh-CN.md), [TASK-024](TASK-024-Sandbox-Graph-and-Movement.zh-CN.md), [TASK-025](TASK-025-Node-Operations-and-Story.zh-CN.md)。
- 条件前置：若上方尚未列出，任何随机结果前必须完成 TASK-005。
- 交付已评审公开命令、只读视图、定义夹具和测试证据。状态所有者实现 TASK-012 快照参与；UI 不拥有可变玩法状态。
- 模块 README 配对、共享头文件、地图、二进制资源和中央文件采用串行占用；其他所有者／构建使用期间不得修改。UE 构建／Editor／PIE 只有一条通道。
- 不授权可写子 Agent。可由一名有界只读 architect／reviewer 检查相关契约／差异。Primary Agent 负责结果；Coordinator 拥有共享集成状态和 Changelog。

## 验收标准

- [ ] 消费获准熵／灾变／世界事件，不重复计算 TASK-019 的归因或自行增加惩罚；统治者变化可观察、可按快照恢复。
- [ ] 记录首个统治者配置的发现、威胁增长、候选优先级和响应条件。
- [ ] 配置夹具可分别触发一次巡逻、隐蔽接近、被发现接近、增援和反攻。
- [ ] AI 读取公开世界视图、提交普通命令并展示候选／理由／所选行动日志；不直接修改 World 内部状态。
- [ ] 每日／事件威胁变化和随机同分决策可复现；快照恢复巡逻、待到达／响应工作及流位置。
- [ ] 精确写集、同步任务／模块文档、必要检查和独立评审通过；没有以默认值填补未决规则。

## 验证与证据

同种子决策与无合法行动策略夹具；用户推进世界并观察统治者可见响应。

原生代码变化时编译 Editor 目标。对实际实现的狭窄自动化，使用 Scripts/RunTests.ps1，传入已验证 EngineRoot 和 TestFilter `DreamOfPadma.MVP.TASK026`；确认报告中的用例真正通过，而非只检查进程退出。运行 Scripts/AuditDocs.ps1、适用的 Scripts/ValidateProject.ps1 -Strict 及限定范围 git diff --check。不要仅为本任务建立大规模 UI 自动化。记录可复现用户 PIE 观察（书面证据可接受）、输入／配置 ID 和日志；图片／视频可补充。本契约不声称已观察运行或手工成功。

## 学习契约

| 目标 | 证据 | 状态 |
|---|---|---|
| 可解释的确定性 AI | 预测威胁事件后哪个候选胜出，检查日志并解释优先级。 | Not started |

练习：用户亲自完成上方最小观察／修改，并解释一个失败案例。迁移：解释相同归属原则如何适用于下个依赖任务。Agent 产物与用户练习是分开的证据。

## 风险与恢复

风险：模式耦合、遗漏生命周期／快照参与或范围漂移。未决问题或写入冲突前停止相关实现并保留证据。恢复时经评审只回退本任务自身修改，不能删除生成或无关内容。最终集成将功能缺陷退回本所有者。

## 完成报告

- 最终状态：Backlog；契约已创建，实现尚未开始。
- 实现修改文件、检查、手工观察与评审：待完成。
- Agent 产出证据：仅本契约；不声称运行时或用户学习成果。
- 用户学习证据：待提供。
- Changelog 草稿：实现交接时必需，包含类别、可见结果、验证及剩余限制。
- 中英文配对：一并创建；后续每次修改均需同步。
- 集成／提交：实现时指定；不授权合并／推送。
