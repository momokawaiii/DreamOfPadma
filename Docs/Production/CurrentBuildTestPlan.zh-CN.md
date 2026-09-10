# 当前 UE 与 HTML 版本测试方案

- 英文原文：CurrentBuildTestPlan.md
- 文档 ID：TESTPLAN-CURRENT-001
- 版本：0.3
- 状态：当前原生回归与历史 HTML 草图用例；视觉验收进行中
- 基线日期：2026-09-09
- Owner：[TASK-041](Tasks/TASK-041-Current-Build-Test-Plan.zh-CN.md)
- 执行结果：TASK-048 集成报告 Editor 构建、最终运行时渲染及 22 项相关自动化通过（21 项无警告，1 项预期警告），运行时 PCG 生成 200 个实例。本次文档编辑没有重跑这些验证；下列手动验收仍作为用户检查清单。

## 当前 HTML 入口更新

TASK-042 已新增 [可玩 HTML](../../Prototypes/HTML/index.html)，以 [v0.2 试玩指南](../../Prototypes/HTML/README.zh-CN.md) 的 1–8 步验证归处编队、对话与原地合成。下方表格和 HTML01–HTML13 保留 TASK-041 时的旧草图基线，不代表当前 HTML 的功能上限。当前代码／状态证据见 TASK-042；新增浏览器验收待完成。UE 制作已恢复为当前原生实现；按 [NativePlayableDemo](../Content/NativePlayableDemo.zh-CN.md) 与 [WorldMapAuthoring](../Content/WorldMapAuthoring.zh-CN.md) 执行。

## 1. 本方案验证什么

当前 UE 验证覆盖原生地图、冻结本局、检查点分支、分离的 Encounter／ACT GAS 及模型／ACT 配置。HTML01–HTML13 保留为 TASK-041 历史画面草图检查，不描述当前原生功能。

| 范围 | UE 候选 | HTML 草图 |
|---|---|---|
| 地图／进入 | 创作／种子生成单向地图及地图到战斗返回 | 七个历史示例节点，悬浮和局部查看 |
| 人物／建筑显示 | 真实 UE 组件和软引用资产，使用占位几何体 | CSS 剪影和静态文字 |
| 技能 | Encounter／ACT 来源分离；通用模型预览无技能 | 五个共享槽位示例，可切换模式标签 |
| 全局配置 | 当前原生配置 UI 与 ACT Data Asset 配置 | 六个草图分类，没有真实装配／增益服务 |
| 日历／资源／合成／GAS／胜负／存档 | 当前原生样例已实现，按其指南验证 | 历史静态草图占位 |
| 卡牌移动规则 | 原生整组原子移动／扣费 | 草图仍无“移动到此处”执行 |

先执行 MAP01–MAP06 与[原生试玩指南](../Content/NativePlayableDemo.zh-CN.md)，再执行 UE08–UE13 和当前自动化；按需要增加 Q 样例失败检查及可选 ACT 素材检查。仅在复核旧画面草图时执行 HTML01–HTML13。

## 2. 版本基线与启动入口

UE：用已安装的 UE 5.8.2（`E:/Epic Games/UE_5.8`）打开 [DreamOfPadma.uproject](E:/2026ue/DreamOfPadma/DreamOfPadma.uproject)。记录当前分支、HEAD 与工作区状态，单独 HEAD 无法标识候选。模块过期时构建当前 Editor 目标，保留引擎关联。

在内容浏览器明确打开以下地图，不依赖启动地图设置：

- `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld`：当前原生地图与战斗路线。
- `/Game/Padma/World/Maps/Editing/L_MapAuthoring`：编辑器图／手柄流程。
- `/Game/Padma/MVP/Presentation/ModelPreview/L_ModelAuthoringPreview`：UE08–UE11。

使用单人 PIE 和正常 Play，不用 Simulate。先保存自己的工作；需要改测试定义／地图时使用副本。切换地图前停止 PIE。模型预览使用固定镜头，尚未实现移动／攻击输入。

HTML：使用 Chrome 或 Edge 打开 [qa-padma-world.html](C:/Users/lenovo/.codex/visualizations/2026/09/08/01a07f68-77cf-7fe0-8479-93a66fb8a4f6/qa-padma-world.html)。如果应用把文件作为文本显示，可在浏览器 Ctrl+O 选择该文件。它是独立运行包装页；源片段是 [padma-world-design.html](C:/Users/lenovo/.codex/visualizations/2026/09/08/01a07f68-77cf-7fe0-8479-93a66fb8a4f6/padma-world-design.html)。两者为 Git 仓库外本地文件，不需要服务器。编码、主题和 iframe 测试使用包装页；检查元素时，开发者工具 Console 应选草图框架上下文。

