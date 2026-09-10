# TASK-033 首个时代内容包

- 英文原文：TASK-033-First-Era-Content-Pack.md
- 文档 ID：TASK-033
- 版本：0.2
- 状态：Backlog
- 上级：[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)
- Primary Role：Level / Content / Numerical
- Primary Agent：Ready 时指定唯一所有者；尚无实现写入者
- 分支／worktree：实现预检时指定 Local 功能分支；默认不建立 worktree

## 目标与当前价值

装配并验证一个已配置七天内容包，覆盖已实现经济、合成、沙盒、独立战斗模式和结局。 本任务通过一个有界交付结果扩展已评审前置成果，不得把临时夹具变成未批准玩法规则。

## 必读

阅读 AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)、[ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md)、[MVP 决策](../MVPDecisionRegister.zh-CN.md)和 [PadmaWorld 契约](../../Architecture/Modules/PadmaWorld/README.zh-CN.md)。另读 `Docs/Design/EN/01_MainPlanner.md`、`Docs/Design/EN/04_LevelContentPlanner.md`、`Docs/Design/EN/05_NumericalPlanner.md`、前置任务契约和下列路径适用的 Source／Content／Docs 嵌套规则。Agent 以英文来源为准。

来源：[PDF 卡牌、规则与画面提取](../MVPSourceExtraction.zh-CN.md)。旧契印沙盘召唤效果首版不实现；ACT 卡集缺失不以回合制角色卡补齐。

## 决策与 Ready 条件

- 已冻结：获接受 MVP／规则、独立 Encounter／ACT GAS 体系及配表、独立回合制／ACT 角色卡集合，以及共享身份／槽位但效果归各模式的基础技能卡。
- 已冻结：占位素材由用户导入，正式动画／渲染打磨后做。每个未接受数值在使用前均须由用户逐项确认，测试配置也不例外。
- 决策编号：D02, D03, D11, D12, D13, D18, D19, D22, D23 具体所有者与缺失信息见登记表。
- Backlog 表示已列入规划，并非可直接实施。将门槛答案记录到规范规则／架构来源和本报告，完成前置成果后再设 Ready。技术结构由 Architect 评审；玩法与数值选择由用户决定。
- 若关闭门槛需要修改写集外规范配对或结构 ADR，Coordinator 须在 Ready 前补入精确路径；这是计划内预检，不是任意修改权限。

## 范围与允许路径

- `Source/DreamOfPadma/Public/MVP/ContentValidation/FirstEra/`
- `Source/DreamOfPadma/Private/MVP/ContentValidation/FirstEra/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-033/`
- `Content/Padma/MVP/ContentPacks/FirstEra/`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaWorld/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-033-First-Era-Content-Pack.md`
- `Docs/Production/Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md`

排除：新玩法机制、修改他人任务地图／模板、完整 49 天内容及未批准正式美术迁移。 其他任务源代码／内容、上方未列共享配置、参考原型、生成目录和新独立 UE 模块均不在范围内。已有 Demo 代码通过公开契约消费；适配／迁移须有明确命名的写集。

## 依赖、交接与冲突

- 必要前置：[TASK-023](TASK-023-Synthesis-UI.zh-CN.md), [TASK-025](TASK-025-Node-Operations-and-Story.zh-CN.md), [TASK-026](TASK-026-Ruler-World-AI.zh-CN.md), [TASK-029](TASK-029-Safe-Boundary-Save-Load.zh-CN.md), [TASK-031](TASK-031-ACT-Repository-and-Bullet-Time.zh-CN.md), [TASK-032](TASK-032-Run-Outcomes-and-Restart.zh-CN.md), [TASK-035](TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md), [TASK-037](TASK-037-Node-Inspection-and-Local-Scenes.zh-CN.md), [TASK-038](TASK-038-Global-Preparation-and-Bonus-UI.zh-CN.md), [TASK-039](TASK-039-Story-Relationships-and-Bonus-State.zh-CN.md)。
- 条件前置：若上方尚未列出，任何随机结果前必须完成 TASK-005。
- 交付已评审公开命令、只读视图、定义夹具和测试证据。状态所有者实现 TASK-012 快照参与；UI 不拥有可变玩法状态。
- 模块 README 配对、共享头文件、地图、二进制资源和中央文件采用串行占用；其他所有者／构建使用期间不得修改。UE 构建／Editor／PIE 只有一条通道。
- 不授权可写子 Agent。可由一名有界只读 architect／reviewer 检查相关契约／差异。Primary Agent 负责结果；Coordinator 拥有共享集成状态和 Changelog。

## 验收标准

- [ ] 编写两条用户批准且可复现的验收路线：低熵安全玩法与高熵收益／灾变玩法，包含造成方归因和可见世界／统治者后果。这不把合成失败变成奖励。
- [ ] 提供至少两张 A／两张 B／两张 C、足够支撑获准卡库表现的基础战斗卡、无奖励失败定义、玩家／统治者核心和 Boss。
- [ ] 提供约八节点及要求节点类型、一条条件路线、一个地形效果、轶闻选项、恢复／熔炉和统治者巡逻／响应夹具。
- [ ] 分别配置 Encounter 与 ACT 目录／配表实例和模式绑定，保持卡身份一致并独立验证模式数据。
- [ ] 七天节奏路线包含合成成功／无奖励失败、经济变化及两种战斗模式；两种胜利夹具和获准失败都可达到。
- [ ] 配置带版本，并记录种子、费用、数值观察、资源引用验证及基础动画／VFX／音频可读性；只调数据不改变规则。
- [ ] 精确写集、同步任务／模块文档、必要检查和独立评审通过；没有以默认值填补未决规则。

## 验证与证据

内容 ID／引用／模式绑定验证与有界数值夹具；游玩已配置路线并记录一次选择的可见世界后果。

原生代码变化时编译 Editor 目标。对实际实现的狭窄自动化，使用 Scripts/RunTests.ps1，传入已验证 EngineRoot 和 TestFilter `DreamOfPadma.MVP.TASK033`；确认报告中的用例真正通过，而非只检查进程退出。运行 Scripts/AuditDocs.ps1、适用的 Scripts/ValidateProject.ps1 -Strict 及限定范围 git diff --check。不要仅为本任务建立大规模 UI 自动化。记录可复现用户 PIE 观察（书面证据可接受）、输入／配置 ID 和日志；图片／视频可补充。本契约不声称已观察运行或手工成功。

## 学习契约

| 目标 | 证据 | 状态 |
|---|---|---|
| 内容配置与机制 | 修改允许的测试配置值，预测后果并指出保持相同的规则。 | Not started |

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

## 画面与关系内容验收

纳入 TASK-037／038／039 中用户批准的地块局部场景／模型映射、悬浮可见性样例、全局配置内容及最小具名关系／增益路径。查看人物／工事模型，返回后核对同一世界状态，并跟踪一条批准的关系来源到实际目标和增益总览。具体 NPC、布局、内容与数值仍为 D12／D19／D24–D28。不把会话草图当玩法样例，不擅加 FPS 战斗。
