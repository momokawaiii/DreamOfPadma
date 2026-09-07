# PadmaWorld 模块程序文档

- 文档 ID：ARCH-MODULE-WORLD-001
- 版本：0.2
- 状态：计划中的逻辑边界；TASK-007 固定 Demo fixture 已在临时共享模块中实现；尚未建立独立 UE 模块
- 英文原文（Agent 阅读）：README.md
- Owner：World 模块 Agent
- 当前实现：Source/DreamOfPadma/ 是临时共享运行时模块；TASK-007 在 `Content/Padma/Demo/World/` 下拥有项目自有 Demo Sandbox fixture
- 上级架构：../../ProgramArchitecture.zh-CN.md、../../DataDrivenArchitecture.zh-CN.md

## 1. 目的

PadmaWorld 负责共享沙盘图和持久世界变化。它把玩家操作和统治者响应表现为稳定节点、归属、路线、设施、遭遇和环境旗标的变化。

TASK-007 增加了一个明确标记为 playtest-only 的 Demo Sandbox fixture。表现层拥有选择/高亮状态；切场身份通过稳定 ID 强类型表达，并交给 Game 会话边界。该 fixture 不定义正式世界图或世界规则。

## 2. 负责事项

- 独立于坐标的稳定节点和边 ID。
- 手工制作的 MVP 图，以及未来 World Partition/PCG 接口。
- 归处/核心、普通节点、轶闻、熔炉、薪火/安全屋、前哨和统治者核心节点类型。
- 移动合法性、相邻关系、放置、占领、设施操作和路线条件。
- 玩家与统治者的归属状态。
- 世界变化和变化事件。
- 局部战斗路线（遭遇战或 RealTimeAction）的进入/退出边界，包括 ABC 卡角色移动到敌方占据地点的触发；敌我共同限制且可以选择/触发表现模式和剧情的交战路线；以及完整战前快照令牌请求。
- 作为数据输入的地形、天气和环境修正。
- 产生持久世界状态变化的剧情选择。
- TASK-007 固定 Demo Sandbox 地块表现、强类型选择/确认请求和 fixture 校验。

## 3. 不负责事项

PadmaWorld 不负责：

- 伤害公式、状态结算或 AI 效用评分。
- 卡牌合成候选选择或成功率。
- UI 决策或镜头表现。
- Actor 指针的存档序列化。
- 用硬编码关卡名称判断玩法条件。

World 可以提供合法查询并接受命令，但状态变化必须留在世界规则/应用路径中。

## 4. 对外契约

- NodeId、EdgeId、NodeDefinition 和 NodeRuntimeState。
- 移动、放置、附着、占领、设施、轶闻、熔炉、安全屋和路线解锁命令。
- 世界变化结果和事件。
- 局部战斗路线进入上下文、交战路线限制、模式/剧情触发结果、完整快照令牌和返回沙盘时的提交/回滚结果。
- 只读图与归属查询。
- 地形/环境修正输入。
- 固定 Demo 切片使用的 `FDemoNodeId`、`FDemoScenarioId`、`FDemoSpawnPointId`、`FDemoSelectNodeRequest`、`FDemoConfirmNodeSelectionRequest` 和 `FDemoTransitionContext`。
- 用于脱离地图进行 fixture 查找和选择状态管理的 `FDemoWorldTileDefinition` 与 `FDemoWorldSelectionModel`。

坐标和关卡资产属于表现数据。事件、存档和测试使用稳定 ID 作为身份。

TASK-007 会把一次性地块命中转换为 `FDemoNodeId`；Actor 指针和原始坐标不会进入 `FDemoTransitionContext`。上下文通过 `UDemoTransitionSessionSubsystem` 发布、查看和消费，其生命周期属于 GameInstance/会话边界。

## 5. 数据与运行时所有权

World 消费 WorldGraph、NodeDefinition、MutationDefinition、EncounterDefinition、StoryEvent、TerrainProfile、交战路线和环境数据。它拥有节点归属、发现、驻军、设施、路线和变化等可变状态。PadmaGame 负责协调事务；PadmaWorld 提供世界状态快照边界以及提交/回滚结果。

第一张地图是规则测试场。未来的 World Partition、PCG、大地形、水面、天气、植被和最终美术可以替换表现而不改变稳定世界契约。

## 6. 依赖与集成

World 依赖 PadmaCore 的 ID、命令、状态值和事件。它由 PadmaGame 组合，并与 PadmaGameplay 交换遭遇战/ACT 战斗路线请求和结果。

在 TASK-007 中，World 只调用 PadmaGame 暴露的强类型会话契约。会话存储在替换 pending 上下文前校验三个身份，因此无效 fixture 会失败并保持已有上下文不变。

PadmaWorld 不能调用 UI 代码，也不能把 Widget 当作占领或路线状态的权威。

## 7. 测试与调试证据

必须测试：

- 稳定图 ID 和邻接关系。
- 合法/非法移动和放置。
- 节点占领、设施变化和条件路线。
- 轶闻选择改变持久状态。
- 熔炉和安全屋操作。
- ABC 卡角色到达敌方占据地点时的局部战斗路线进入、敌我共同的交战路线限制、模式/剧情触发结算、完整战前快照、胜利提交和失败/退出精确回滚。
- 相同世界事件序列恢复相同状态。
- 修改坐标不会修改节点身份。
- TASK-007 自动化测试：稳定节点/场景/出生点 ID 在上下文创建后保持一致；无效替换和缺失 fixture 字段失败且不修改原有上下文。
- DemoSandbox 地图加载 smoke：项目自有地图启动固定 fixture，并输出用户可读的选择提示。

调试输出应显示命令 ID、节点 ID、归属前后、路线条件、变化来源、遭遇上下文和事件顺序。

## 8. 实现阶段

1. 在临时共享模块中实现 TASK-007 固定 Demo fixture、选择表现和强类型切场交接。
2. 使用占位表现实现数据定义的图。
3. 加入移动、放置、占领和一种世界变化。
4. 加入一个轶闻、熔炉和安全屋流程。
5. 加入统治者响应接口、敌我共同的交战路线限制、Encounter/ACT/未来模式触发接口和完整快照事务边界。
6. 规则图可测试后，再加入环境扩展接口。

## 9. 学习目标与风险

学习目标：稳定世界身份与表现坐标的区别、选择到会话的强类型交接、世界状态建模、图结构关卡设计、Gameplay Framework/World 集成、存档边界、World Partition 规划、PCG 接口和环境系统。TASK-007 只引入前两个目标，不宣称用户已经完成讲解回授。

主要风险：把玩法真相存进关卡 Actor 或坐标，导致地图重建破坏存档和回放兼容性。
