# TASK-049 原生策略界面与交互整理

- 英文源文档：TASK-049-Native-Strategy-HUD.md
- 状态：Review — 实现已检查；等待用户试玩
- Primary：root，当前 Local codex/mvp-demo-foundations
- 授权：用户要求参照所附策略 UI 图修改，并明确选择原生 UE 可玩版。

## 结果与范围

围绕真实 3D 地图重新组织原生界面：紧凑的日历/资源顶栏、右侧节点详情与上下文操作、底部插画手牌与本地驻军选择、独立阶段推进，以及次级菜单/准备页面。保留已接受的局内/战斗命令、数值、拓扑、存档格式及模式隔离。参考图中的名称、数值和场景仅是视觉示例，不构成新增规则。

允许路径：Source/DreamOfPadma 下的 Public/UI/Screens/PadmaGameScreen.h、Private/UI/Screens/PadmaGameScreen.cpp、Public/Game/Framework/PadmaPlayerController.h 和 Private/Game/Framework/PadmaPlayerController{,UI,Map}.cpp；Private/Tests/UI 下的针对性测试；新增 Content/Padma/UI 资产/原图；Scripts/Editor/AuthorStrategyUI.py；本 TASK 双语文件、NativePlayableDemo 指南双语文件、00_INDEX、ProjectState 和 Changelog 双语文件。

地图/战场输入使用真实 Widget 几何范围。明确出发卡组与目标节点查看的区别；隐藏无关战斗路线按钮；保留免费预览/取消。读档覆盖及撤退等局内操作提供确认；所有模态窗口显示时阻止底层输入。不改 Core 规则、战斗结算、世界资产、依赖配置、历史原型或无关工作区改动。

委派：一个只读控制器流程检查；一个仅负责新增卡面图集 PNG 的图像工作者。root 负责源代码、UE 通道及收尾。

## 验收

- [x] 真实渲染具有深蓝／金色层级、插画卡牌、情境详情及独立阶段操作。
- [x] 命令回归覆盖卡牌选择／筛选／附着／取消、备战草稿与弹窗优先级；既有移动／合成／战斗／存档测试通过。
- [x] 输入使用真实控件范围与统一弹窗判断，限定代码审查通过；实际鼠标／拖放／键盘试玩见下方。
- [x] Editor 编译、定向自动化、真实原生画面检查及文档审计通过。
- [ ] 用户审美验收及实际输入／拖放试玩。

## 证据与剩余工作

已实现：紧凑的真实资源／日历顶栏、真实天平进度、右侧节点／驻军与情境操作、查看目的地时保留来源选择、B 卡附着到首张合法本地 A、插画手牌与全部／A／B／C 筛选、图鉴及固定合成来源、真实拓扑小地图、独立时段推进、菜单读档／撤退确认、统一备战／局部场景导航、弹窗输入阻挡、按控件范围判定镜头／输入、清晰的战斗选中条目及 1280×720 短按钮、隐藏准备页面空技能栏。

改动为范围内六个运行源文件、`Private/Tests/UI/PadmaStrategyInteractionTest.cpp`、`Content/Padma/UI` 下的 PNG 图集与导入后的 `Textures/T_CardIllustrations.uasset`、导入脚本及列明的双语文档。图集为生成的 1254×1254 概念插画，九格各 418×418；未改世界模型或游戏数据。

证据根目录：`C:/Users/lenovo/.codex/visualizations/2026/09/09/01a084b7-b919-7d61-8ca1-bf77da2ef822/task049`。

最终交付预览：`world-final.png`。`import-repeat.log` 为零错误／警告，重跑后已有纹理 SHA-256 仍为 `BA313CFA35FDC687B7F632FBD8D750081ADE205E503AC4F4CC515220625B4460`。

- `build.log`：最终 DreamOfPadmaEditor Win64 Development 编译成功。
- `Tests/index.json`：22 项既有 WorldMap／TASK046／TASK040／TASK047 测试通过，21 项无警告、一项为预期的非法生成警告，零失败。筛选：`DreamOfPadma.WorldMap+DreamOfPadma.MVP.TASK046+Project.MVP.TASK046+Project.Naming.TASK047+DreamOfPadma.MVP.TASK040`。
- `UITests/index.json`：`DreamOfPadma.UI.StrategyInteraction` 通过，调用真实控制器命令，覆盖 C/A/A 混合选择中的首张 A 附着、跨节点附着目标拒绝、筛选／开关选择、草稿保留、读取确认及 Esc 顺序，没有写入真实存档槽。引擎启动在测试入队前输出过 `Condition failed`，上述选定测试报告没有失败。
- 原生 `-game` 截图：1920×1080 的 `world.png`、1280×720 的 `small-final.png`、`home-final.png`、`encounter-final.png`、`act.png`。`home-final.log` 确认场景设置与截图时均为 home。较早的 `home.png` 实际截到地图，不作为归处证据。
- `import.log`：图集导入无 Python 错误。对照源文件快照及新文件的 `git diff --check` 通过；AuditDocs 检查 232 个 Markdown／116 对双语，ValidateProject -Strict 通过。
- 只读控制器审查发现附着目标不一致和草稿重置回归；修正后由同一审查者限定复查，结论 Pass。

待用户检查：打开 `L_PadmaWorld`，试玩手牌筛选、晌午 C 卡部署与目标移动、A/B 附着与合成预览／取消、菜单关闭、备战／局部场景返回，以及战斗拖放／确认／撤退。命令测试和截图不代表鼠标手感已验收；computer-use 目标窗口在交互验证前关闭。ACT 单位聚集时标签仍可能重叠。世界／角色资产仍为占位，正式美术、长局平衡及打包兼容性不在本次交付内。没有暂存、提交、合并或推送。
