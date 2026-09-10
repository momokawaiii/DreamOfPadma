# TASK-025 节点操作与故事变化

- 英文原文：TASK-025-Node-Operations-and-Story.md
- 文档 ID：TASK-025
- 版本：0.2
- 状态：Backlog
- 上级：[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)
- Primary Role：World Content Systems
- Primary Agent：Ready 时指定唯一所有者；尚无实现写入者
- 分支／worktree：实现预检时指定 Local 功能分支；默认不建立 worktree

## 目标与当前价值

使占领、一处设施、熔炉转化、安全屋恢复、条件开路及一个双选项轶闻产生持久可见的世界变化。 本任务通过一个有界交付结果扩展已评审前置成果，不得把临时夹具变成未批准玩法规则。

## 必读

阅读 AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)、[ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md)、[MVP 决策](../MVPDecisionRegister.zh-CN.md)和 [PadmaWorld 契约](../../Architecture/Modules/PadmaWorld/README.zh-CN.md)。另读 `Docs/Rules/WorldState.md`、`Docs/Design/EN/04_LevelContentPlanner.md`、前置任务契约和下列路径适用的 Source／Content／Docs 嵌套规则。Agent 以英文来源为准。

来源：[PDF 卡牌、规则与画面提取](../MVPSourceExtraction.zh-CN.md)。旧契印沙盘召唤效果首版不实现；ACT 卡集缺失不以回合制角色卡补齐。

## 决策与 Ready 条件

- 已冻结：获接受 MVP／规则、独立 Encounter／ACT GAS 体系及配表、独立回合制／ACT 角色卡集合，以及共享身份／槽位但效果归各模式的基础技能卡。
- 已冻结：占位素材由用户导入，正式动画／渲染打磨后做。每个未接受数值在使用前均须由用户逐项确认，测试配置也不例外。
- 决策编号：D03, D12, D19 具体所有者与缺失信息见登记表。
- Backlog 表示已列入规划，并非可直接实施。将门槛答案记录到规范规则／架构来源和本报告，完成前置成果后再设 Ready。技术结构由 Architect 评审；玩法与数值选择由用户决定。
- 若关闭门槛需要修改写集外规范配对或结构 ADR，Coordinator 须在 Ready 前补入精确路径；这是计划内预检，不是任意修改权限。

## 范围与允许路径

- `Source/DreamOfPadma/Public/MVP/World/NodeOperations/`
- `Source/DreamOfPadma/Private/MVP/World/NodeOperations/`
- `Source/DreamOfPadma/Public/MVP/World/Story/`
- `Source/DreamOfPadma/Private/MVP/World/Story/`
- `Source/DreamOfPadma/Public/MVP/UI/NodeOperations/`
- `Source/DreamOfPadma/Private/MVP/UI/NodeOperations/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-025/`
- `Content/Padma/MVP/World/NodeOperationFixtures/`
- `Content/Padma/MVP/World/StoryFixtures/`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaWorld/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-025-Node-Operations-and-Story.md`
- `Docs/Production/Tasks/TASK-025-Node-Operations-and-Story.zh-CN.md`

排除：完整剧情、任意世界脚本、Widget 直接修改世界以及补猜免费恢复／兑换规则。 其他任务源代码／内容、上方未列共享配置、参考原型、生成目录和新独立 UE 模块均不在范围内。已有 Demo 代码通过公开契约消费；适配／迁移须有明确命名的写集。

## 依赖、交接与冲突

- 必要前置：[TASK-018](TASK-018-Calendar-and-Phase-Scheduler.zh-CN.md), [TASK-021](TASK-021-ABC-Sandbox-Operations.zh-CN.md), [TASK-024](TASK-024-Sandbox-Graph-and-Movement.zh-CN.md)。
- 条件前置：若上方尚未列出，任何随机结果前必须完成 TASK-005。
- 交付已评审公开命令、只读视图、定义夹具和测试证据。状态所有者实现 TASK-012 快照参与；UI 不拥有可变玩法状态。
- 模块 README 配对、共享头文件、地图、二进制资源和中央文件采用串行占用；其他所有者／构建使用期间不得修改。UE 构建／Editor／PIE 只有一条通道。
- 不授权可写子 Agent。可由一名有界只读 architect／reviewer 检查相关契约／差异。Primary Agent 负责结果；Coordinator 拥有共享集成状态和 Changelog。

## 验收标准

- [ ] 世界后果按事件身份保证幂等；重复灾变事件不能二次应用归属／资源／世界效果，恢复需保留已应用事件记录。
- [ ] 通过与节点／故事操作相同的校验、变更事件和快照边界，应用 TASK-019 灾变结果中获准世界后果；保留造成方归因，未知效果不部分修改状态。
- [ ] 记录选定节点条件／费用和故事后果；占领及设施变化通过已校验命令／事件发生。
- [ ] 熔炉执行获准操作后变为普通节点；安全屋恢复和条件道路展示费用及状态变化。
- [ ] 双选项轶闻改变标记和获准资源／威胁／信仰／路线后果；再次访问遵循已记录事件状态。
- [ ] 地形词缀产生一个获准且可见的规则效果；所有标记、归属、设施和事件历史都纳入快照恢复。
- [ ] 精确写集、同步任务／模块文档、必要检查和独立评审通过；没有以默认值填补未决规则。

## 验证与证据

前置／付款／重复操作测试与变化／恢复往返；用户观察占领、转化、恢复及两个不同故事夹具。

原生代码变化时编译 Editor 目标。对实际实现的狭窄自动化，使用 Scripts/RunTests.ps1，传入已验证 EngineRoot 和 TestFilter `DreamOfPadma.MVP.TASK025`；确认报告中的用例真正通过，而非只检查进程退出。运行 Scripts/AuditDocs.ps1、适用的 Scripts/ValidateProject.ps1 -Strict 及限定范围 git diff --check。不要仅为本任务建立大规模 UI 自动化。记录可复现用户 PIE 观察（书面证据可接受）、输入／配置 ID 和日志；图片／视频可补充。本契约不声称已观察运行或手工成功。

## 学习契约

| 目标 | 证据 | 状态 |
|---|---|---|
| 由命令产生持久变化 | 预测各故事选项后的地图与标记，再检查记录的事件。 | Not started |

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

## 关系事件交接

TASK-039 可经 TASK-034 组合消费本任务稳定的剧情结果事件 ID。输出事件／来源／选择身份，不在此实现关系成长或增益执行；重放可识别，关系消费者能够拒绝重复。D28 负责具体关系后果。TASK-037 消费当前设施／剧情／节点视图，不另存一份状态。
