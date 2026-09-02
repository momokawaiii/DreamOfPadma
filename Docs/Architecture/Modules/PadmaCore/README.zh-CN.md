# PadmaCore 模块程序文档

- 文档 ID：ARCH-MODULE-CORE-001
- 版本：0.1
- 状态：计划中的逻辑边界；尚未建立独立 UE 模块
- 英文原文（Agent 阅读）：README.md
- Owner：Core 模块 Agent
- 当前实现：Source/DreamOfPadma/ 是临时共享运行时模块
- 上级架构：../../ProgramArchitecture.zh-CN.md、../../DataDrivenArchitecture.zh-CN.md

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
