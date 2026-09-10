# 模块地图

- 英文原文：[ModuleMap.md](ModuleMap.md)
- 状态：当前目录映射；离线 Demo 新增项为目标

一个 UE 运行时模块 `DreamOfPadma` 包含下列逻辑边界。依赖和结算约定由 [ProgramArchitecture](ProgramArchitecture.zh-CN.md) 拥有。

| 领域 / 归属 | Source/DreamOfPadma/{Public,Private} 下的当前路径 | 当前职责 | 尚未交付的 Demo 新增项 |
|---|---|---|---|
| [Core](Modules/PadmaCore/README.zh-CN.md) | Core/Content、Core/Run、Core/WorldMap、Core/Cards | 值定义、规则、冻结图/检查点、随机流与校验 | 章节/剧情/教程值契约与阶段分配持久化 |
| [Game](Modules/PadmaGame/README.zh-CN.md) | Game/Content、Game/Run、Game/Save、Game/Framework、Game/WorldMap、Game/Presentation | 目录/会话/存档/切图组装及地图编辑桥接 | 章节协调、烘焙 MapKey 查找与版本校验 |
| [Gameplay](Modules/PadmaGameplay/README.zh-CN.md) | Gameplay/Encounter、Gameplay/ACT | 独立原生 GAS 模式、效果与战斗适配 | 生产内容及按序教程战斗 |
| [World](Modules/PadmaWorld/README.zh-CN.md) | World/Map、World/PCG | 连续地形、缓存道路/代理与装饰 PCG | 绑定权威地图元数据的冻结烘焙表现 |
| [UI](Modules/PadmaUI/README.zh-CN.md) / Presentation | UI、Presentation | CommonUI/Slate HUD、分层检视与反馈 | 菜单/开场/教程/结尾流程与最终视觉 |
| [Editor](Modules/PadmaEditor/README.zh-CN.md) | 现有编辑器条件编译适配；Source 外的 Scripts/Editor | Python 制作、模型/地图预览和校验器 | 烘焙/校验工具；剧情图编辑器暂缓 |
| [Tests](Modules/PadmaTests/README.zh-CN.md) | Private/Tests | 规则/目录/地图/UI 回归 | 章节状态、迁移和打包黄金路径覆盖 |

World Actor 与装饰 PCG 消费状态；当前可变世界结算位于 Core/Run，由 Game 协调。默认绘景教程使用独立 Slate 表现，不证明 3D 地图烘焙路径已完成。见 [WorldMapAuthoring](../Content/WorldMapAuthoring.zh-CN.md)。

Editor 和 Tests 是逻辑职责，不意味着七个独立 UE 模块。模块拆分需要明确的依赖/构建/测试理由；运行时不能依赖未来 Slate/UEdGraph 编辑器。类型细节见 [RuntimeNaming](RuntimeNaming.zh-CN.md)，当前证据见 [ProjectState](../ProjectState.zh-CN.md)。
