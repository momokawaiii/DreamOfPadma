# TASK-035 ACT 角色卡与出战阵容

- 英文原文：TASK-035-ACT-Character-Cards-and-Roster.md
- 文档 ID：TASK-035
- 版本：0.2
- 状态：Backlog
- 上级：[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)
- Primary Role：ACT Roster / UI
- Primary Agent：Ready 时指定唯一所有者；尚无实现写入者
- 分支／worktree：实现预检时指定 Local 功能分支；默认不建立 worktree

## 目标与当前价值

提供独立 ACT 角色卡集合及战斗总设置，用于配置角色、武器和获准装备子集，并校验上下文可用性。 本任务通过一个有界交付结果扩展已评审前置成果，不得把临时夹具变成未批准玩法规则。

## 必读

阅读 AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)、[ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md)、[MVP 决策](../MVPDecisionRegister.zh-CN.md)和 [PadmaGame 契约](../../Architecture/Modules/PadmaGame/README.zh-CN.md)。另读 `Docs/Architecture/SaveSchema.md`、`Docs/Rules/Combat.md`、前置任务契约和下列路径适用的 Source／Content／Docs 嵌套规则。Agent 以英文来源为准。

来源：[PDF 卡牌、规则与画面提取](../MVPSourceExtraction.zh-CN.md)。旧契印沙盘召唤效果首版不实现；ACT 卡集缺失不以回合制角色卡补齐。

## 决策与 Ready 条件

- 已冻结：获接受 MVP／规则、独立 Encounter／ACT GAS 体系及配表、独立回合制／ACT 角色卡集合，以及共享身份／槽位但效果归各模式的基础技能卡。
- 已冻结：占位素材由用户导入，正式动画／渲染打磨后做。每个未接受数值在使用前均须由用户逐项确认，测试配置也不例外。
- 决策编号：D01, D02, D03, D20, D23 具体所有者与缺失信息见登记表。
- Backlog 表示已列入规划，并非可直接实施。将门槛答案记录到规范规则／架构来源和本报告，完成前置成果后再设 Ready。技术结构由 Architect 评审；玩法与数值选择由用户决定。
- 若关闭门槛需要修改写集外规范配对或结构 ADR，Coordinator 须在 Ready 前补入精确路径；这是计划内预检，不是任意修改权限。

## 范围与允许路径

