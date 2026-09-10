# TASK-051 绘制教程与悬停侧栏

- 英文主文档：TASK-051-Painted-Tutorial-and-Hover.md
- 状态：Review（实现已验证；用户美术验收待完成）
- Primary／Local 工作副本：Codex Primary / codex/mvp-demo-foundations
- 用户授权：2026-09-09 要求固定教程绘画与着色器氛围、参考风格 HUD 和悬停侧栏；保留既有关键节点，生成玩家及 AI 可占领的多地形宜居荒野。

## 结果与范围

交付具有真实领地与上下文动作的原生绘画教程地图。金色／深蓝界面使用等比缩放的 1672×941 设计画布，顶部为日历／资源，中央为绘画，右侧为节点预览，底部为卡牌。显示的资源、归属、驻军和移动费用来自当前运行局。固定源绘画为 1774×887（2:1）；视觉输入为用户提供的参考图，美术请求记录在 [art-prompt.txt](../../../Artifacts/TASK-051/art-prompt.txt)。这是参考引导的重建，不声称逐像素复刻。

纯值教程生成器产生 162 个地块，包含位于精确绘画位置的七个既有关键节点，以及带种子的多地形荒野。`tutorial-painted-v1` 显式启用六边形相邻连接及可占领宜居地块。新局默认选择此布局，除非使用 `-PadmaLegacyMap`，或由 `encounter`、`act`、`roundtrip` 场景请求旧初始化。既有运行局保留已冻结的布局。[ADR-0009](../../Decisions/ADR-0009-Painted-Tutorial-Hex-Map.zh-CN.md) 记录拓扑、固定位置与表现所有权。

玩家占领使用已校验的移动和既有费用。新增 AI 边疆机制在黄昏最多选择一块相邻、空置、中立、宜居荒野；不新增攻击玩家领地、单位或经济。荒野不奖励战争天平／核心／资源，生成道路保留手工锁定路线。已保存的布局元数据与运行时归属承载其结果。CommonUI 激活和详情计时继续遵守 [ADR-0008](../../Decisions/ADR-0008-Activatable-Presentation-Layers.zh-CN.md)。

悬停仅修改预览状态。进入侧栏时保留最后预览；已选卡牌继续保持选择，显式动作使用预览节点的稳定 ID。模态覆盖层与拖动手势阻挡地图输入。除明确批准的教程荒野行为外，既有战斗、合成、对话、卡牌详情与备战规则不属于本次表现修改。

### 精确允许路径

下列源码路径均相对于 `Source/DreamOfPadma/`：

- UI：`Public/UI/Screens/PadmaGameScreen.h`、`Private/UI/Screens/PadmaGameScreen.cpp`、`Public/UI/Screens/PadmaTutorialMapWidget.h`、`Private/UI/Screens/PadmaTutorialMapWidget.cpp`。
- 控制器：`Public/Game/Framework/PadmaPlayerController.h`；`Private/Game/Framework/PadmaPlayerController.cpp`、`PadmaPlayerControllerMap.cpp`、`PadmaPlayerControllerUI.cpp`、`PadmaPlayerControllerInspection.cpp`。
- 运行集成：`Public/Game/Run/PadmaRunSubsystem.h`、`Private/Game/Run/PadmaRunSubsystem.cpp`。
- Core：`Public/Core/Content/PadmaContentTypes.h`、`Private/Core/Content/PadmaContentTypes.cpp`、`Private/Core/WorldMap/PadmaWorldMapDefinition.cpp`、`Public/Core/WorldMap/PadmaTutorialMap.h`、`Private/Core/WorldMap/PadmaTutorialMap.cpp`、`Public/Core/Run/PadmaRunRules.h`、`Private/Core/Run/PadmaRunRules.cpp`、`Private/Core/Run/PadmaRunWilderness.cpp`。
- 测试：`Private/Tests/World/PadmaTutorialHexTest.cpp`、`Private/Tests/UI/PadmaStrategyInteractionTest.cpp`。

项目相对创作路径：`Scripts/Editor/AuthorTutorialPresentation.py`、`Scripts/Editor/ExportPlayableData.cjs`、`Content/Padma/UI/SourceArt/T_TutorialMap.png`、`Content/Padma/UI/Textures/T_TutorialMap.uasset`、`Content/Padma/UI/Materials/M_TutorialMap.uasset`、`Content/Padma/UI/Materials/MI_TutorialMap.uasset`。验证产物限定在 `Artifacts/TASK-051/`。

文档：本 TASK 配对；`Docs/Decisions/ADR-0009-Painted-Tutorial-Hex-Map` 与 `Docs/Content/StrategyPresentation` 配对。Primary 负责必要的 `Docs/Rules/WorldState`、`Docs/Rules/Time`、`Docs/ProjectState`、`Docs/00_INDEX` 与 `Docs/Changelog` 配对更新。受委派的文档写者仅修改前三组配对。既有运行规则与布局序列化字段承载日历／存档值；`PadmaRunSubsystem` 额外传递原始手工地图引用，用于跨地图存档校验。保留无关本地改动，使用单一串行 UE 执行通道。

## 验收

