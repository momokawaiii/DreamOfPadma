# PadmaGame 模块程序文档

- 文档 ID：ARCH-MODULE-GAME-001
- 版本：0.5
- 状态：共享运行时模块中的当前逻辑组合边界；旧 Demo 链路已退役；无独立 UE 模块
- 英文原文（Agent 阅读）：README.md
- Owner：Game 组合模块 Agent
- 当前实现：Source/DreamOfPadma/{Public,Private}/Game；Core 拥有规则，Game 组合当前地图与分离战斗模式
- 上级架构：../../ProgramArchitecture.zh-CN.md、../../DataDrivenArchitecture.zh-CN.md

## TASK-046/048 当前原生实现

Game/Content 解析软引用分表；Game/Run 在 GameInstance 中保存值状态，并写入 Game/Save/PadmaRunSaveGame.h 声明的安全存档外壳。Game/Framework 将 PadmaGameMode.h/.cpp 与 PadmaPlayerController.h 及其组合／UI／战斗实现文件分开；控制器组合地图／镜头／CommonUI／战斗，把 UI 意图转为命令，切入战斗地图并提交一次结果，处理缺图／启动／切场失败及回调清理。TASK-048 已退役 TASK-007/008 的 Demo 地图、定义和切场链路。Game/WorldMap 拥有地图资产、检查点表导入与编辑手柄；Core/WorldMap 拥有值生成／校验。Game/Run 冻结所选地图用于存档与回滚。见[地图编辑](../../../Content/WorldMapAuthoring.zh-CN.md)。 [原生可玩指南](../../../Content/NativePlayableDemo.zh-CN.md).

## 1. 目的

PadmaGame 是运行时组合根。它把 Core 契约连接到 Gameplay、World、UI、持久化和控制模式表现，但不拥有这些模块的具体规则。

历史 TASK-007 曾通过 UDemoTransitionSessionSubsystem 交接强类型上下文。该实现已退役；当前 Game/Run 与 Game/Framework 协调原生本局／战斗生命周期，原始证据保留在 TASK 报告。

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
- 跨存档／读取及战斗事务保留本局身份和冻结地图，不序列化场景 Actor 或 Widget。

## 3. 不负责事项

PadmaGame 不负责：

- 卡牌公式、战斗伤害公式或 AI 评分细节。
- Widget 内部或输入设备的直接解析。
- 节点布局或世界变化规则。
- 一个包含所有系统的万能可变 Manager。
- World/Map 的选择／高亮几何或 Core 世界规则变更。

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
- 经当前本局子系统交接 FPadmaMapLayout、本局命令结果及完整战斗快照。

Game 层必须传递有类型的命令和结果，不能暴露子系统内部。

## 5. 数据与运行时所有权

PadmaGame 拥有本局身份、生命周期、存档编排和最终结局锁定等会话级状态。它消费 Core、World 和 Gameplay 的状态视图，不复制它们的可变值。

结算表和场景配置是数据输入。结算器根据权威状态执行条件检查并记录 OutcomeId。

历史 TASK-007 上下文存储校验保留在任务证据中；已退役的存储不是当前 API。当前 Core/Run 在替换前校验地图／本局快照。

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
- 当前 WorldMap 生成、检查点、存档顺序与回滚测试，以及可玩地图启动／返回冒烟。

调试输出应显示服务初始化顺序、当前配置 ID、阶段转换、存档边界、结算检查和命令路由失败。

## 8. 实现阶段

1. 当前 Core/Game/Gameplay/World 组合继续位于共享 DreamOfPadma 模块；TASK-007 交接已退役。
2. 加入明确的服务所有权和状态视图。
3. 增加 ABC 卡角色到达敌方占据地点时的沙盘到遭遇战路由、行动条编排和完整战斗事务边界。
4. Core 与 World 契约建立后，再加入存档和结算编排。
5. 遭遇战路径稳定后加入完整 ACT RealTimeAction 路线编排。
6. 只有构建依赖足以证明需要拆分时，才建立 PadmaGame 模块。

## 9. 学习目标与风险

学习目标：Unreal Gameplay Framework、子系统生命周期、组合根、强类型会话状态、存档编排、模式路由和集成测试。TASK-007 仅作为历史生命周期证据，不是当前入口。

主要风险：把 GameMode 或 GameInstance 变成万能规则容器，而不是协调者。

## 10. 分离模式的装配与阵容交接

TASK-027 根据世界／上下文约束解析战斗路线并调用相应模式适配器。Encounter 接收回合制部署；ACT 接收 TASK-035 在战斗总设置中独立配置的阵容及入战地形／上下文。与触发地图单位是否有额外关联仍属 D20，不隐式转换角色卡。

TASK-028／029 通过 TASK-012 参与者协调完整战斗事务与安全边界持久化。TASK-035 注册所有获准本局拥有且战斗可修改的阵容／武器／库存字段；全局预设另行确定生命周期。Game 从稳定定义与状态重建模式运行对象，不保存 GAS 句柄。

TASK-034 向 Encounter 与 ACT 注入 TASK-020 的正式卡牌／付款／生命周期 provider，替换获准测试夹具并验证整局。单个死亡／打断由模式处理；只有获准局部结果才结束战斗事务。不因此增加万能规则 Manager 或独立 UE 模块。


## 历史 TASK-008 固定 Demo 组合——已退役

原 Demo/Integration 切场子系统曾通过凭据与 Actor 就绪检查排队进入固定 Encounter 关卡。TASK-048 已移除该路线及其生产依赖；当前切场、提交与回滚由 Game/Framework、Game/Run 组合。

历史 Demo smoke／上下文／召唤证据保留在 TASK-008；相关测试与调试输入已退役。当前测试入口见[测试方案](../../../Production/CurrentBuildTestPlan.zh-CN.md)。

## TASK-040 交接

TASK-040 在 Game/Presentation 将 ACT／通用模型定义适配成中立已加载表现视图。带领域定义键只选一个来源。APadmaNodePreview 组合静态槽位和预览绑定，不写会话、阵容、移动／费用或存档。TASK-048 已把驻军预览迁移为通用模型；当前运行时入口见 NativePlayableDemo。

## 第零章目标边界

Game 将按 [ADR-0010](../../../Decisions/ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md) 组装 ChapterDefinition、匹配烘焙地图元数据、剧情/教程执行、菜单/开场/结尾及安全恢复。生命周期和持久化见 [RuntimeFlow](../../RuntimeFlow.zh-CN.md)、[SaveSchema](../../SaveSchema.zh-CN.md)，不在此复制结构。

替换当前局前，按独立安装的地图/内容定义验证存档；根据稳定状态重建表现与模式 GAS。教程 UI/Sequence 提交意图，Core/Game 服务结算进度和奖励。重玩产生新的教程身份，继续恢复已有身份。章节功能、MapKey 烘焙查找和奖励持久化仍待完成，当前 Game/Run 未交付。

验收需覆盖两种包的新局/继续/跳过/重玩、地图不匹配、模式切图失败和干净退出。剧情编辑器暂缓。保持当前 Core/Game 权威与无关完整 MVP 分工，不增加万能章节 Manager 或运行时编辑器依赖。