- `Source/DreamOfPadma/Public/MVP/Core/ACTRoster/`
- `Source/DreamOfPadma/Private/MVP/Core/ACTRoster/`
- `Source/DreamOfPadma/Public/MVP/Game/ACTRoster/`
- `Source/DreamOfPadma/Private/MVP/Game/ACTRoster/`
- `Source/DreamOfPadma/Public/MVP/UI/BattleSettings/`
- `Source/DreamOfPadma/Private/MVP/UI/BattleSettings/`
- `Source/DreamOfPadma/Public/MVP/Gameplay/ACT/RosterData/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/ACT/RosterData/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-035/`
- `Content/Padma/MVP/UI/BattleSettings/`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.md`
- `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md`

排除：把已部署回合制卡当作同一张 ACT 角色卡、补猜阵容人数／费用／解锁策略，以及 ACT 攻击执行。 其他任务源代码／内容、上方未列共享配置、参考原型、生成目录和新独立 UE 模块均不在范围内。已有 Demo 代码通过公开契约消费；适配／迁移须有明确命名的写集。

## 依赖、交接与冲突

- 必要前置：[TASK-012](TASK-012-Run-State-and-Contracts.zh-CN.md), [TASK-019](TASK-019-Resource-and-Global-Ledgers.zh-CN.md), [TASK-020](TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md), [TASK-036](TASK-036-ACT-Authoring-Definitions.zh-CN.md), [TASK-040](TASK-040-Model-Assets-and-Scene-Bindings.zh-CN.md)。
- 条件前置：若上方尚未列出，任何随机结果前必须完成 TASK-005。
- 交付已评审公开命令、只读视图、定义夹具和测试证据。状态所有者实现 TASK-012 快照参与；UI 不拥有可变玩法状态。
- 模块 README 配对、共享头文件、地图、二进制资源和中央文件采用串行占用；其他所有者／构建使用期间不得修改。UE 构建／Editor／PIE 只有一条通道。
- 不授权可写子 Agent。可由一名有界只读 architect／reviewer 检查相关契约／差异。Primary Agent 负责结果；Coordinator 拥有共享集成状态和 Changelog。

## 验收标准

- [ ] 回合制角色卡用于沙盒部署；ACT 角色卡属于独立集合，具有独立稳定定义和模式绑定。
- [ ] 实现前记录阵容容量、选择／修改时点、解锁／持有、初始可用角色、费用及预设与本局存档归属。
- [ ] 战斗总设置 UI 选择并校验 ACT 阵容，对不可用／不完整选择提供可读错误，不修改已部署回合制单位。
- [ ] 公开入战契约传递带版本阵容状态及稳定角色／定义 ID；本任务使用获准上下文夹具和测试消费者验证。真实路由／ACT 消费由 TASK-027／030 负责。与触发地图单位的关系遵循用户已记录决策。
- [ ] D20 明确归类为本局拥有的状态通过参与者契约导出／恢复，并测试战败／退出／新局／存读行为。任何全局预设属于独立边界；不得假定全部战斗设置字段都参与局部战斗回滚。
- [ ] 地形／上下文限制具有明确作用对象：角色、武器、技能或特性。仅限制特性时，不能静默禁用整个角色；具体作用范围、时点和反馈在 D20 确认。
- [ ] 精确写集、同步任务／模块文档、必要检查和独立评审通过；没有以默认值填补未决规则。

## 验证与证据

独立卡片集合、角色／武器选择、非法阵容、地形受限特性、过期上下文和快照测试；用户配置阵容并在 ACT 入战上下文看到可用性。

原生代码变化时编译 Editor 目标。对实际实现的狭窄自动化，使用 Scripts/RunTests.ps1，传入已验证 EngineRoot 和 TestFilter `DreamOfPadma.MVP.TASK035`；确认报告中的用例真正通过，而非只检查进程退出。运行 Scripts/AuditDocs.ps1、适用的 Scripts/ValidateProject.ps1 -Strict 及限定范围 git diff --check。不要仅为本任务建立大规模 UI 自动化。记录可复现用户 PIE 观察（书面证据可接受）、输入／配置 ID 和日志；图片／视频可补充。本契约不声称已观察运行或手工成功。

## 学习契约

| 目标 | 证据 | 状态 |
|---|---|---|
| 部署单位与出战阵容 | 比较地图单位 ID 与配置的 ACT 阵容 ID，解释各自所属服务。 | Not started |

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


TASK-036 接管 ACT 静态角色／武器／技能定义、强类型技能表和 ACTCharacterCards 内容。先消费其已审查接口；本任务不重复定义这些类型。ACT/Data 与 Definitions 只放执行／效果配置，RosterData 只放阵容视图／适配器。角色／武器内容仍由用户填写；空模板不关闭可玩内容门禁。

## 全局配置组合

TASK-038 挂载本任务的 ACT 选择面板和校验阵容服务，不重复两者权威。TASK-036 保留静态定义。D27 在 D20 预设／本局归属门禁上补充可用性／修改／时间规则。关系增益来源经批准的服务契约进入，不把其状态移入 ACT 阵容。D26 回答前，FPS 保留自身尚未实现的能力边界。

## 2026-09-09 已确认模型／移动交接

复用 TASK-040 已添加到 ACT 角色定义的移动元数据，它不会把 ACT 阵容项变成已部署世界卡。模型／动画／技能表继续由 TASK-036 定义拥有，040 的 Game 适配器读取。本任务仍拥有真实玩家阵容命令和地形检查。D26 排除 FPS 玩法。
