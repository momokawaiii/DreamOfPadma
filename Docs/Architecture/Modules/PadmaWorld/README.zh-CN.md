# PadmaWorld

- 文档 ID：ARCH-MODULE-WORLD-001
- 版本：0.4
- 状态：已实现逻辑表现边界；Demo 烘焙目标待完成
- 英文原文：[README.md](README.md)
- 负责人：World 模块 Agent
- 源码权限：Source/DreamOfPadma/{Public,Private}/World/Map 与 World/PCG；项目包装资产位于 Content/Padma

## 当前职责

World 表现地形、道路、节点、守军、锚点和反馈。3D 路径缓存连续程序表面，对坡度/水域/节点/道路筛选后，以独立 Seed 向 PCG Static Mesh Spawner 提供点。默认绘景教程使用独立 UI 表现。当前样板资产和操作见 [StrategyPresentation](../../../Content/StrategyPresentation.zh-CN.md)。

Core/WorldMap 拥有值图生成/校验，Core/Run 拥有可变世界状态、移动、占领及检查点决定。Game 拥有加载/编辑桥接/切图/存档。世界逻辑领域跨越这些适配器，World 场景 Actor 不是状态权威；这澄清了旧文将全部世界变更归给 World 的表述。

## 契约与接入

- FPadmaMapLayout：Game 选择的冻结值，图边决定通行。
- FPadmaWorldNodeView/FPadmaWorldEdgeView：稳定 ID 与只读视觉状态。
- APadmaWorldMapActor / UPadmaMapVisualTheme：缓存场景投影、地形/主题/模型引用与视觉反馈。
- Actor 命中返回稳定节点 ID，由 Game 向 Core 提交命令。
- PCG 消费装饰输入，不创建玩法边、不移动剧情锚点、不推进玩法随机流。
- Encounter/ACT 地点/上下文及结算保持为 Core/Game/Gameplay 的类型化契约；World 显示结果。

World 依赖 Core 值契约和场景/表现 API，不能使用 UI 作为占领权威、计算伤害/合成、序列化 Actor 身份或按关卡名选择规则。

## 已接受的 Demo 目标

生成/校验/冻结/烘焙流程见 [WorldMapAuthoring](../../../Content/WorldMapAuthoring.zh-CN.md)，优先级见 [ADR-0010](../../../Decisions/ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md)。两种打包配置均加载带匹配 MapKey 元数据的烘焙地形/装饰；当前运行时 PCG 样板尚未交付此管线。

保留章节固定地标、显式路线和锁。未来程序章节可改变获批玩法输入，装饰 PCG 仍无规则权威。本文不要求 ALandscape、World Partition 或指定地形算法。

## 验证、存档与风险

测试几何重建不改变状态、命中/视觉坐标匹配、锚点/道路排除、确定性生成与装饰随机隔离。目标还需烘焙/Cook/MapKey 一致，以及无需编辑器/运行时生成的包加载。

[SaveSchema](../../SaveSchema.zh-CN.md) 拥有冻结地图/状态兼容；视觉根据状态与匹配内容重建。场景指针不进入存档或未来命令传输。构建/PIE/PCG 证据保留在 TASK-048/050/052；最终美术与目标包验收待完成。

按需学习主题：地形表示、种子生成、PCG 筛选、实例化、烘焙/Cook、地图与状态隔离。主要风险是视觉/玩法不一致、绕锁，以及美术重建改变存档身份。