记录浏览器版本、视口宽度、缩放和主题。编写方案时已核对两个指纹：

- 源文件 SHA-256：`808D281C7F2F3C6FDB36C497CEDAD11A4696EF708590124DF251C3900BD10DF9`。
- 独立包装页 SHA-256：`6CBAC3D4B01B3A41E87A001FE5ECF6C89937152728832ECCD0DDAA91FE9F47C3`。

## 3. 观察与恢复约定

每项记录实际现象，以及 Pass（通过）／Fail（失败）／Blocked（受阻）／Not applicable（不适用）。初始状态为 Not run（未执行）。负面测试按预期拒绝错误输入即通过；必需素材缺失只阻塞依赖它的用例，未来功能缺失不等于现有功能回归失败。

UE 证据为画面加相关 `[PadmaMap]`、`[PadmaMapPCG]`、`[World Map]`、原生本局／战斗、`[TASK-040]`、`[ACT Authoring]` 日志。TASK-007/008 日志名属于历史，不是当前冒烟目标。

HTML 证据为对应节点／页面／标签、实际文字与 Console 异常。简短复现步骤和日志摘录即可；截图／视频可选，视觉缺陷时有帮助。

停止／重启 PIE 重置运行时状态，不会撤销 Data Asset 编辑。每个失败变体后恢复明确修改的字段，或使用已知测试副本；刷新重置 HTML 浏览状态。不要用 git reset／restore 或删除项目资产来“恢复测试环境”。

## 4. UE 手动用例

