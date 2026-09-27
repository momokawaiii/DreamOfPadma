# 模块地图

- 状态：当前目录映射；离线 Demo 新增项为目标

玩法代码目前集中在 DreamOfPadma 这个 UE 模块里，下面是职责划分：

- Core：规则、值数据、随机和校验。
- Game：本局流程、存档、切场和内容加载。
- Gameplay：Encounter、ACT 和 GAS。
- World：地形、道路、模型和装饰 PCG。
- UI：页面、输入和反馈；不结算玩法。
- Editor / Tests：制作工具和测试，不代表已有独立模块。

ACT 的动画和 Niagara 依赖留在表现一侧；编辑器图工具只在编辑器构建使用。具体路径见英文。

ACT 目录整理工具只在编辑器构建依赖 AssetTools/AssetRegistry，用于明确授权的引用修复，不进入运行时 Core。见 [ADR-0014](../Decisions/ADR-0014-ACT-Asset-Layout.zh-CN.md)。

详细依据、操作和证据见[英文原文](ModuleMap.md)。

PadmaNPR 是独立插件：Runtime 管 Cloth HLSL 注册、配置与 MID 创建；Editor 管面板和材质生成。Editor 依赖 Runtime，两者都不依赖玩法模块。详见 [ADR-0013](../Decisions/ADR-0013-NPR-Cloth-Core.zh-CN.md)。
