# PadmaCore 模块程序文档

- 文档 ID：ARCH-MODULE-CORE-001
- 版本：0.3
- 状态：Core 目录边界已实现；无独立 UE 模块
- 英文原文（Agent 阅读）：README.md
- Owner：Core 模块 Agent
- 当前实现：Source/DreamOfPadma/{Public,Private}/Core；准确写权限由 TASK 指定
- 上级架构：../../ProgramArchitecture.zh-CN.md、../../DataDrivenArchitecture.zh-CN.md

## TASK-046 当前原生实现

Core/Content 定义校验后的表值快照；Core/Run 拥有已接受 HTML 的日历、资源、ABC 流转、种子合成、图命令、战斗回滚／提交及存档验证，不含场景／UI／GAS 对象。原生切片由 8 项规则测试和 2 项目录测试覆盖。 [原生可玩指南](../../../Content/NativePlayableDemo.zh-CN.md).

## 1. 目的

PadmaCore 包含面向规则的值、稳定身份、数据契约、确定性计算、命令、结果和事件。它必须独立于表现层和具体世界 Actor。

它计划承载日历、资源、卡牌契约、合成计算、确定性随机流和存档模式值类型。

## 2. 负责事项

- 卡牌、节点、遭遇、剧情事件、结局和存档记录的稳定 ID。
- 日历状态和阶段切换契约。
- 心流、算力、熵、信仰、威胁、战争天平、核心和耐久值类型。
- 卡牌定义、卡牌运行时状态和卡牌发动场景契约。
- 合成输入、候选、预览、成功/失败和非奖励西格玛/失败后果结果类型。
- 战斗模式、遭遇战行动条、先攻/优先级/窗口、ACT RealTimeAction 输入、卡牌仓库、分页和卡槽值类型。ACT MVP 契约是 `Tab` 打开仓库、背景虚化、世界时间为 1/10、禁止移动/攻击输入和每页五个可见卡槽；总页数延期。
- 玩家、GM、自动化测试和未来网络命令契约。
- 领域结果和明确的状态变化事件。
- 确定性随机流接口和计算追踪。
- 不包含 Actor 指针的存档模式值类型。

## 3. 不负责事项

PadmaCore 不负责：

- UMG Widget、Niagara、镜头、动画或音频播放。
- 具体地图或战斗 Actor。
- 关卡布局、World Partition、PCG、天气、水面或植被表现。
- 输入设备细节。
- 详细 AI 表现或 UI 状态。
- 把资产路径当作玩法身份。

Core 边界可以使用分阶段项目所需的最低 Unreal 支持，但不能获得表现层或世界层依赖。

## 4. 对外契约

计划中的契约包括：

- CalendarState 和阶段转换事件。
- ResourceState 和资源变化结果。
- CardDefinition、CardRuntimeState 和牌堆操作。
- BattleMode、BattleActionTimelineState、CardActivationContext、CardRepositoryPage 和打牌请求/结果契约。
- SynthesisInput、SynthesisPreview、SynthesisResult 和计算追踪。
- 命令、验证错误、结果和事件封装。
- WorldState 和存档模式值对象。
- OutcomeConditionDefinition 和 RunOutcomeState。

名称在真正创建 C++ 类型前都是暂定的，语义必须遵循英文策划案/规则文档。

## 5. 数据与运行时所有权

PadmaCore 消费类型化的卡牌、日历、资源、合成和结局定义。它拥有计算和契约，不拥有可变 UObject 资产。可变本局状态由运行时组合层负责，并通过明确服务传入。

定义和状态必须可分离，从而使新增卡牌数值或合成曲线无需修改代码。

## 6. 依赖与集成

逻辑上不依赖 PadmaGameplay、PadmaWorld、PadmaUI 或 PadmaEditor。

集成使用：

- 来自 UI、GM 工具、测试和未来网络调用者的命令。
- 返回给调用者的结果。
- 由 Gameplay、World、UI、Save 和遥测消费的事件。
- 使用稳定 ID 和有类型的查询，而不是具体 Actor 引用。

当前 DreamOfPadma 的 Build.cs 只是过渡实现。未来建立独立 PadmaCore 模块必须有 ADR 和独立构建/测试理由。

