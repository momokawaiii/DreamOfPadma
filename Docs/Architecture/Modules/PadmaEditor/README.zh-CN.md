# PadmaEditor

- 文档 ID：ARCH-MODULE-EDITOR-001
- 版本：0.2
- 状态：已有制作脚本/适配器；自定义剧情编辑器暂缓；无独立 Editor 模块
- 英文原文：[README.md](README.md)
- 负责人：Editor 与内容工具 Agent
- 权限归属：Scripts/Editor 及当前源码中受编辑器条件保护的制作/预览适配器；具体写集由 TASK 指定

## 当前工具

项目已有 Python 制作脚本、ACT/模型定义，以及地图预览/导入/验证适配器。“尚无项目编辑器代码”已过时。实际入口见 [WorldMapAuthoring](../../../Content/WorldMapAuthoring.zh-CN.md)、[ModelAuthoring](../../../Content/ModelAuthoring.zh-CN.md)、[ACTAuthoring](../../../Content/ACTAuthoring.zh-CN.md)。

工具创建/编辑经校验定义并展示报告，不拥有当前局状态，也不另写一套战斗/合成公式。预览复用运行时求值器；源配置与生成结果分开，重跑时尊重已有作者编辑。

## 近期职责

- 消费前校验 ID、引用、行/模式兼容与已配置值。
- 按 [ADR-0010](../../../Decisions/ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md) 完善地图生成/烘焙/元数据/Cook 路径。
- 错误可定位至资产/字段/原因；导入/生成失败保留之前有效内容。
- 专用编辑器暂缓期间，以普通类型化资产/表提供必要章节运行时数据。

编辑器 API 可依赖运行时契约，打包运行时不能依赖编辑器专用类。已有资产不是新增独立模块的理由，仅在实际编辑器依赖需要时建立边界。

## 暂缓的剧情编辑器

未来已接受设计是独立 ChapterSourceAsset 持有 UEdGraph，编译为运行时 ChapterDefinition。受限 DAG 包含 Start、Dialogue、Choice、Condition、TutorialAction、Effect、End，使用类型化执行连线，参数放在 Details；不支持任意 Blueprint 调用、通用数据 Pin 或并行脚本语言。

可采用 FAssetEditorToolkit、SGraphEditor、Details、FScopedTransaction、FMessageLog。这里只保留设计意图，不是当前依赖或实现任务。按用户最新决定，剧情图编辑/编译、文案/本地化录入工具和 PresentationCue 制作 UI 全部暂缓，不阻塞 Demo 运行时。

## 契约、检查与风险

公开工具结果包含源版本、资产/字段、严重性和失败原因；预览返回与运行时一致的类型化计算结果。导入/导出管线存在时再执行往返检查。

测试重复/缺失 ID、非法引用/模式、预览一致性、失败编辑保留和烘焙/Cook 一致性。运行时只保存稳定编译内容 ID，不保存 UEdGraph 或临时预览 Actor。工具不引入网络权威。

按需学习主题：Unreal 反射、编辑器扩展、验证、事务、图 Schema 与编译。主要风险是在可玩 Demo 路径完成前，先做出第二个大型编辑器产品。
