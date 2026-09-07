# PadmaGame 模块程序文档

- 文档 ID：ARCH-MODULE-GAME-001
- 版本：0.2
- 状态：计划中的逻辑边界；TASK-007 会话交接已在临时共享模块中实现；尚未建立独立 UE 模块
- 英文原文（Agent 阅读）：README.md
- Owner：Game 组合模块 Agent
- 当前实现：Source/DreamOfPadma/ 是临时共享运行时模块；TASK-007 只增加 Demo 会话边界，不修改默认地图配置
- 上级架构：../../ProgramArchitecture.zh-CN.md、../../DataDrivenArchitecture.zh-CN.md

## 1. 目的

PadmaGame 是运行时组合根。它把 Core 契约连接到 Gameplay、World、UI、持久化和控制模式表现，但不拥有这些模块的具体规则。

在 TASK-007 中，组合边界是 GameInstance 生命周期内的 `UDemoTransitionSessionSubsystem`。它在未来世界切换期间保存经过校验的 `FDemoTransitionContext`，并向下一个场景提供发布/查看/消费操作。

## 2. 负责事项

- GameMode、GameState、PlayerController、GameInstance 和会话生命周期集成。
- 逻辑服务的组合和初始化。
- 把命令路由给正确的应用服务或规则服务。
- 协调日历推进和安全阶段边界。
- 协调存档/读档、战前自动存档以及战斗提交/回滚事务。
- 加载当前场景或 MVP 规则配置。
- 协调通用结算器。
- 路由战略沙盘、条件驱动的回合制遭遇战和 RealTimeAction 模式请求，包括 MVP 完整 ACT 路线以及未来 FPS/其他变体。
- 协调敌我双方、地形、卡牌和剧情效果提供的交战路线限制，但不拥有这些细节规则；最终路线可以选择/触发 Encounter、ACT 或未来的其他 RealTimeAction 表现模式和剧情后果。
- 通过只读视图或事件向 UI 暴露运行时状态。
- 拥有固定 Demo 切场上下文的会话生命周期，但不序列化地图 Actor、关卡坐标或 Widget 状态。

## 3. 不负责事项

PadmaGame 不负责：

- 卡牌公式、战斗伤害公式或 AI 评分细节。
- Widget 内部或输入设备的直接解析。
- 节点布局或世界变化规则。
- 一个包含所有系统的万能可变 Manager。
- Demo 地块的选择/高亮表现或 World fixture 的图规则。

它可以协调服务，但服务必须拥有自己的状态和契约。

## 4. 对外契约

- 开始、继续和结束本局命令。
- 当前配置和定义加载结果。
- 阶段切换和安全边界通知。
- 存档请求、存档结果、读档结果和迁移失败。
- OutcomeResolved 与本局锁定通知。
- 控制模式和交战路线切换请求与结果。
- 完整版本化战前快照、战斗提交和精确战斗回滚通知。
- 供表现层消费的只读状态快照。
- 通过 `UDemoTransitionSessionSubsystem::PublishTransition`、`PeekTransition` 和 `ConsumeTransition` 交接 `FDemoTransitionContext`。

Game 层必须传递有类型的命令和结果，不能暴露子系统内部。

## 5. 数据与运行时所有权

PadmaGame 拥有本局身份、生命周期、存档编排和最终结局锁定等会话级状态。它消费 Core、World 和 Gameplay 的状态视图，不复制它们的可变值。

结算表和场景配置是数据输入。结算器根据权威状态执行条件检查并记录 OutcomeId。

TASK-007 的 `FDemoTransitionContextStore` 会在替换 pending 状态前校验节点、场景和出生点 ID。无效替换会报告可读失败并保留此前上下文。

## 6. 依赖与集成

作为组合根，PadmaGame 可以依赖 PadmaCore、PadmaGameplay、PadmaWorld 和 PadmaUI 的公开契约；这些模块不能依赖 PadmaGame 内部。

未来联机时，本层用于区分本地会话协调与服务器拥有的权威状态。

## 7. 测试与调试证据

必须测试：

- 项目启动和子系统初始化。
- 新局和继续游戏生命周期。
- 日/时代/Chapter 边界路由。
- 存档/读档往返、完整战前自动存档、失败/退出精确恢复和胜利提交。
- 结算优先级和本局锁定。
- 沙盘到战斗的路线（包括 ABC 卡角色移动到敌方占据地点）、敌我共同的交战路线限制、模式/剧情触发结算，以及不改变规则状态的控制模式请求路由。
- 遭遇战行动条生命周期和完整 ACT RealTimeAction 战斗闭环路由。
- PIE 冒烟测试和正常关闭。
- TASK-007 上下文自动化测试和 DemoSandbox 地图加载 smoke。

调试输出应显示服务初始化顺序、当前配置 ID、阶段转换、存档边界、结算检查和命令路由失败。

## 8. 实现阶段

1. 先保留在生成的 DreamOfPadma 模块中完成组合，并提供 TASK-007 会话生命周期内的 Demo 交接。
2. 加入明确的服务所有权和状态视图。
3. 增加 ABC 卡角色到达敌方占据地点时的沙盘到遭遇战路由、行动条编排和完整战斗事务边界。
4. Core 与 World 契约建立后，再加入存档和结算编排。
5. 遭遇战路径稳定后加入完整 ACT RealTimeAction 路线编排。
6. 只有构建依赖足以证明需要拆分时，才建立 PadmaGame 模块。

## 9. 学习目标与风险

学习目标：Unreal Gameplay Framework、子系统生命周期、组合根、强类型会话状态、存档编排、模式路由和集成测试。TASK-007 引入了会话生命周期交接；用户讲解回授仍待完成。

主要风险：把 GameMode 或 GameInstance 变成万能规则容器，而不是协调者。
