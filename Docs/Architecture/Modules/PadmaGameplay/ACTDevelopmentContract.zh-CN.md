# 未来 ACT 开发 Agent 契约

- 文档 ID：ARCH-GAMEPLAY-ACT-001
- 版本：0.2
- 状态：架构契约已接受；实现延期
- 英文原文（Agent 阅读）：ACTDevelopmentContract.md
- Owner：Gameplay 模块 Agent
- 决策记录：../../../Decisions/ADR-0002-Encounter-First-ACT-Preparation.zh-CN.md
- TASK 契约：../../../Production/Tasks/TASK-004-ACT-Architecture-Decision.zh-CN.md
- 上级文档：README.zh-CN.md、../../DataDrivenArchitecture.zh-CN.md、../../ProgramArchitecture.zh-CN.md

## 1. 目的与权限

本文档是未来 ACT RealTimeAction 开发的实现边界。它在实现前记录已经达成的架构共识，使后续 Agent 不需要从聊天记录中重建共识，也不会未经评审就照搬外部 Combat 原型。

本文档**不授权现在实现 ACT**。当前交付顺序仍是先做 Encounter。只有明确批准的任务把一个有边界的 ACT 切片标记为 `Ready`，且其 Encounter 前置条件已经满足，才能开始 ACT 工作。

策划和规则文档仍然是玩法含义的权威来源。规则只要标记为“开放”“提案”“待确认”或延期，Agent 就必须保留该状态；如果实现选择会替用户补全答案，Agent 必须停止并请求决策。

## 2. 必读文档与任务门槛

未来 ACT 任务开始前，Primary Agent 必须阅读：

1. `AGENTS.md`、`Docs/00_INDEX.md` 和 `Docs/ProjectState.md`。
2. 已批准的具体 `TASK-xxx` 契约。
3. `Docs/Rules/Combat.md` 和相关的已验收策划基线。
4. `Docs/Architecture/ProgramArchitecture.md`、`DataDrivenArchitecture.md` 和 `RuntimeFlow.md`。
5. 本文档、PadmaGameplay 模块 README 和 ADR-0002。
6. 每个可写目录中的嵌套 `AGENTS.md`。

任务必须声明 Goal、精确允许路径、排除范围、依赖、验收标准、自动/手工测试、所需证据和不超过两个学习目标。一个可写 Primary Agent 对任务负责；除非另行授权，探索和 Review 始终只读。

## 3. 当前仓库事实

- `Source/DreamOfPadma/` 仍是共享运行时模块；`PadmaGameplay` 是逻辑边界，不是独立 Unreal 模块。
- GAS 目前不是项目依赖，也没有实现 ACT C++ 框架或 ACT 内容资产。
- Enhanced Input 已可用，但 ACT 输入模式和逐角色绑定尚未批准。
- `GASCompanion` 与 `AuroraDevs_UGC` 属于外部参考原型，不是 Padma 依赖。
- Epic Launcher 安装版 Unreal Engine 足以编译项目玩法代码。只有单独批准的任务要修改或深度调试引擎源码时，才需要源码版引擎。

未来任务必须重新验证这些事实，因为仓库和引擎状态可能变化。

## 4. 已接受的架构方向

未来 ACT 采用 C++ 为主、数据驱动的方向：

- C++ 负责权威规则、发动校验、状态转换、目标选择、命中结算、伤害施加和清理。
- ACT 自有 C++ Gameplay Ability 父类负责 ACT 内部公共生命周期；Encounter 拥有独立技能／执行契约。
- C++ Ability Task 负责可复用的异步时序和序列解释。
- `AbilityDefinition` Primary Data Asset 负责一个技能的完整不可变定义。
- `ACTSequence` Primary Data Asset 负责序列节点、转换、阶段动作和动画时序引用。
- ACT 内逐角色 DataTable 共用强类型 C++ RowStruct；它们不规定 Encounter 行结构。
- Data-only Ability Blueprint 可以选择原生类并提供面向资产的默认值，但不得重复实现公共生命周期或权威规则。
- 当 GAS 是已批准执行路径时，Gameplay Effect 表达消耗、冷却、属性、标签、状态和伤害变化。
- Gameplay Cue 只承载视听表现，不能成为伤害或状态的权威来源。

这是目标边界，不代表可以在第一个垂直切片证明需求之前一次性建立全部抽象。

## 5. Encounter 优先的交付边界

当前实现优先级保持为：