| ID | 起点与操作 | 预期结果与证据 |
|---|---|---|
| MAP01 | 打开 L_PadmaWorld 并进入 PIE；悬停／选择节点，滚轮缩放、中键拖动、WASD／方向键平移、Q/E 旋转及 Home 取景。 | 地图与面板可读，查看不改资源；普通状态刷新保留镜头位置，Home 自动取景。 |
| MAP02 | 用同一种子开两次新局，再尝试其他种子；记录地图 ID、种子、节点／边身份和 NPC 位置。 | 同输入复现拓扑与固定锚点；抽样种子可产生不同支路。生成有效后才替换当前未保存的新局。 |
| MAP03 | 分别新开游戏：先完成 watcher 再到 gate，或绕过人物事件到 gate；查看之后的晌午对话。 | 到检查点前不记录错过；抵达时只判定一次 matched／missed，晌午对话沿用该分支；取消／重开不重判，不要求所有主线点都必须访问。 |
| MAP04 | 在副本上按 WorldMapAuthoring 操作：移动／应用模板手柄、生成、尝试移动生成锚点、导入正确／错误检查点行。 | 有效编辑显式保存；非法编辑／导入／生成保留旧值；生成锚点保持模板身份／坐标。Clear Preview 后不残留临时地图／节点／模型 Actor。 |
| MAP05 | 检查六边形地表、道路、标记及 PCG，多次悬停，离开／返回地图页面。 | PCG 完成日志显示实际实例（当前工具配置为 200）；碰撞不阻挡拾取，普通悬停不改变 layout-build；隐藏地图子对象不泄漏到其他页面。记录发现的视觉问题。 |
| MAP06 | 保存生成地图本局，进入／返回战斗，再修改配置地图后读取存档。 | 存档／战斗快照恢复自身冻结图及检查点状态；只提交一次，退出／失败沿用既有回滚契约。战斗准备按原生试玩指南执行。 |
| UE08 | 打开 L_ModelAuthoringPreview，选中 Home - Model Authoring Preview，连续两次 Rebuild Preview，再两次启动／停止 PIE。 | 始终只有三个表现对象：椭圆坐镇占位、建筑方块、地表平面；无重复模型或多余默认 Pawn。[TASK-040] 中 Garrison 为 Visual=1 Skills=0，Building／Ground 为 Visual=1 Skills=0。只数表现对象，灯光／镜头等 Actor 不计。 |
| UE09 | 使用下方 Q 测试副本。记录 QA 建筑定义 ID，只将 StaticModel 从 Cube 改为 Sphere 后重建；再清空模型引用重建，最后恢复 Cube。 | 替换后画面改变而记录的 ID 不变；清空引用时旧建筑消失并提示模型错误；恢复可正常显示。通用模型来源不含技能是预期行为。 |
| UE10 | 在 Q 中只将 Garrison 绑定 DefinitionId 改为 QA.Missing 并重建，再恢复 Visual.Preview.Garrison 重建。Domain 保持 Model，SelectedSkillRow 保持 None。 | 旧坐镇模型及旧技能信息清除，日志为 Visual=0 Skills=0；建筑／地表仍可显示。恢复后重新解析无技能行的通用坐镇模型。直接改 Details 不等于调用 SetPreviewSelection；该 API 的自动清行由 AUT06 验证。 |
| UE11 | 在 Q 分别测试：复制当前建筑目录键；恢复后，再在复制的场景定义中制造重复 SlotId；恢复并重建。 | 重复选中来源被拒绝，不取第一条兜底；静态布局无效时表现对象为零，不构建一半场景。每种变体恢复后都回到三个表现对象，分别留日志。 |
| UE12 | 查看当前原生卡牌行与 ACT 角色的 Mobility，在副本上切换 Configured 和策略／ConditionId。 | Movable／Immovable／Conditional 元数据正确校验；改字段不执行移动，也不让 ACT 角色成为沙盘部署卡。当前运行时移动另按原生指南验证。 |
| UE13 | 打开未填写的 DA_ACTAuthoring_Template，点击 Validate Authoring，查看 Output Log 中 [ACT Authoring]。 | 空草稿被拒绝，给出缺失引用／字段原因且不崩溃。正确拒绝未完成数据即该用例通过，不是在测试可玩 ACT。若模板已填好，则另建空测试目录。 |
| UE14 | 具备兼容骨骼模型后，按下方 A 样例完成配置并校验，再把 QA ACT 角色绑定到 Q 的 Garrison 槽位并重建。 | 静态配置通过；显示选中模型并解析一条 QA ACT 技能；缺失的 ACT 描述保持为空。不代表 GAS、攻击或武器挂接已完成。没有可用模型时标 Blocked：缺素材；AUT04／AUT05 仍可验证人工构造样例。 |
| UE15 | 从通过的 A 样例出发，每次只改一项：未知 SkillId／行、重复激活绑定、未知特性／地形，或有素材时配置不兼容动画；逐项校验并恢复。 | 各适用变体均被拒绝并指明来源／行／原因。Q 中有效 ACT 模型可在技能未就绪时继续显示，不混入 Demo 技能。编辑器类型筛选禁止选入错误表结构时，用 AUT04／AUT05 测该非法输入。 |
| UE16 | 进阶可选：记录资产哈希并关闭 UE 后，对当前完整样例重跑既有配置脚本，见命令行附录。不要往共享原始地图注入错误数据。 | 不覆盖已有配置文件；AuthorModelPreview 每次都校验已保存既有地图。空 ACT 草稿应报告未完成；仅原本缺失的文件允许新建。无效既有地图已有 TASK-040 历史证据，新一轮失败重跑需隔离项目副本。 |

### Q 样例：UE09–UE11 的隔离模型编辑

在编辑器 `/Game/Padma/Tests/Manual/` 下建立手动测试文件夹。将预览地图、场景定义、目录和建筑模型资产复制进去。打开复制地图，把宿主 SceneDefinition、Catalog 指向对应副本；只将复制目录中的建筑条目指向复制模型，并将模型 VisualDefinitionId、目录键、Building 绑定键统一设为 `QA.Visual.Building`。坐镇／地表来源保持只读，SelectedSkillRow 为 None。修改错误输入前，先 Rebuild Preview 确认三个模型的基线。

复制地图的修改不能影响原始定义。槽位布局资产不拥有当前坐镇状态，它的 Level 软引用也不是切场请求。Cube／Sphere 测试不需要用户美术素材。

### A 样例：UE14–UE15 的可选 ACT 配置

把 `/Game/Padma/MVP/Definitions/ACTCharacterCards/` 中五份 ACT 模板复制到手动测试文件夹。它们是静态 QA 数据，不是新增可玩角色或数值配置：

