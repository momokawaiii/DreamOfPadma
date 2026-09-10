# TASK-012 本局状态与共享契约

- 英文原文：TASK-012-Run-State-and-Contracts.md
- 文档 ID：TASK-012
- 版本：0.1
- 状态：Backlog
- 上级：[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)
- Primary Role：Game Composition
- Primary Agent：Ready 时指定唯一所有者；尚无实现写入者
- 分支／worktree：实现预检时指定 Local 功能分支；默认不建立 worktree

## 目标与当前价值

提供一个本局／会话边界，具备稳定 ID、强类型命令／结果／事件、定义加载校验，以及带版本的状态参与者导出／恢复契约。 本任务通过一个有界交付结果扩展已评审前置成果，不得把临时夹具变成未批准玩法规则。

## 必读

阅读 AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)、[ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md)、[MVP 决策](../MVPDecisionRegister.zh-CN.md)和 [PadmaGame 契约](../../Architecture/Modules/PadmaGame/README.zh-CN.md)。另读 `Docs/Architecture/SaveSchema.md`、前置任务契约和下列路径适用的 Source／Content／Docs 嵌套规则。Agent 以英文来源为准。

来源：[PDF 卡牌、规则与画面提取](../MVPSourceExtraction.zh-CN.md)。旧契印沙盘召唤效果首版不实现；ACT 卡集缺失不以回合制角色卡补齐。

## 决策与 Ready 条件

- 已冻结：获接受 MVP／规则、独立 Encounter／ACT GAS 体系及配表、独立回合制／ACT 角色卡集合，以及共享身份／槽位但效果归各模式的基础技能卡。
- 已冻结：占位素材由用户导入，正式动画／渲染打磨后做。每个未接受数值在使用前均须由用户逐项确认，测试配置也不例外。
- 决策编号：D04 具体所有者与缺失信息见登记表。
- Backlog 表示已列入规划，并非可直接实施。将门槛答案记录到规范规则／架构来源和本报告，完成前置成果后再设 Ready。技术结构由 Architect 评审；玩法与数值选择由用户决定。
- 若关闭门槛需要修改写集外规范配对或结构 ADR，Coordinator 须在 Ready 前补入精确路径；这是计划内预检，不是任意修改权限。

## 范围与允许路径

- `Source/DreamOfPadma/Public/MVP/Core/Contracts/`
- `Source/DreamOfPadma/Private/MVP/Core/Contracts/`
- `Source/DreamOfPadma/Public/MVP/Game/Session/`
- `Source/DreamOfPadma/Private/MVP/Game/Session/`
- `Source/DreamOfPadma/Public/MVP/Game/DefinitionLoading/`
- `Source/DreamOfPadma/Private/MVP/Game/DefinitionLoading/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-012/`
- `Content/Padma/MVP/Definitions/Bootstrap/`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-012-Run-State-and-Contracts.md`
- `Docs/Production/Tasks/TASK-012-Run-State-and-Contracts.zh-CN.md`

排除：玩法公式、自动抽取全部 Demo 代码、磁盘存档和具体模式技能。 其他任务源代码／内容、上方未列共享配置、参考原型、生成目录和新独立 UE 模块均不在范围内。已有 Demo 代码通过公开契约消费；适配／迁移须有明确命名的写集。

## 依赖、交接与冲突

- 必要前置：[TASK-010](TASK-010-Playable-Prototype-Integration.zh-CN.md), [TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md), [TASK-040](TASK-040-Model-Assets-and-Scene-Bindings.zh-CN.md)。
- 条件前置：若上方尚未列出，任何随机结果前必须完成 TASK-005。
- 交付已评审公开命令、只读视图、定义夹具和测试证据。状态所有者实现 TASK-012 快照参与；UI 不拥有可变玩法状态。
- 模块 README 配对、共享头文件、地图、二进制资源和中央文件采用串行占用；其他所有者／构建使用期间不得修改。UE 构建／Editor／PIE 只有一条通道。
- 不授权可写子 Agent。可由一名有界只读 architect／reviewer 检查相关契约／差异。Primary Agent 负责结果；Coordinator 拥有共享集成状态和 Changelog。

## 验收标准

- [ ] 将模式中立的卡实例／槽位／查询／合法出牌／付款／生命周期 provider 端口冻结为 Core 值契约。TASK-017／030 消费，TASK-020 提供正式实现，TASK-034 注入；端口不依赖具体 Gameplay 技能、Actor 或最终卡定义行。
- [ ] 测试场景生命周期中保持本局身份；重复进入不会创建第二个权威状态所有者。
- [ ] 在部分初始化前拒绝缺失 ID、重复定义及无法解析的模式引用；使用 TASK-007／008／009 夹具时通过明确适配器。
- [ ] 区分定义 ID、持有卡实例 ID 和战斗实体 ID；回合制与 ACT 角色身份域独立。共享身份／槽位仅适用于基础非 A 技能卡，不包含角色卡、存活技能或冷却状态。全部逻辑 ID 与 Actor／ASC／技能／效果句柄分离，不强制统一模式定义结构。
- [ ] 确定参与者注册、带版本导出／恢复和缺失参与者错误，使后续战斗可修改状态不会静默漏出回滚。
- [ ] 精确写集、同步任务／模块文档、必要检查和独立评审通过；没有以默认值填补未决规则。

## 验证与证据

会话重建／重复进入、无效定义及参与者快照往返自动化；在 PIE 查看保留的本局／上下文 ID。

原生代码变化时编译 Editor 目标。对实际实现的狭窄自动化，使用 Scripts/RunTests.ps1，传入已验证 EngineRoot 和 TestFilter `DreamOfPadma.MVP.TASK012`；确认报告中的用例真正通过，而非只检查进程退出。运行 Scripts/AuditDocs.ps1、适用的 Scripts/ValidateProject.ps1 -Strict 及限定范围 git diff --check。不要仅为本任务建立大规模 UI 自动化。记录可复现用户 PIE 观察（书面证据可接受）、输入／配置 ID 和日志；图片／视频可补充。本契约不声称已观察运行或手工成功。

## 学习契约

| 目标 | 证据 | 状态 |
|---|---|---|
| 状态归属与身份 | 预测换地图后保留哪些值，再比较 ID 并解释状态所有者。 | Not started |

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

## 中立增益来源与修正契约

下游计算开工前，冻结独立于表现的值类型和只读查询端口，描述带版本的效果来源／贡献：稳定来源／增益身份、目标领域／定义身份、范围／时钟／上下文、资格和贡献身份。中立契约不包含 GAS 句柄、Actor 指针、任意可执行载荷或擅定幅度。D28 选择实际支持目标及叠加／时机／数值语义，D04 记录技术结构。

TASK-039 产生关系来源／资格数据。选定目标由既有权威消费：019 资源操作、020 卡牌／生命周期投影、022 合成计算或 015 战斗计算。每个选定标量／效果登记唯一消费者，避免同一贡献被多层重复应用。消费者先使用明确注入的服务样例，034 再注入 039 的真实服务；不依赖后做的关系实现。未知领域、重复贡献身份和不兼容版本明确失败。把端口及生产／消费矩阵加入契约验收和验证证据，不能只由未来消费者提到一个接口名称。

## 2026-09-09 已确认模型／移动交接

卡牌契约复用 TASK-040 独立于表现的 FPadmaCardMobilityDefinition，不另建三态枚举。未来实例／节点／成组命令仍由本任务冻结，不采用预览键作为本局结构。D29 要求明确卡牌实例和 A＋附着 B 的原子移动结果。