1. 纯伤害/结果服务和夹具。
2. Encounter 回合状态和一个可玩的占位遭遇。
3. 该遭遇需要的统治者行为。
4. 该遭遇使用的最小状态子集。
5. Encounter 行动条路径稳定且 ACT 任务得到明确批准后，再实现 ACT。

按 ADR-0004，TASK-013 在可玩实现前提供双模式原生 GAS 基础设施，Encounter 仍是先交付的可玩战斗路线。ACT 准备本身不授权 ACT 玩法／资源；Encounter 任务不吸收 ACT 配表、序列或运行时。后续实现归 TASK-030／031。

## 6. ACT 静态数据模型

### 6.1 角色目录与表格

每个可操作 ACT 角色可以拥有一个 `CharacterACTCatalog` Primary Data Asset，引用该角色专属的一组表：

- Ability Table。
- Projectile Table。
- Summon Table。
- Scope Table；必须先确认 `Scope` 的具体含义。
- Ammo Table。

ACT 内角色专属表实例共用 ACT 强类型 C++ RowStruct；Encounter 使用自己的结构。DataTable 不形成继承层级；ACT 内复用来自 RowStruct、稳定 ID、定义和显式覆写。

Ability Table 是技能名册和装配索引，不是一张巨型技能文档。概念行可以包含：

- 稳定 `AbilityId`。
- 指向 `AbilityDefinition` 的软引用。
- 输入或 Gameplay Event 绑定 ID。
- 授予策略和初始等级。
- 启用状态。
- 角色所需和阻断标签。
- 范围严格的角色专属覆写。

Projectile、Summon、Scope 和 Ammo 表都用稳定 ID 建索引。技能引用这些逻辑 ID，而不依赖某张物理表的路径；解析器使用当前角色目录找到对应行。只有内容有意绑定到某张特定表时，才允许直接使用强类型 Row Handle。

`Scope` 目前还不是可实现的数据模式。创建 RowStruct 前，Owner 必须决定它表示区域/法术场实体、目标选择配置、武器瞄具，还是应拆分成多个概念。

### 6.2 AbilityDefinition

`AbilityDefinition` 是单个技能不可变编辑数据的聚合来源。未来的强类型模式可以包含：

- 稳定 `AbilityId` 和定义版本。
- 原生 Gameplay Ability 类，或可选的 Data-only Ability Blueprint 类。
- 发动场景、所需标签、阻断标签和授予标签。
- 消耗与冷却规格。
- 伤害/效果规格和强类型计算器 ID。
- 指向 `ACTSequence` 的软引用。
- 目标、命中和 Motion Warping 配置引用。
- 适用时的 Projectile、Summon、Field、Scope 或 Ammo 逻辑 ID。
- Gameplay Cue 标签和表现配置引用。
- 从已知强类型策略中选择的加载与执行策略。

定义可以提供 Gameplay Effect 消费的数值或引用，但每个玩法数值必须只有一个声明清楚的真值来源。相同冷却时长、消耗或伤害值不能分别在 INI、表格行、Data Asset 和蓝图图表中独立编辑。

### 6.3 ACTSequence

`ACTSequence` 描述执行拓扑，而不保存可变执行状态。未来模式预期分开：

- 节点：稳定节点 ID/状态标签、动画蒙太奇软引用、蒙太奇 Section、阶段列表、Hit Profile 和 Motion Warping Profile。
- 边：来源节点、目标节点、输入标签、所需/阻断标签、优先级和缓冲策略。
- 阶段动作：打开窗口、请求 Target Data、施加/移除 Gameplay Effect、发出 Gameplay Event、生成已批准载荷或发送 Gameplay Cue 等强类型动作。

Gameplay Tag 用于标识状态、事件、权限和门控。Tag 不能替代显式图边或运行时节点状态。Montage Section 用于表现并同步序列，但不能成为唯一的玩法状态机。

### 6.4 静态数据与运行时所有权

静态定义可以包含基础消耗、冷却策略、动画引用、图拓扑、标签、载荷 ID 和表现引用。

运行时状态包括当前冷却、当前弹药、当前连招节点、缓冲输入、已打开窗口、当前目标、活动 Effect Handle、已生成载荷实例、Prediction Key 和打断状态。运行时状态属于技能执行上下文和可复制玩法状态，绝不能写进 Data Asset、DataTable 或 INI。

## 7. 自定义 INI 与 Developer Settings 边界

未来可以通过强类型 `UDeveloperSettings`，在 `DefaultGame.ini` 或专用配置类别中暴露项目级 ACT 默认值。它适合基础设施策略和校验行为，例如：

