# TASK-031 ACT 卡库与子弹时间

- 英文原文：TASK-031-ACT-Repository-and-Bullet-Time.md
- 文档 ID：TASK-031
- 版本：0.1
- 状态：Backlog
- 上级：[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)
- Primary Role：ACT Input / UI
- Primary Agent：Ready 时指定唯一所有者；尚无实现写入者
- 分支／worktree：实现预检时指定 Local 功能分支；默认不建立 worktree

## 目标与当前价值

把共享卡身份／卡槽视图接入 ACT 专属 Tab 卡库窗口、背景虚化、1／10 世界时间和合法数字槽位分发。 本任务通过一个有界交付结果扩展已评审前置成果，不得把临时夹具变成未批准玩法规则。

## 必读

阅读 AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)、[ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md)、[MVP 决策](../MVPDecisionRegister.zh-CN.md)和 [PadmaUI 契约](../../Architecture/Modules/PadmaUI/README.zh-CN.md)。另读 `Docs/Rules/Combat.md`、`Docs/Architecture/RuntimeFlow.md`、前置任务契约和下列路径适用的 Source／Content／Docs 嵌套规则。Agent 以英文来源为准。

来源：[PDF 卡牌、规则与画面提取](../MVPSourceExtraction.zh-CN.md)。旧契印沙盘召唤效果首版不实现；ACT 卡集缺失不以回合制角色卡补齐。

## 决策与 Ready 条件

- 已冻结：获接受 MVP／规则、独立 Encounter／ACT GAS 体系及配表、独立回合制／ACT 角色卡集合，以及共享身份／槽位但效果归各模式的基础技能卡。
- 已冻结：占位素材由用户导入，正式动画／渲染打磨后做。每个未接受数值在使用前均须由用户逐项确认，测试配置也不例外。
- 决策编号：D01, D09, D17, D23 具体所有者与缺失信息见登记表。
- Backlog 表示已列入规划，并非可直接实施。将门槛答案记录到规范规则／架构来源和本报告，完成前置成果后再设 Ready。技术结构由 Architect 评审；玩法与数值选择由用户决定。
- 若关闭门槛需要修改写集外规范配对或结构 ADR，Coordinator 须在 Ready 前补入精确路径；这是计划内预检，不是任意修改权限。

## 范围与允许路径

- `Source/DreamOfPadma/Public/MVP/Gameplay/ACT/RepositoryWindow/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/ACT/RepositoryWindow/`
- `Source/DreamOfPadma/Public/MVP/UI/ACTRepository/`
- `Source/DreamOfPadma/Private/MVP/UI/ACTRepository/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-031/`
- `Content/Padma/MVP/ACT/Repository/`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-031-ACT-Repository-and-Bullet-Time.md`
- `Docs/Production/Tasks/TASK-031-ACT-Repository-and-Bullet-Time.zh-CN.md`

排除：把 Encounter 每回合额度用于 ACT、硬暂停物理、改变 Encounter 时钟及补猜关闭／分页边界。 其他任务源代码／内容、上方未列共享配置、参考原型、生成目录和新独立 UE 模块均不在范围内。已有 Demo 代码通过公开契约消费；适配／迁移须有明确命名的写集。

## 依赖、交接与冲突

- 必要前置：[TASK-020](TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md), [TASK-030](TASK-030-ACT-GAS-Playable-Loop.zh-CN.md)。
- 条件前置：若上方尚未列出，任何随机结果前必须完成 TASK-005。
- 交付已评审公开命令、只读视图、定义夹具和测试证据。状态所有者实现 TASK-012 快照参与；UI 不拥有可变玩法状态。
- 模块 README 配对、共享头文件、地图、二进制资源和中央文件采用串行占用；其他所有者／构建使用期间不得修改。UE 构建／Editor／PIE 只有一条通道。
- 不授权可写子 Agent。可由一名有界只读 architect／reviewer 检查相关契约／差异。Primary Agent 负责结果；Coordinator 拥有共享集成状态和 Changelog。

## 验收标准

- [ ] 实现前记录切换／关闭／再进入、页数／边界、选牌／结束窗口行为及费用／冷却时钟策略。
- [ ] Tab 可见打开背景虚化五卡面板并将世界时间设为 1／10；移动／攻击请求被拒，合法卡牌／面板操作可用。
- [ ] 场景保留惯性；数字键映射当前页 1–5 槽，滚轮请求下一页，权威模式校验器拒绝非法输入。
- [ ] 共享卡绑定解析 ACT 定义而非 Encounter 技能；生命周期／付款使用获准所有者，不重复施加副作用。
- [ ] 关闭、死亡、战败、退出、切场和加载恢复时间缩放、虚化和输入状态，覆盖嵌套／错误路径；重新完成含出牌的整场战斗。
- [ ] 精确写集、同步任务／模块文档、必要检查和独立评审通过；没有以默认值填补未决规则。

## 验证与证据

状态／输入／时间恢复／分页边界测试及受控惯性观察；用户完成 Tab、槽位、分页、退出／错误和战斗结果路线。

原生代码变化时编译 Editor 目标。对实际实现的狭窄自动化，使用 Scripts/RunTests.ps1，传入已验证 EngineRoot 和 TestFilter `DreamOfPadma.MVP.TASK031`；确认报告中的用例真正通过，而非只检查进程退出。运行 Scripts/AuditDocs.ps1、适用的 Scripts/ValidateProject.ps1 -Strict 及限定范围 git diff --check。不要仅为本任务建立大规模 UI 自动化。记录可复现用户 PIE 观察（书面证据可接受）、输入／配置 ID 和日志；图片／视频可补充。本契约不声称已观察运行或手工成功。

## 学习契约

| 目标 | 证据 | 状态 |
|---|---|---|
| 输入门槛与独立时钟 | 预测子弹时间内哪些输入／时钟继续，再验证并解释恢复。 | Not started |

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
