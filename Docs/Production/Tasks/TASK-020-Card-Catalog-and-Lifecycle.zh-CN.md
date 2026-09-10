# TASK-020 卡牌目录与生命周期

- 英文原文：TASK-020-Card-Catalog-and-Lifecycle.md
- 文档 ID：TASK-020
- 版本：0.1
- 状态：Backlog
- 上级：[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)
- Primary Role：Core Cards
- Primary Agent：Ready 时指定唯一所有者；尚无实现写入者
- 分支／worktree：实现预检时指定 Local 功能分支；默认不建立 worktree

## 目标与当前价值

把固定卡牌视图扩展为经过校验的 ABC／基础卡目录与权威牌堆／生命周期状态，提供共享卡身份到不同战斗模式定义的映射。 本任务通过一个有界交付结果扩展已评审前置成果，不得把临时夹具变成未批准玩法规则。

## 必读

阅读 AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)、[ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md)、[MVP 决策](../MVPDecisionRegister.zh-CN.md)和 [PadmaCore 契约](../../Architecture/Modules/PadmaCore/README.zh-CN.md)。另读 `Docs/Design/EN/02_SystemPlanner.md`、`Docs/Rules/Combat.md`、前置任务契约和下列路径适用的 Source／Content／Docs 嵌套规则。Agent 以英文来源为准。

来源：[PDF 卡牌、规则与画面提取](../MVPSourceExtraction.zh-CN.md)。旧契印沙盘召唤效果首版不实现；ACT 卡集缺失不以回合制角色卡补齐。

## 决策与 Ready 条件

- 已冻结：获接受 MVP／规则、独立 Encounter／ACT GAS 体系及配表、独立回合制／ACT 角色卡集合，以及共享身份／槽位但效果归各模式的基础技能卡。
- 已冻结：占位素材由用户导入，正式动画／渲染打磨后做。每个未接受数值在使用前均须由用户逐项确认，测试配置也不例外。
- 决策编号：D01, D03, D09, D22 具体所有者与缺失信息见登记表。
- Backlog 表示已列入规划，并非可直接实施。将门槛答案记录到规范规则／架构来源和本报告，完成前置成果后再设 Ready。技术结构由 Architect 评审；玩法与数值选择由用户决定。
- 若关闭门槛需要修改写集外规范配对或结构 ADR，Coordinator 须在 Ready 前补入精确路径；这是计划内预检，不是任意修改权限。

## 范围与允许路径