- 默认原生 Ability 父类或允许的类族。
- 默认 Input Profile ID 和 Enhanced Input 集成策略。
- 默认冷却 Gameplay Effect 类或冷却 Tag 根。
- 允许的技能阶段 Tag 根。
- 某种技能原型是否必须具有 Sequence、Montage、Cooldown、目标策略或输入绑定。
- 校验器严重级别、调试日志、异步加载策略和开发功能开关。

INI 不得拥有：

- 某个具体技能的 Montage 或 Montage Section。
- 某个具体技能的伤害、消耗或冷却时长。
- 连招节点、连招边、输入窗口或取消优先级。
- Hit Profile、Motion Warping Profile、Projectile、Summon、Field、VFX 或 SFX。
- 可变运行时状态。

Enhanced Input Action 和 Mapping Context 仍是 Unreal 资产。角色目录/表格数据负责把逻辑技能输入绑定给某个角色。INI 可以选择默认输入配置或校验规则，但不能变成第二套逐技能绑定数据库。

配置优先级是先提供策略默认值，再由显式强类型内容确定具体技能。默认值只能补齐可选的基础设施，不得静默覆盖已编辑的玩法平衡；缺少必需内容时，校验器应报告问题，而不是自行生成玩法值。

本文档本身不授权创建 ACT Developer Settings 类或 INI 区段。

## 8. 技能生命周期与阶段模型

技术模型必须支持强类型阶段序列，但全局阶段分类仍未最终确定。合理的候选词汇是：

- `Startup`：前摇和 Commit 前准备。
- `Commit`：提交消耗/冷却以及不可逆发动边界。
- `Active`：命中、载荷或主要效果窗口。
- `Recovery`：动作后锁定、取消和派生规则。
- `End`：确定性清理和结果事件。

`PreMissile`、`Missile` 等名称只适用于投射物，应成为可选强类型动作或子阶段，而不能强迫所有近战、移动、防御和被动技能使用。

每个阶段可以通过显式阶段动作施加或移除 Gameplay Effect 与标签。必须跟踪 Effect Handle 和清理所有权，使取消、打断、死亡和技能结束产生相同且确定的清理结果。

最终阶段名称、Commit 点、取消矩阵、打断优先级以及与 Encounter/Card 时序的关系，都必须在共享枚举、标签或序列化模式冻结前获得批准。

## 9. 连招与预输入缓冲边界

默认目标是在一次已激活的连招 Ability 内保存运行时节点状态。普通连招的每一段不应分别激活新 Gameplay Ability，除非明确需求证明必须拆分。

运行时执行上下文至少应跟踪：

- 当前和上一个节点 ID。
- 当前阶段。
- 已打开的输入/取消窗口。
- 缓冲输入和过期策略。
- 选中的边及转换原因。
- 当前 Target Data、Effect Handle 和预测上下文。

连招边是显式数据。所需/阻断 Gameplay Tag 用于门控边，Input Tag 用于选择候选；边优先级和缓冲策略负责在合法候选中决策。不能只靠 Tag 编码整张图。

预输入缓冲必须作为状态机重新设计并测试。外部 Combat 原型至少存在一条损坏的 PreInput 路径，因此其蓝图只能提供测试案例证据，不能直接移植。

实现连招前，Owner 必须确认缓冲寿命、替换策略、多输入优先级、过早/过晚输入、取消优先级、打断行为和服务器/权威校验。

## 10. 运行时职责流程

未来目标流程是：

    设备输入
      -> PadmaUI 或玩家输入适配器
      -> 强类型输入命令或 Gameplay Event
      -> 发动策略与 Ability System Component
      -> C++ Gameplay Ability 父类
      -> C++ Ability Task 序列解释器
      -> 目标/命中校验
      -> 权威 Gameplay Effect 与运行时状态
      -> 强类型玩法事件
      -> Gameplay Cue 与表现消费者

输入代码只请求动作，不施加伤害；动画代码只报告已编辑的时机标记，不决定权威命中；表现层只消费结果和 Cue，不定义合法性。

如果未来批准联网 ACT，客户端可以预测可逆的输入响应、Montage 播放和表现。Target 接受、伤害、消耗、冷却、库存/弹药和权威 Actor 生成必须由权威端校验。项目实际网络基线仍是开放项；在决定之前，不能把客户端预测写成验收要求。

## 11. 动画、命中、Motion Warping 与表现

