# TASK-027 战斗路由与场景切换

- 英文原文：TASK-027-Battle-Routing-and-Travel.md
- 文档 ID：TASK-027
- 版本：0.2
- 状态：Backlog
- 上级：[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)
- Primary Role：Game Routing
- Primary Agent：Ready 时指定唯一所有者；尚无实现写入者
- 分支／worktree：实现预检时指定 Local 功能分支；默认不建立 worktree

## 目标与当前价值

依据双方约束解析敌方地点交战，并在沙盒／局部场景切换、镜头与控制模式转换中传递稳定上下文。 本任务通过一个有界交付结果扩展已评审前置成果，不得把临时夹具变成未批准玩法规则。

## 必读

阅读 AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)、[ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md)、[MVP 决策](../MVPDecisionRegister.zh-CN.md)和 [PadmaGame 契约](../../Architecture/Modules/PadmaGame/README.zh-CN.md)。另读 `Docs/Architecture/RuntimeFlow.md`、`Docs/Rules/Combat.md`、前置任务契约和下列路径适用的 Source／Content／Docs 嵌套规则。Agent 以英文来源为准。

来源：[PDF 卡牌、规则与画面提取](../MVPSourceExtraction.zh-CN.md)。旧契印沙盘召唤效果首版不实现；ACT 卡集缺失不以回合制角色卡补齐。

## 决策与 Ready 条件

- 已冻结：获接受 MVP／规则、独立 Encounter／ACT GAS 体系及配表、独立回合制／ACT 角色卡集合，以及共享身份／槽位但效果归各模式的基础技能卡。
- 已冻结：占位素材由用户导入，正式动画／渲染打磨后做。每个未接受数值在使用前均须由用户逐项确认，测试配置也不例外。
- 决策编号：D14 具体所有者与缺失信息见登记表。
- Backlog 表示已列入规划，并非可直接实施。将门槛答案记录到规范规则／架构来源和本报告，完成前置成果后再设 Ready。技术结构由 Architect 评审；玩法与数值选择由用户决定。
- 若关闭门槛需要修改写集外规范配对或结构 ADR，Coordinator 须在 Ready 前补入精确路径；这是计划内预检，不是任意修改权限。

## 范围与允许路径

- `Source/DreamOfPadma/Public/MVP/Game/BattleRouting/`
- `Source/DreamOfPadma/Private/MVP/Game/BattleRouting/`
- `Source/DreamOfPadma/Public/MVP/UI/BattleRouting/`
- `Source/DreamOfPadma/Private/MVP/UI/BattleRouting/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-027/`
- `Content/Padma/MVP/Definitions/BattleRoutes/`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-027-Battle-Routing-and-Travel.md`
- `Docs/Production/Tasks/TASK-027-Battle-Routing-and-Travel.zh-CN.md`

排除：假定每场 Encounter 自动进入 ACT、把模式仅视作镜头选项、通用世界流送框架及修改子任务地图。 其他任务源代码／内容、上方未列共享配置、参考原型、生成目录和新独立 UE 模块均不在范围内。已有 Demo 代码通过公开契约消费；适配／迁移须有明确命名的写集。

## 依赖、交接与冲突

- 必要前置：[TASK-017](TASK-017-Encounter-GAS-Playable-Loop.zh-CN.md), [TASK-021](TASK-021-ABC-Sandbox-Operations.zh-CN.md), [TASK-024](TASK-024-Sandbox-Graph-and-Movement.zh-CN.md), [TASK-025](TASK-025-Node-Operations-and-Story.zh-CN.md), [TASK-026](TASK-026-Ruler-World-AI.zh-CN.md), [TASK-035](TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md)。
- 条件前置：若上方尚未列出，任何随机结果前必须完成 TASK-005。
- 交付已评审公开命令、只读视图、定义夹具和测试证据。状态所有者实现 TASK-012 快照参与；UI 不拥有可变玩法状态。
- 模块 README 配对、共享头文件、地图、二进制资源和中央文件采用串行占用；其他所有者／构建使用期间不得修改。UE 构建／Editor／PIE 只有一条通道。
- 不授权可写子 Agent。可由一名有界只读 architect／reviewer 检查相关契约／差异。Primary Agent 负责结果；Coordinator 拥有共享集成状态和 Changelog。

## 验收标准

- [ ] ABC 控制单位进入敌方驻点会在任意日历阶段触发合法路线解析；地形／卡片和双方约束都是明确输入。
- [ ] 解析模式／地点／出生点／定义及故事后果身份；无效或缺失绑定报告失败，不部分消耗上下文或改变控制。
- [ ] Encounter 切换／返回使用其获准适配器；TASK-030 提供真实适配器前可用桩测试 ACT 路线契约，尚不可用的 ACT 进入必须可见失败。
- [ ] 镜头／控制权／输入上下文生命周期跟随当前模式；旧场景委托和失效战斗 ID 不能影响目标场景。TASK-028 添加事务化进入／返回。
- [ ] 以独立 ID／视图传递已选 ACT 阵容／装备和地形上下文；通过 ACT 阵容校验器解析卡／特性限制，不复用地图已部署回合制角色身份。
- [ ] 精确写集、同步任务／模块文档、必要检查和独立评审通过；没有以默认值填补未决规则。

## 验证与证据

双方约束夹具、缺失目标和过期上下文失败测试；PIE 沙盒进入 Encounter 再返回。真实 ACT 切换由 TASK-030／034 再次验证。

原生代码变化时编译 Editor 目标。对实际实现的狭窄自动化，使用 Scripts/RunTests.ps1，传入已验证 EngineRoot 和 TestFilter `DreamOfPadma.MVP.TASK027`；确认报告中的用例真正通过，而非只检查进程退出。运行 Scripts/AuditDocs.ps1、适用的 Scripts/ValidateProject.ps1 -Strict 及限定范围 git diff --check。不要仅为本任务建立大规模 UI 自动化。记录可复现用户 PIE 观察（书面证据可接受）、输入／配置 ID 和日志；图片／视频可补充。本契约不声称已观察运行或手工成功。

## 学习契约

| 目标 | 证据 | 状态 |
|---|---|---|
| 模式路由与表现 | 解释同一节点如何解析不同合法路线，而不把地图名当规则。 | Not started |

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

## 查看是独立意图

TASK-037 拥有地块查看进出和局部表现。本任务在批准的移动／操作结果后路由真实交战；单独打开节点场景不能虚构敌方占位触发或战前事务。D24 规定组合点击行为。通过公共契约共享稳定场地／上下文；场景可以承载后续战斗表现，不使每次访问都成为战斗。FPS 范围仍为 D26。

## 2026-09-09 已确认模型／移动交接

D24 确认查看地块不创建战斗／事务请求，只有实际获准的移动／操作结果才可能触发路由。FPS 不在首版可玩（D26）；Encounter 和 ACT 仍分别路由。