1. 给角色、武器、技能设置互不重复的 ID 和显示名，例如 QA.ACT.Character、QA.ACT.Weapon、QA.ACT.Skill。
2. 给角色指定可用骨骼 Model。先将 AnimationClass 和技能 Montage 留空，之后再测可选兼容素材。
3. 在复制的 PadmaACTSkillRow 数据表加入 QA_Skill 行：SkillId=QA.ACT.Skill，ActivationBindingId=QA.Input.Primary，Definition 指向复制技能。
4. 技能的 AbilityImplementationId=QA.Executor.Preview；角色 SkillTable 指向复制的表。
5. 武器恰好填写一个模型，例如引擎 Cube；把角色／武器软引用加入复制的 ACT 目录。
6. 将 QA.Input.Primary、QA.Executor.Preview 加入目录 References 中对应集合，基线先留空可选特性／地形限制。
7. 执行 Validate Authoring。要在 Q 中预览，增加 ACTCharacter 目录条目，其 DefinitionId 与角色一致，填角色的强类型软引用；Garrison 选择该键。同一条目不要再填 Model。
8. 读取 Presentation 结果，再逐项制造失败并恢复通过配置。

这些 QA 字符串只声明校验词汇，不代表已安装 GAS 技能。缺少导入骨骼素材可以标 Blocked，不代填伤害、费用或冷却。

## 5. HTML 手动用例

| ID | 起点与操作 | 预期结果与证据 |
|---|---|---|
| HTML01 | 用 Chrome／Edge 打开 qa-padma-world.html，开启开发者工具 Console 后刷新一次。 | 中文可读，六边形地图上有七个示例节点按钮；两个都叫“归处”，需按阵营／类型区分。资源数值为 — 占位，无未捕获页面异常。数量只是布局样例，不是玩法上限。 |
| HTML02 | 只悬浮／聚焦各节点，重点检查玩家“归处”、“薪火”和统治者“关隘”，不点击。 | 侧栏对应更新名称、类型、地形、控制方。玩家归处显示 A 佛罗伦萨的曙光、附着 A 的 B 合一珠、C 维特鲁威人；空样例显示“未配置部署”。悬浮不进入局部视图。敌方政策旧文字另记 GAP01。 |
| HTML03 | 点击玩家“归处”，用“返回沙盘”返回；再用侧栏“进入地块”进入并返回。 | 两种入口都显示同一节点的局部查看，返回后保留该节点选择；不产生移动、资源或战斗行为。高亮只是查看焦点，不是归属或卡牌位置。 |
| HTML04 | 在玩家归处局部视图，先点“人物模型”，再点“工事模型”。 | 人物详情显示维特鲁威人 · C / R，以及完美肉体、黄金分割、食烟火；工事切换到独立占位详情。剪影为 CSS 图形，不要求真实三维模型、动画或效果执行。 |
| HTML05 | 返回后进入“薪火”等空样例，再查看统治者“关隘”。 | 不残留上一玩家节点的模型／详情；空样例没有人物／工事按钮。敌方内容尚未配置，不能据此宣布已实现敌方全公开（GAP01）。 |
| HTML06 | 分别从沙盘和局部场景打开“全局配置”并返回；配置页已打开时再点顶部“全局配置”，再“返回”。 | 回到原来的沙盘／局部视图并保留当前节点；重复打开不会覆盖返回目的地，也不会产生装配变更。 |
| HTML07 | 依次打开 ACT 出战、FPS 出战、共享基础技能、关系与成长、增益总览、卡牌图鉴。 | 激活标签与内容对应；ACT 为角色／武器／技能待配置，关系／增益为占位；图鉴显示 A·F／B·D／C·R。FPS 范围旧文案记 GAP02，不当作已接受设计。 |
| HTML08 | 在共享基础技能中切换 Encounter → ACT → Encounter，查看五项条目。 | 名称、顺序、槽号始终为 1—5：鱼骸骤雨、一刀两断、天鹤之拥、梦中彩笔、马尔可夫链；只改变模式标签，效果仍“待配置”。仅验证草图共享槽位显示，不代表完整卡库数据。 |
| HTML09 | 在两种模式分别点击技能条目，再浏览关系和增益总览。 | 页脚显示选中卡名与当前模式；浏览不会施放技能、装配武器、增加好感或执行 buff。字段目前没有后端玩法服务。 |
| HTML10 | 在视口宽 1280、768、390 CSS 像素下重复沙盘 → 局部 → 配置，用 DevTools Rendering 模拟浅色／深色 prefers-color-scheme。 | 节点、按钮、文字仍可访问，返回／配置控件不被裁掉、不出现妨碍使用的重叠；窄屏侧栏改为上下排列。允许框架内部纵向滚动。检查所有标签，实际遮挡算布局缺陷。 |
| HTML11 | 先聚焦草图 iframe 内，用 Tab／Shift+Tab 到节点，Enter 打开，再按 Escape；配置页也测 Escape。 | 键盘可到达控件；Escape 从局部返回沙盘，从配置返回之前视图。浏览器 Tab 是焦点导航，不是未来 ACT 卡库／子弹时间键。焦点在 DevTools／地址栏时先回到草图再判断。 |
| HTML12 | 切换多个视图／模式后刷新页面。 | 恢复初始沙盘／玩家归处／ACT 配置标签／Encounter 技能模式默认值。不宣称拥有真实卡库存档；刷新丢失浏览状态为预期。记录脚本异常。 |
| HTML13 | 可选：宿主显示 Tweak 调整控件时切换“地块信息密度”和“路线展示”，再切节点。 | 紧凑隐藏地形／设施块，展开恢复；路线切换只改变连线显示，不改节点身份／数量。纯片段宿主没有 Tweak 控件时记“不适用”，不记失败。 |