- Montage 引用和 Section 映射属于 `AbilityDefinition` 或 `ACTSequence`，不属于 INI，也不能在每个子蓝图图表中重复。
- AnimNotify/AnimNotifyState 可以发出职责单一的时机标记；不能在同一个蓝图中同时负责动画时机、命中权威、伤害、AI 通知和 VFX/SFX 编排。
- 命中检测与 Target Data 构建属于原生组件或 Ability Task，并具有明确的权威/预测规则。
- 伤害和状态变化使用规则服务或 Gameplay Effect，由权威端负责。
- Motion Warping 使用强类型 Profile 加运行时目标数据；没有合法 Warp 目标时必须有确定的回退行为。
- Gameplay Cue 或表现事件触发 VFX、SFX、镜头和其他纯表现；表现失败不能改变玩法结果。
- AI 观察强类型玩法事件或状态变化，不能依赖某个特定动画 Notify 蓝图。

## 12. GAS、插件、蓝图与工具

ADR-0004 选择让 Encounter 与 ACT 都使用原生 GAS，模式域独立。TASK-013 拥有经评审的依赖／生命周期基础，TASK-017 拥有 Encounter，TASK-030 拥有 ACT 执行。文档本身不启用依赖；Encounter 实现不得扩大为 ACT 玩法。

外部 GAS Companion 或 Aurora 插件既非必需，也未获批准。加入任一插件都需要独立 ADR，说明所有权、源码可用性、版本兼容、打包、许可、升级风险和移除策略。

蓝图仍适合内容装配、Data-only 子类、动画图和表现。公共生命周期与权威玩法逻辑属于 C++。

只读审计蓝图不需要 MCP Server。Unreal Python、Asset Registry 和 `BlueprintEditorLibrary` 可以清点资产并读取受支持的编辑器元数据。MCP 未来可以改善编辑器自动化，但它属于开发工具决策，不是运行时依赖，也不自动授权修改资产。

## 13. 外部 Combat 原型带来的教训

该原型只提供参考证据；其中的资产、插件、数值和规则都不是 Padma 默认值。

- 如果连招父类只移除 Tag 或切换武器 Mesh，而 Commit、Montage、分支、等待事件和结束逻辑仍在每个子类重复，那么父类过薄。Padma 的父类/Task 拆分必须集中不变量生命周期，只暴露强类型扩展点。
- 一个 AnimNotifyState 同时拥有时机、命中检测、伤害、AI 通知和表现，意味着权限过多。Padma 按上述边界拆分职责。
- 普通连招路径已有配置，不代表预输入缓冲正确。每条转换和取消路径都需要确定性测试。
- 原型中的处决阈值、距离、按键、锁定假设和特殊反击触发仅是示例。没有 Padma 规则决策，不得复制到 Padma 数据。

## 14. 模块与所有权边界

| Owner | ACT 职责 |
|---|---|
| PadmaCore | 跨模式共享的稳定卡牌/技能身份和与表现无关的卡牌契约 |
| PadmaGameplay | 技能规则、序列执行、目标/命中/伤害结算、战斗运行时状态和未来 GAS 集成 |
| PadmaGame | 模式组合、Possession，以及 Sandbox、Encounter、RealTimeAction 之间的切换 |
| PadmaUI | 设备绑定、输入适配、HUD/仓库表现、提示和纯表现反馈 |
| PadmaEditor | 资产校验器、迁移/导入辅助和编辑诊断 |
| PadmaTests | 纯规则、数据校验、自动化、回放/确定性和未来 PIE 覆盖 |

在物理 UE 模块建立前，这些只是 `Source/DreamOfPadma/` 内部的依赖和所有权规则；它们不授权创建跨模块 Include 或提前建立模块目录。

## 15. 仍然开放的决策

未来 Agent 不得根据提议的类名自行推断以下内容：

- 第一个 ACT 垂直切片及其具体玩家技能。
- Ability System Component 所有权、Avatar/Owner 生命周期和授予/移除策略。
- 纯单机或联网基线，以及预测验收标准。
- 最终 Gameplay Tag 分类和注册所有权。
- AttributeSet、伤害管线、Gameplay Effect 数值所有权和叠层规则。
- 最终技能阶段分类、取消矩阵、打断策略和连招缓冲语义。
- `CharacterACTCatalog` 与各 RowStruct 的精确模式、覆写策略和定义版本。
- `Scope` 的含义以及它是一类还是多个数据域。
- 现有设计仍标记开放的 Encounter/ACT 卡牌到技能发动规则。
- Projectile、Summon、Field、Ammo、锁定、处决和 Motion Warping 生命周期。
- 资产加载、Cook、复制、回放和存档兼容策略。
- 策划基线中已标记开放的 ACT `Tab` 持续/切换行为、消耗/冷却、页面边界和总页数。

