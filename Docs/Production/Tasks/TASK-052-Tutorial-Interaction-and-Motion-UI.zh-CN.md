# TASK-052 教程交互与动态 UI

- 英文原文：TASK-052-Tutorial-Interaction-and-Motion-UI.md
- 状态：Review（实现已验证；用户美术／长按手感验收待完成）
- Primary／Local：Codex Primary／codex/mvp-demo-foundations
- 用户授权：修复五个试玩问题：悬浮分区刷新、选择 A/B 后点击地块并确认部署／选择附着对象、纵向羊皮纸手牌、WASD 与减少教程格数、50:50 动态战争天平及精致运行时 UI 动效。

## 结果与范围

保留固定教程绘景及真实规则。最新跟进改为单击固定右侧详情，悬浮仅更新地图高亮，不替换顶部、目标或手牌。明确点击目标时冻结卡牌和地块 ID：A 需确认，B 需选择同地块 A 或显示无目标弹窗；确认时重新校验当前状态，不绕过费用和阶段限制。保留原战争分数／胜利阈值，中立值显示为 50:50，将有符号进度映射为双方比例。

允许源码：Source/DreamOfPadma/Public 与 Private/UI/{Screens,Input,Layers}；Public/Game/Framework/PadmaPlayerController.h 与 Private/Game/Framework/PadmaPlayerController{,UI,Map,Inspection}.cpp 及有界部署辅助文件；Public/Private/Core/{Content/PadmaContentTypes,WorldMap/PadmaTutorialMap,WorldMap/PadmaWorldMapDefinition,Run/PadmaRunRules}；可信兼容确有需要时的 Public/Private/Game/Run/PadmaRunSubsystem；Private/Tests/{UI,World}。资产／脚本：Content/Padma/UI/SourceArt、UI Textures/Materials、Scripts/Editor/AuthorMotionUI.py，以及反射夹具字段变化时的 ExportPlayableData.cjs。不改第三方资产、无关配置、关卡和战斗结算。

文档：本 TASK 中英、受影响 StrategyPresentation、WorldState、ADR-0008/0009、确有变化的存档／创作契约、ProjectState、索引及 Changelog 中英。Artifacts/TASK-052 保存基线、提示词、截图与检查。UE 单通道；独立工作包分别负责控制器、Core 网格及美术创作，Primary 负责控件集成。

## 验收

- [x] 悬浮保留非详情控件身份、滚动位置和选择，不重建整个 HUD。
- [x] 选 A/B 后点击目标：A 确认，B 合规 A 列表／无 A 提示；取消和过期状态安全，真实部署／附着成功。
- [x] 手牌换行后纵向滚动，具有羊皮纸纹理和清晰计数。
- [x] 初始缩放下 WASD 可平移；新教程显著减少格数、保留七个关键点；旧网格存档一致。
- [x] 战争 UI 初始 50:50，中间天平图标、双方动态液体，读取真实战争进度。
- [x] 哑光深蓝／旧金／纸张配色、克制扫光／揭示／错峰动效，保留 CommonUI 模态路由。
- [x] Editor 编译、定向测试、原生交互／渲染证据与限定独立审查。
- [x] 最终双语文档及限定空白检查。

## 2026-09-10 跟进：PIE 退出、单击详情与附近守军

用户明确要求修复 PlayLevel.cpp:553 的 PIE 退出断言，改为单击固定地块详情，并在归处旁提供敌方驻军节点。此跟进由 Primary 串行实现，原任务其他视觉验收边界保留；下文早期悬浮／v2 证据仅描述跟进前版本。新增文档写入范围为受影响的 Design/EN 与 Design/ZH/06_MapAndPreparation.md 对照。

- 控制器仅在单击时改变 SelectedNode／PreviewNode 并排队局部详情；悬浮不改侧栏。select-node 不再无条件触发完整 HUD 刷新；A／B 模态流程仍按需完整刷新。
- EndPlay 通过幂等 ShutdownPresentation 清空回调、取消输入并显式释放手动 CommonUI 栈与 Slate 缓存。按钮闭包强持有 MID 改用 transient package Outer。原崩溃日志未给出完整根引用路径；这两条源码保活路径修复后以单次 GC 和真实 PIE 退出证据确认未再复现。
- 新教程生成器 v3 与 v2 共用 55 格投影；薪火台（fire）初始为王庭、1 名守卫，归处直达。v1／v2 和新局在可信手工参照下可往返恢复，旧局不补守军。没有修改关卡、配置、资产或战斗结算。
- 精确跟进源码差异与基线保存在 Artifacts/TASK-052/ClickExit/Before 和 changed-files.json；涉及 PlayerController 的退出／Map／UI／Placement，GameScreen、ActivatableLayer、TutorialMap 及其 UI／World 测试。新增 PadmaUISessionReleaseTest.cpp。
- ClickExit/build-final.log：Editor 构建成功。ClickExit/TestsFinal/index.json：32 项通过（31 项无警告，1 项预期非法生成警告），零失败／未运行。包含一次 GC 释放 GameInstance／Screen／栈、重复 shutdown、仍持有按钮、点击／悬停隔离、A／B、直达 Encounter、v1／v2／v3 恢复与非法锚点拒绝。首轮新增测试对守军数量施加了超出既有固定锚点契约的限制，已改为真实契约的锚点位置拒绝；未扩大存档校验范围。
- ClickExit/pie-result.json 与 pie.log：真实 Editor、D3D12 RHI 连续 3 次 PIE 开始／停止；每轮打开 map→菜单→图鉴→角色定义详情，均正常退出，未再出现 PIE／GC 残留断言。启动阶段另有未归因的引擎自动化测试日志，因此不声称整份日志零错误。
- 独立只读审查 Pass，无未解决 P0–P3；核对当前差异、最终构建／32 项测试及三轮 PIE。原生试玩窗口已打开；鼠标操作因桌面应用授权超时未完成，单击规则证据来自自动化测试，不冒充鼠标实测。
- 跟进文档检查：AuditDocs 通过（284 个 Markdown／142 对），30 个修改文件的限定空白检查通过。日志为 ClickExit/audit-docs.log 和 ClickExit/whitespace-check.log。未执行 Git 集成。