## 6. 与最新已接受设计的已知差异

GAP01／GAP02 描述历史 HTML 草图；GAP03／GAP04 区分旧基线与已实现的原生运行时，不再笼统声称 UE 系统缺失。

| ID | 当前差异 | 后续验收要求／Owner |
|---|---|---|
| GAP01 | HTML 仍显示敌方情报规则待确认，缺少敌方模型／内容样例 | 首版敌方全公开，含卡牌身份／附着／状态；TASK-024／037 及草图更新 |
| GAP02 | HTML FPS 标签仍显示范围待确认 | 明确未来能力、首版不可玩；ACT 保持实时 MVP；TASK-038 及草图更新 |
| GAP03 | 历史草图没有“移动到此处”执行器；当前 UE 已有原子移动 | 按 NativePlayableDemo 回归当前移动；草图验收仍限于静态行为 |
| GAP04 | 原“UE 尚未接通”判断已被原生实现取代 | 当前地图／检查点／PCG 视觉与交互验收继续由 TASK-048 执行 |

打开地块保持只读查看。旧 Demo Enter 路线已退役；当前验收使用 MAP01–MAP06 与原生指南。仅通过历史草图／模型用例不能证明完整游戏已完成。

## 7. 既有自动化与可选配置脚本重跑

| ID | 自动化完整名称 | 主要覆盖 |
|---|---|---|
| AUT04 | `DreamOfPadma.MVP.TASK036.AuthoringValidation` | ACT 身份、行类型、引用、骨架与特性／地形结构 |
| AUT05 | `DreamOfPadma.MVP.TASK040.ModelAndSkillResolution` | 移动性结构、静态／骨骼模型、模型／技能独立就绪 |
| AUT06 | `DreamOfPadma.MVP.TASK040.NodeBindingReplacement` | 替换、清行／清技能、无效布局与表现对象生命周期 |
| AUT07 | `DreamOfPadma.WorldMap.Generation.` | 四项生成／校验／原子编辑用例 |
| AUT08 | `DreamOfPadma.WorldMap.Run.` | 四项检查点／回滚／冻结地图／阶段用例 |
| AUT09 | `DreamOfPadma.MVP.TASK046.` | 当前目录、本局规则与分离 GAS 回归 |

先保存工作并关闭编辑器，保持一次只有一个 UE 进程。下方命令在用户临时目录生成新报告，使用进程级 DDC 参数避开本机此前 Installed DDC 启动失败，不修改项目配置。

```powershell
$qaRun = Join-Path $env:TEMP ('PadmaQA-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Path $qaRun | Out-Null
& 'E:/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' `
  'E:/2026ue/DreamOfPadma/DreamOfPadma.uproject' `
  -unattended -nop4 -nosplash -NullRHI -NoSound `
  -DDC-ForceMemoryCache -ddc=NoZenLocalFallback `
  '-ExecCmds=Automation RunTests DreamOfPadma.; Quit' `
  '-TestExit=Automation Test Queue Empty' `
  "-ReportOutputPath=$qaRun/Automation" `
  "-abslog=$qaRun/Automation.log"