其中任何选择若改变公共身份、依赖、存档/网络模式、导入行为或影响多个模块，必须在实现前新增或更新 ADR。

## 16. 建议的未来任务拆分

以下只是工作包，不是有效 TASK ID，也不授权实现：

1. 确认第一个 ACT 垂直切片及依赖/网络决策。
2. 若获批准，加入原生 GAS 基础和一个最小 C++ 技能生命周期。
3. 加入强类型 ACT 数据契约、Asset Manager 加载和单角色目录校验。
4. 实现一条带取消与确定性清理的序列解释路径。
5. 加入一条命中/伤害路径和一条纯表现 Cue 路径。
6. 加入一个双节点连招，并测试预输入缓冲。
7. 只有垂直切片需要时，才加入 Motion Warping 或某类载荷。
8. 集成已接受的 ACT `Tab` 仓库/子弹时间契约，并完成一个战斗闭环。

每个任务只产生一个可观察结果，使用最小写入范围；除非资产所有权和 Editor 会话串行规则明确，否则不修改共享二进制资产。

## 17. 验证与证据要求

未来实现不能仅凭编译成功认定完成。相关任务必须选择以下证据：

- 阶段转换、边选择、缓冲、取消和清理的 C++ 单元/自动化测试。
- 重复 ID、软引用失效、非法 Tag、必需策略缺失和跨表引用非法的数据校验测试。
- 证明表现或输入代码不能生成伤害/消耗/冷却的权威测试。
- 包含 AbilityId、节点、阶段、输入/事件、所选边、目标结果、施加 Effect 和清理原因的确定性追踪。
- Montage 同步、打断、Motion Warping 回退、Cue 和已接受卡牌仓库流程的 PIE 证据。
- 软引用定义和表现资产的打包/Cook 校验。

手工 PIE 观察必须由用户或获授权的 Editor 操作者执行并记录。Agent 不得声称没有实际观察的手工结果。中英双语文档、`Scripts/AuditDocs.ps1`、适用的自动化测试和 `Scripts/ValidateProject.ps1 -Strict` 仍按仓库规范执行。

## 18. ACT Agent 停止条件

遇到以下情况必须停止并请求 Owner 决策：

- 没有已批准且为 `Ready` 的任务授权该 ACT 切片。
- Encounter 前置条件或任务所需契约尚未稳定。
- 实现将替一个开放或延期项做决定。
- 依赖、插件、模块拆分、存档/网络契约或共享模式变更缺少评审。
- 方案把权威移入 UI、动画、Gameplay Cue 或 Data-only Blueprint。
- 同一玩法值将出现两个真值来源。
- 另一个 Agent 拥有重叠的可写文件或二进制资产。

ACT Agent 绝不能静默照搬外部 Combat 原型、修改引擎/插件源码、直接改变其他模块状态，或把 Encounter 任务扩大为推测性的 ACT 实现。

## 19. 独立卡片、装备阵容与模式契约

较新的分离边界由 [ADR-0004](../../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md) 规定。TASK-035 拥有独立 ACT 角色卡集合与战斗总设置角色／武器阵容；TASK-027 将地形／上下文和通过校验的阵容交给 TASK-030。基础技能卡与 Encounter 共享身份／槽位，但绑定单独配置的 ACT 效果。

ACT 阶段、时钟、AttributeSet、原生技能父类和效果实例不属于 Encounter 契约。地形可限制某特性；校验须区分特性、技能、武器和整角色限制，并覆盖非 UI 直接命令。容量、操控／切换、限制时点、触发单位关联及本局／预设归属仍属 D20。只有获准本局拥有且战斗可修改状态参与完整事务／保存边界。全部未接受数值须用户逐项确认。

## 静态配置交接：TASK-036

消费公共 ACT/Authoring 角色／武器／技能资产及 FPadmaACTSkillRow，不在 TASK-030 ACT/Data 或 TASK-035 RosterData 中重复定义。运行配置保持独立：通过 ACT 专用且受支持的 GAS 注册表解析 AbilityImplementationId，验证选定装备，并在权威命令中执行已批准特性限制。配置校验成功不等于可战斗或取得阵容所有权。软引用在集成时需要加载／烘焙策略。空模板不提供玩法数值或序列。