## 7. 测试与调试证据

必须测试：

- 日历边界切换。
- 资源记账和明确设计、有代价/风险的转换循环。
- 稳定 ID 和序列化夹具。
- 候选筛选、Softmax、成功/失败和无奖励的西格玛/失败后果结果。
- 基础非 A 卡在战斗外、玩家不符合条件的遭遇战行动回合以及超过每回合一张限制后被拒绝。
- ABC 卡主动技能可以从沙盘发动而不进入战斗。
- ACT RealTimeAction `Tab` 仓库切换、背景虚化、1/10 世界时间减缓、禁止移动/攻击输入、子弹时间只接受卡牌输入、场景惯性、每页五个可见卡槽分页和卡槽请求校验。不得假设总页数。
- 相同种子产生相同结果。
- 预览不修改状态，也不推进真实随机流。
- 非法命令和缺失数据错误可读。

调试输出应包含输入 ID、数据版本、种子/随机流位置、计算字段、状态变化和发出的事件。

## 8. 实现阶段

1. 在 Source/DreamOfPadma/ 内提取值类型和契约。
2. 在加入 Actor 前增加纯计算测试。
3. 通过约定的数据边界加入数据定义加载。
4. 边界被证明后，再拆成独立 UE 模块。

## 9. 学习目标与风险

学习目标：C++ 值类型设计、接口、确定性模拟、序列化、数据契约、单元测试和依赖控制。

主要风险：为了方便直接访问 Engine、Actor 或全局状态，最终把 Core 变成无法测试的玩法 Manager。

## 10. 完整 MVP 契约分配

TASK-012 拥有模式中立 ID、状态参与者及强类型命令／结果封装；TASK-015 只拥有规则含义明确相同的计算。TASK-020 拥有回合制／ABC 卡系统，以及共享基础技能的身份、槽位和生命周期 provider。TASK-035 拥有独立 ACT 角色卡集合与阵容值。定义 ID、持有卡实例 ID、战斗实体 ID 是不同身份域。

Core 不包含 ASC，也不强迫 Encounter 与 ACT 共用执行配表。逻辑回合／窗口时钟与 ACT 实时时钟是各模式服务的明确输入。共享基础技能槽位选择当前模式效果定义，不合并角色集合、冷却或存活技能。见 [ADR-0004](../../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md)。

状态 Owner 为战斗可修改的本局状态实现导出／恢复参与接口。D20 在选择存档字段前区分 ACT 预设和本局阵容／装备。来源 PDF 中缺失或含义不清的字段记为未决，不能变成数值为零的运行默认值。

## 中立关系贡献边界

TASK-012 在资源／卡牌／合成／战斗消费者实现前冻结只读效果来源／修正值契约。TASK-039 提供关系来源状态；019／020／022／015 仅消费 D28 选定、归各自所有的目标领域，每个标量／效果有唯一应用所有者。TASK-034 注入真实服务。契约和 Core 独立于 GAS、Actor、地图和 UI，未知／重复／版本无效贡献明确失败。关系状态、数值执行和增益总览各负其责。

## TASK-040 交接

TASK-040 在 Core/Cards 实现 FPadmaCardMobilityDefinition，不依赖 Actor／UI／模型。已配置标记避免默认移动授权，条件 ID 仍是数据。TASK-012／020／024 拥有后续规则命令和求值器注册。

## 第零章目标边界

[DataDrivenArchitecture](../../DataDrivenArchitecture.zh-CN.md)、[RuntimeFlow](../../RuntimeFlow.zh-CN.md)、[SaveSchema](../../SaveSchema.zh-CN.md) 拥有 ADR-0010 下新增章节/剧情/教程契约。Core 拥有纯值条件/效果、阶段分配、检查点历史和教程奖励身份，Game 拥有加载/协调。这些新增并非当前 API。

冻结生成结果与可变局内状态分离，PCG、Sequence、UEdGraph、Widget 对象不得进入 Core。测试覆盖阶段/读档确定性、同次奖励幂等与新次重玩区别、非法恢复及完整战斗回滚。准确注册表编码和持久事务实现仍待定。本文既有完整 MVP 分工是后续工作，不是离线 Demo 检查表。