$LASTEXITCODE
$qaRun
```

通过要求：当前相关用例必须有成功记录，失败／跳过需调查；以 TASK-048 套件清单为准（本次集成基线为 22 项相关测试）。AUT01–AUT03、UE01–UE07 属于已退役 TASK-007/008 样例，不再要求运行。须读取实际报告，退出码 0 或空报告不足以证明通过；`DreamOfPadma.` 也可能包含其他活动项目测试。

NullRHI 不能验证可见渲染，MAP01–MAP06 与 UE08 的画面检查须单独做。已知引擎启动 widget-factory／r.MotionVectorSimulation 警告及引擎自测发现信息记录在 TASK-040；可与基线对照，但新增项目错误仍需调查。

UE16 比较 AuthorModelPreview 运行前后的既有 .uasset／.umap 哈希。AuthorACTTemplates.py 仍可单独重跑，保留空草稿未完成的预期处理。地图编辑脚本按 [WorldMapAuthoring](../Content/WorldMapAuthoring.zh-CN.md) 执行，不运行已退役的 Demo 配置脚本。

```powershell
# 沿用自动化命令的 $qaRun，或先创建新的证据目录。
& 'E:/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' `
  'E:/2026ue/DreamOfPadma/DreamOfPadma.uproject' `
  -run=pythonscript -EnablePlugins=PythonScriptPlugin `
  '-script=E:/2026ue/DreamOfPadma/Scripts/Editor/AuthorModelPreview.py' `
  -unattended -nop4 -nosplash -NullRHI -NoSound `
  -DDC-ForceMemoryCache -ddc=NoZenLocalFallback `
  "-abslog=$qaRun/AuthorModelPreview.log"
```

通用模型预览不再依赖 Demo 配置。若脚本意外修改已有资产，停止该测试并记录路径，不覆盖原件来修补测试。

## 8. 历史证据与通过标准

编写方案时读取的历史证据：`C:/Users/lenovo/.codex/visualizations/2026/09/08/01a07f68-77cf-7fe0-8479-93a66fb8a4f6/runtime-review/model-tests-reviewed/index.json` 中六项项目测试成功，用例内警告／错误为零。TASK-040 还记录 `model-build.log`、`model-finalize.log`、`model-render.log`、`model-preview.png`。HTML 更早的三种宽度／明暗检查记录在 TASK-011。这些属于历史 TASK-040/011 结果；当前 TASK-048 构建／22 项测试／200 个 PCG 实例与最终渲染证据另行记录。

现有功能回归通过的条件是：必测冒烟与自动化通过，无阻塞该范围的关键缺陷。依赖素材的用例缺素材时记 Blocked，可选宿主能力记 Not applicable，历史草图差异与当前原生缺陷分别记录。不能据此宣布完整游戏已完成。

严重程度建议：P0 为项目／数据损坏或构建完全不可用；P1 为已有关键流程中断、状态重复或非法输入绕过门禁；P2 为详情错误、模型／技能残留、布局不可用或规则文案不符；P3 为控件仍可用的外观问题。严重程度、复现步骤与功能是否已实现分别记录。

每执行一项，复制以下记录：

| 字段 | 填写内容 |
|---|---|
| 用例／日期／测试人 | MAP01 或 UE08 等；时间 |
| 构建 | 分支 + HEAD + 工作区候选；适用时写 HTML 指纹 |
| 环境 | UE／浏览器版本、地图、视口、主题 |
| 起始数据与步骤 | 测试副本 ID、逐项修改字段和操作 |
| 预期／实际 | 精确现象、ID／数量／提示文字 |
| 结果／严重程度 | Pass、Fail、Blocked、Not applicable；有缺陷时标 P0–P3 |
| 证据／恢复 | 日志摘录或可选图片；重置／恢复结果 |

## 9. 来源索引

- [当前项目状态](../ProjectState.zh-CN.md)
- [UE 模型指南](../Content/ModelAuthoring.zh-CN.md)与 [ACT 配置指南](../Content/ACTAuthoring.zh-CN.md)
- [TASK-008 证据](Tasks/TASK-008-Encounter-Character-and-Skill-Data.zh-CN.md)、[TASK-036 证据](Tasks/TASK-036-ACT-Authoring-Definitions.zh-CN.md)、[TASK-040 证据](Tasks/TASK-040-Model-Assets-and-Scene-Bindings.zh-CN.md)
- [已接受决定](MVPDecisionRegister.zh-CN.md)与[画面设计](../Design/ZH/06_MapAndPreparation.md)
- 源码核对：Core/Run、Core/WorldMap、Game/WorldMap、World/Map、World/PCG、PadmaNodePreview／PresentationCatalog、分离的 Gameplay/Encounter 与 Gameplay/ACT、当前测试及历史 HTML 脚本。本方案不引入新规则。