- [x] 生成的贴图／材质／实例校验成功；源与导入贴图均保留 1774×887 像素，UI 参数回读正确，资产报告无警告。
- [x] 原生教程从等比缩放的设计画布，在 1920×1080 正确显示绘画、金色／深蓝 HUD 与实际数值；Primary 已检查参考风格构图，不声称像素相同。
- [x] 固定时间截图中的水光、雾气与熔炉效果保持局部，地标 UV 固定；`FreezeAnimation`／`PreviewTime` 已通过截图验证，强度归零时返回原始绘景的分支已检查材质创作源码。
- [x] 162 地块图确定性生成，保留七个精确关键位置与剧情语义、锁定路线，且仅包含已校验的可通行连接。
- [x] 玩家移动支付既有费用；黄昏 AI 仅扩张至合规中立荒野；不能刷荒野战争／奖励，不攻击玩家领地，战斗／对话中不发生边疆变化。
- [x] 存读档恢复六边形元数据、拓扑与归属；手工／教程地图的读取顺序与非法锚点的原子拒绝检查通过，相关旧功能回归通过。
- [x] 悬停立即投影正确节点，保留侧栏预览和已选卡牌，不提交玩法变更。模态／拖动阻挡与显式动作目标通过针对性回归；原生悬停、侧栏保留、缩放、全览与菜单阻挡已实际操作。
- [x] 编辑器构建、针对性 Core／UI 与相关旧功能回归、独立边界审查及原生运行检查通过。
- [x] 集成修改完成后的最终 AuditDocs 与严格结构检查。

## 证据与剩余工作

证据根目录：`Artifacts/TASK-051/`。最终动画截图与集成检查已完成。

- `art-prompt.txt`：基于用户参考图记录的图像生成请求。交付源文件为 `Content/Padma/UI/SourceArt/T_TutorialMap.png`。
- `author-presentation.log` 与 `tutorial-presentation.json`：资产制作重跑已完成，报告 `status=ready`，资产报告无警告。贴图和父材质被保留，材质实例已创建。源与导入贴图均为 1774×887。JSON 记录着色器哈希及实际参数值。
- 创作默认值：`AnimationStrength=0.7`、`CloudStrength=0.55`、`FreezeAnimation=0`、`PreviewTime=0`；`MapTexture` 引用教程绘画。确定性氛围使用 `FreezeAnimation=1`、`PreviewTime=0`；`AnimationStrength=0` 显示基础美术。用法与精确资产路径集中维护于 [StrategyPresentation](../../Content/StrategyPresentation.zh-CN.md)。
- `build-final.log`：牌库计数排版修复后的最终 DreamOfPadmaEditor 构建成功，耗时 9.58 秒。后续全览按钮修改已通过原生会话检查，仅影响排版的计数修复已通过最终渲染检查。
- `Tests/index.json`，报告 `2026.09.09-14.53.22`：**31 项通过**（30 项无警告，一项预期的非法生成警告），**0 失败／0 未运行**。`tests-final.log` 以测试退出码 0 结束。覆盖教程生成／锁定分区、移动费用／边疆／奖励边界、战斗冻结、悬停／手势行为及相关旧功能／存档／战斗回归。
- 跨地图存档权威修复：启动和 `StartTutorialMap` 在生成教程前复制原始手工布局；`UPadmaRunSubsystem` 将其传给 `FPadmaRunRules::Initialize(Content, AuthoredReference)`。恢复使用保留的可信引用进行校验。`AuthoredAndTutorialRestoreOrder` 验证两种读取顺序、已保存数值不变，以及对移动锚点或未配置地图身份的原子拒绝。
- 独立只读边界审查：**Pass**，无剩余 P0–P3 问题。
- `tutorial-time0.png`：Primary 已检查实际原生 1920×1080 渲染。`interactive.log` 记录全览修复后的原生 Windows 会话。通过零滚动输入移动指针，无需点击即可更新风蚀原（`plain`）侧栏；进入侧栏时保留该目标。加号按钮执行缩放，全览按钮释放后复位；Escape 打开菜单并阻挡底层悬停，再次 Escape 关闭后恢复归处悬停。这些交互结果由 Primary 实际观察；日志记录会话与层激活。
- 当前原生自动化 API 不支持持续按下 500 ms。长按阈值、移动取消及消费释放已有手势自动化证据；实体长按手感仍待用户试玩。
- 文档工作包检查：`AuditDocs.ps1` 通过（248 个 Markdown 文件／124 组配对）；六个文件的限定范围空白检查通过。最终集成检查仍由 Primary 负责。
- `tutorial-final.png` 与 `tutorial-time4.png`：使用 `-PadmaMapArtTime=0`／`4` 的最终 1920×1080 原生截图。`animation-frame-check.json` 记录水光（572/816）、山谷薄雾（714/1620）和熔炉（331/440）采样点的像素变化；固定 HUD 采样完全相同（0/1260）。地标保持原始 UV，最终计数排版清楚可读，两份日志均无运行错误。
- 最终 `AuditDocs.ps1`：248 个 Markdown 文件／124 组配对通过；`ValidateProject.ps1 -Strict` 和 `ExportPlayableData.cjs --check` 通过。限定源码／文档空白检查包含未跟踪文件；资产和证据保留本地，未执行 Git 集成。

用户视觉／试玩验收与打包行为仍未验证。本工作包未获得提交、合并或推送授权，也未执行这些操作。