- `Source/DreamOfPadma/Public/MVP/Core/Cards/`
- `Source/DreamOfPadma/Private/MVP/Core/Cards/`
- `Source/DreamOfPadma/Public/MVP/Game/CardCatalog/`
- `Source/DreamOfPadma/Private/MVP/Game/CardCatalog/`
- `Source/DreamOfPadma/Public/MVP/UI/CardRepository/`
- `Source/DreamOfPadma/Private/MVP/UI/CardRepository/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-020/`
- `Content/Padma/MVP/Definitions/Cards/`
- `Content/Padma/MVP/UI/CardRepository/`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-020-Card-Catalog-and-Lifecycle.md`
- `Docs/Production/Tasks/TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md`

排除：跨 Encounter／ACT 的万能技能表、全部升级百科及补猜回牌概率。 其他任务源代码／内容、上方未列共享配置、参考原型、生成目录和新独立 UE 模块均不在范围内。已有 Demo 代码通过公开契约消费；适配／迁移须有明确命名的写集。

## 依赖、交接与冲突

- 必要前置：[TASK-005](TASK-005-Deterministic-Random-Foundation.zh-CN.md), [TASK-012](TASK-012-Run-State-and-Contracts.zh-CN.md), [TASK-018](TASK-018-Calendar-and-Phase-Scheduler.zh-CN.md), [TASK-019](TASK-019-Resource-and-Global-Ledgers.zh-CN.md), [TASK-040](TASK-040-Model-Assets-and-Scene-Bindings.zh-CN.md)。
- 条件前置：若上方尚未列出，任何随机结果前必须完成 TASK-005。
- 交付已评审公开命令、只读视图、定义夹具和测试证据。状态所有者实现 TASK-012 快照参与；UI 不拥有可变玩法状态。
- 模块 README 配对、共享头文件、地图、二进制资源和中央文件采用串行占用；其他所有者／构建使用期间不得修改。UE 构建／Editor／PIE 只有一条通道。
- 不授权可写子 Agent。可由一名有界只读 architect／reviewer 检查相关契约／差异。Primary Agent 负责结果；Coordinator 拥有共享集成状态和 Changelog。

## 验收标准

- [ ] 为正式卡状态／付款／生命周期服务实现 TASK-012 的模式中立 provider 端口。Encounter 与 ACT 是消费者，不是该服务的依赖。执行用户确认的 D22d 映射 A→F、B→D、C→R；冲突标注修订前拒绝导入，不自行改写数值或效果。
- [ ] 区分沙盒部署的回合制角色卡、战斗总设置选择的 ACT 角色卡及基础非 A 技能卡；基础技能卡保持共享身份／槽位映射，各模式效果分别配置。
- [ ] 实现获准的手牌／牌堆／弃牌／返回／冷却／焚毁子集、手牌／耗尽边界和日终转换；随机部分使用命名流。
- [ ] 卡身份和可见卡槽映射选择明确模式定义；缺失或错误模式绑定可见失败，不使用另一模式的行。交付 TASK-017 及后续 ACT 适配器要求的公开卡状态／出牌／付款／生命周期 provider，并提供正式路径测试。
- [ ] TASK-020 拥有回合制／ABC 卡生命周期和共享基础技能卡契约；TASK-035 独占 ACT 角色集合、可用状态与阵容。ACT 角色卡不自动进入 ABC 合成、沙盒部署或本牌堆生命周期。模式费用／时钟／效果由获准所有者维护；快照导出／恢复覆盖本任务的位置、附着、生命周期和随机位置。
- [ ] 精确写集、同步任务／模块文档、必要检查和独立评审通过；没有以默认值填补未决规则。

## 验证与证据

牌堆守恒、非法转换、种子抽牌／重现、模式绑定和保存状态往返测试；用户观察抽牌／出牌／返回及卡片映射。

原生代码变化时编译 Editor 目标。对实际实现的狭窄自动化，使用 Scripts/RunTests.ps1，传入已验证 EngineRoot 和 TestFilter `DreamOfPadma.MVP.TASK020`；确认报告中的用例真正通过，而非只检查进程退出。运行 Scripts/AuditDocs.ps1、适用的 Scripts/ValidateProject.ps1 -Strict 及限定范围 git diff --check。不要仅为本任务建立大规模 UI 自动化。记录可复现用户 PIE 观察（书面证据可接受）、输入／配置 ID 和日志；图片／视频可补充。本契约不声称已观察运行或手工成功。

## 学习契约

| 目标 | 证据 | 状态 |
|---|---|---|
| 定义、实例与发动 | 把卡移到另一牌堆，预测稳定身份或模式绑定是否应变化。 | Not started |

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

## 可选关系贡献输入

卡牌／生命周期投影仅为 D28 选定且归本任务所有的目标消费 TASK-012 中立来源／修正端口。TASK-039 提供真实关系资格／来源，034 在集成后注入。先用明确服务样例，不依赖 039。012 冻结目标／时钟／版本及唯一应用所有者；校验资格，拒绝重复／不支持贡献。如果受影响标量归其他层，通过其批准契约传递，不能重复应用。本节准备接口，不在此选择增益目标、幅度或叠加规则。

## 2026-09-09 已确认模型／移动交接

全部正式 ABC／共享基础卡定义包含 TASK-040 三态移动结构及已填写标记。各卡政策／条件来自批准内容，不按类型／模型擅定默认值。本任务提供由 034 接给 037 的正式定义—模型／技能适配器，不把 Demo 适配器当完整卡库。D03 现允许有依据的 mycard.pdf 明确且不冲突数值。