## 证据与剩余工作

证据根目录：`Artifacts/TASK-052/`。保留现有未提交的 Local 工作；任务起点源码／文档基线位于 `Before/`。

- 悬浮采用仅详情投影与持久右侧容器。顶部、目标、阶段、手牌控件和滚动位置均保留；真实资源／手牌变化才更新对应区域。排队的完整刷新不会被后续局部悬浮替换。旧 3D 地图侧栏同样遵守分区契约。
- 部署冻结所选卡牌与点击地块 ID。A 确认和 B 实例列表在扣费时重新执行现有 Core 校验，禁用项显示原因。超过三个弹窗操作时使用有界纵向滚动区，包含关闭操作。无 A、过期和取消不扣费。点击、拖放及侧栏部署进入同一流程。
- 新教程生成器 v2 使用 55 格、归一化地图坐标半径 10，保留七个关键点位置。共享 `FPadmaHexGridConfig` 驱动生成、精确拾取和描边。可信 v1 配置保留 162 格旧存档，兼容没有新增可选网格字段的旧文件；伪造布局原子拒绝。首次平移提供 1.12 倍放大余量且不露空白，Home 恢复完整构图。
- 绘景地图由 Slate 处理鼠标选点、滚轮和中键平移；CommonUI 页面转发导航键。捕获期间消费并清除导航键，切页／弹层变化取消导航和地图自身捕获。绘景不再由 PlayerInput 重复轮询 WASD；3D 相机保留控制器路径。
- `T_HandParchment.png` 与五种项目 UI 材质提供纸面、哑光深色、金色扫光、液体天平和立绘遮罩揭示。`AuthorMotionUI.py` 可重复运行并保留现有 MI 覆盖值；最终已知哈希迁移只改战争材质 shader。`motion-ui.json` 为 ready，无警告。未改第三方资产。
- 战争分数仍遵循原规则，显示比例为 `clamp(0.5 + 0.5 * War / warGoal, 0, 1)` 与其补值：初始 War=0 显示 50:50。液体包含动态液面／泡沫／气泡，中间天平为矢量几何。原生 Slate 元素提供缓动错峰入场，各控件动态 UI 材质提供揭示／扫光。CommonUI 仍负责激活和输入；本次没有新增 AE 视频、Niagara 系统或 UWidgetAnimation 资产。
- `build-final.log`：禁用按钮文字对比度小修正后，最终 Editor 构建 **9.43 秒成功**。`Tests/index.json`：**35 项通过**（34 项无警告、1 项预期非法生成警告），零失败／未运行。`Tests-InputFinal/index.json`：最终键盘／捕获修复后、最后纯颜色调整前，**5/5 UI 测试通过**，零警告／失败。覆盖真实控件身份／滚动／排队状态、多目标可达性、部署取消／过期、生成版本、旧存档、拓扑与相关已有规则。
- `native-input-final.log`：Primary 使用原生 Windows 输入选择地痕、点击归处并确认 A 部署（**心流 60→56**）；选择 B 蓍、点击无 A 地块，观察提示、验证 W 被阻断，取消未扣费。再选归处并明确选择地痕实例后附着 B（**心流 56→55**，驻军数 2）。W、Home 正常，真实 RHI 下观察到液面变化。日志记录会话／弹层，输入工具展示的截图和上述观察构成 UI 结果证据。
- `native-final.log`：消除重复键盘所有者后，Primary 将七张手牌滚到第二行，悬浮另一地块时滚动位置不变；W 平移、打开菜单后 W 不移动底图、Esc 关闭后 Home 恢复全览。顶部和手牌位置保持稳定。
- `tutorial-final.png`／`capture-final.log`：检查了实际 1920x1080 最终 HUD，包括可读的禁用操作文字。`details-final.png`／`capture-details.log`：检查立绘遮罩结束后的实际卡牌详情、当前属性与能力文本。已配置数据如实展示，独立立绘／等级／技能缺失仍明确标注。这些渲染不代表与参考图像素一致。
- 独立只读审查：**Pass**，无未解决 P0-P3。已关闭的问题包括旧地图局部刷新、过长附着列表、动态材质父对象、排队刷新保留及混合键盘来源。
- 最终检查：`AuditDocs.ps1` 与基线到当前文件的限定空白检查通过，包含新增源码。日志为 `audit-docs.log`、`whitespace-check.log`，`changed-files.json` 记录本任务的有限差异。没有结构／共享配置变化，因此无需扩大项目审计。

用户美术／实体长按手感、持续鼠标与按键组合手势，以及打包版本行为仍未验证。本包未授权也未执行提交、合并或推送。
