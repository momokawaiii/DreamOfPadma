# 策略界面与地图表现创作

- 英文主文档：StrategyPresentation.md
- 范围：TASK-050–052 / ADR-0008–0009；原生 UE 可玩地图。

## 绘制教程地图

在 `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld` 开始新局时选择 `tutorial-painted-v1`。固定 2:1 源绘画、深蓝／金色 HUD 与等比缩放的 1672×941 设计画布以用户提供的参考图为依据。生成请求记录在 `Artifacts/TASK-051/art-prompt.txt`；交付图片属于参考引导的重建，不声称逐像素复刻。源美术为 `Content/Padma/UI/SourceArt/T_TutorialMap.png`（1774×887 RGB）。

`SPadmaTutorialMap` 绘制背景与规则覆盖图形。新教程使用生成器 v3，共 55 格，保留七个关键节点位置和带种子荒野；归处直连的薪火台（`fire`）初始属于王庭并有 1 名守卫，方便测试 Encounter。v1 的 162 格和 v2 的 55 格存档保留原拓扑、归属及驻军定义，不自动加入新守卫。宜居性、归属和开放边来自 Core 数据；小地图与侧栏读取相同稳定 ID。详见 [ADR-0009](../Decisions/ADR-0009-Painted-Tutorial-Hex-Map.zh-CN.md)。

单击地块固定右侧详情与动作目标；鼠标经过其他地块只更新地图高亮，不改侧栏目标。换一个点击目标时仅更新详情，保留顶部、目标、手牌、小地图和阶段控件；排队的完整视图仍优先。查看本身不移动、扣费或开战。A／B 已选卡牌继续进入下述确认流程。模态层、长按、捕获和拖动仍阻挡背景输入。

TASK-051 记录了其原生 1920×1080 渲染、悬停／侧栏保留、加号缩放、释放后全览、Escape 菜单阻挡以及时间 0／时间 4 氛围检查。此前检查不代表 TASK-052 的交互和动态修改已经验收；当前构建、测试、截图与用户试玩证据归于 [TASK-052](../Production/Tasks/TASK-052-Tutorial-Interaction-and-Motion-UI.zh-CN.md)。存档兼容性使用的手工引用与当前绘画布局分开保留，因此加载手工地图再返回教程存档时，不会替换可信锚点配置。

使用 `-PadmaLegacyMap` 可进入此前的世界表现。`encounter`、`act`、`roundtrip` 截图场景保留旧地图初始化；继续运行局时使用已保存的布局。下文的地形／PCG 样板仍可在该路径使用。绘画教程的原生运行检查与视觉验收单独记录在 [TASK-051](../Production/Tasks/TASK-051-Painted-Tutorial-and-Hover.zh-CN.md)。

## 教程交互

选择一张 A 手牌后点击地图地块，会以捕获的卡牌和节点 ID 打开部署确认。选择 B 后点击地块，会列出该地所有已部署的 A 实例供选择附着对象。每项显示自己的 Core 可用性校验结果；不符合条件的项禁用。当地没有 A 卡或没有可用 A 时，界面明确说明并允许取消。侧栏部署动作和有效的 A／B 拖放使用同一部署流程。预览在值拷贝上调用既有 Core 部署命令；只有显式确认才对真实运行局执行命令。确认会重新检查阶段、卡牌、节点、附着目标和费用。取消不扣费；之后的悬停不能替换已捕获的目的地。C 部署保留既有命令行为。

教程手牌按五列排列，更多卡牌进入下一行，使用纵向滚动框。手牌需要重建时保留已有滚动偏移；地图选点的仅详情更新不会重建手牌。标题和底栏使用暖象牙纸纹，配合细线边框与稳定图集插画。

WASD 或方向键可平移教程绘画，中键拖动也可平移。初始全览缩放为 1，平移为零。第一次非零平移会把缩放至少提高到 1.12，在保证绘画边缘不越出视口的前提下提供可移动余量。地图全览按钮在释放捕获后恢复缩放 1 和零平移。模态所有权、长按／鼠标捕获以及可编辑文本／数值框焦点会阻挡键盘平移。

绘景导航只有一个输入所有者：CommonUI 页面把导航按下／释放转发给 `SPadmaTutorialMap`，由后者维护按键集合并处理 Home 全览。其原生 Slate 指针处理器消费地图点击、中键拖动与滚轮缩放。切换页面／覆盖层会取消交互、清空导航键并释放地图自身的捕获；窗口失活或遇到冲突的指针捕获也会清空持续导航。控制器不再重复轮询绘景 WASD，避免 Slate 消费释放事件后 PlayerInput 仍残留按下状态。旧三维相机继续由控制器负责。

战争条把既有规则分值表现为两个份额：我方份额为 `clamp(0.5 + 0.5 * War / warGoal, 0, 1)`，王庭份额为其补值。因此中性分值显示为 50:50，原有分值和胜利边界保持不变。`SPadmaWarBalance` 平滑显示份额并绘制中央天平图形；着色器在轻微波动液面下呈现青绿／红色液体，上方保留深蓝空间，并带有细泡沫边和克制的气泡。液面动态覆盖两侧，即使中央图形遮住分界也仍可见。该表现不增加独立的战争资源或模拟。

## 地形样板

在旧表现路径，本局通过 `APadmaWorldMapActor` 投影稳定图节点 ID；共享顶点的连续程序化地表承载缓坡道路和节点地基，独立水面位于外围边缘。这是程序化网格，不是 `ALandscape` 雕刻资产。固定节点位置与允许的图边继续决定玩法，装饰地形不会产生可通行地块。

`/Game/Padma/World/Maps/Presentation` 中的 `DA_FirstRegion_Theme` 控制网格间距、丘陵/山峰起伏、边缘、水体、地貌颜色、装饰数量/缩放/坡度限制和模型绑定。装饰种子独立于玩法随机流，驱动高度噪声与受控 PCG。PCG 排除节点地基、道路、水域和陡坡，再把成簇树石实例放到采样高度上。改变选择会复用地形与装饰。

`/Game/Padma/World/ArtSample` 包含可编辑的模型、材质和地标定义：两种阔叶树、杉树、岩石、城寨、神龛、村落。它们用于确定形态、颜色和比例，尚非最终量产角色/建筑美术。点、过滤和生成器流程可参考 Epic 的 [PCG 概览](https://dev.epicgames.com/documentation/en-us/unreal-engine/procedural-content-generation-overview)。

## 界面所有权与激活

| 层级 | 实现与输入 | 时间和结算 |
|---|---|---|
| 常驻根界面 | `UPadmaGameScreen`，拥有页面栈和覆盖层栈 | 不结算玩法 |
| 玩法/备战页 | 页面栈中的 `UPadmaActivatableLayer`；包含 HUD、地图、手牌与上下文面板 | 日历仅通过命令推进；战斗保持自身时钟 |
| 菜单/图鉴/行记 | 覆盖层栈中的模态可激活项 | 浏览不提交玩法结算 |
| 卡牌/单位详情 | 只读顶层；长按 500 ms，移动取消，释放被消费 | 遭遇战敌方自动行动等待，已提交动作可完成；ACT 计时保持原状 |
| 对话/确认 | 只有顶层选项或显式确认提交命令 | 取消/返回不付费、不合成、不读档、不撤退 |

只有顶层覆盖层接受输入。底层玩法继续可见；返回依次恢复保留的父页面及焦点。玩法页请求 All 输入，覆盖层请求 Menu 输入。项目尚未配置 CommonInput 默认返回动作表，因此由获得焦点的原生激活节点处理 Escape／手柄右侧面键。显示控件、激活控件、允许输入和暂停模拟是独立事项。ACT 的 Tab 保留独立的 1/10 速度技能库策略。战中备战的计时/编辑时机仍为 D27，入口保持禁用。

长按手牌/驻军使用**卡牌实例 ID**；战场单位使用**战斗 ID**；图鉴与 ACT 备战使用各自的**定义 ID**。战斗详情读取即时生命/上限/攻防/归墟/抗衡及行动数据；世界详情读取实例值与静态定义。优先显示配好的模型立绘，否则明确标注卡牌插画。角色等级/经验尚未配置。玩家共享技能与角色自身能力分开展示。未持有 C 定义继续受首次成功合成解锁限制。

主要运行时皮肤使用 UI 域材质实例，呈现平面深蓝墨色、浅旧金和暖象牙纸纹。此前 `/Game/Padma/UI/Textures/T_HUDPanel`、`T_HUDButton` 九宫格资产保留，但不再是运行时面板／按钮的主要皮肤。主要动作按钮分别拥有材质实例，悬停进入／离开绑定 `HoverAmount`，因此一个按钮的扫光不会带动另一个按钮。卡牌按稳定 ID 使用 `T_CardIllustrations` 的 UV 区域。原生 CommonUI 类用 Slate 构建视觉内容；并不意味着每个按钮都是 CommonButtonBase 蓝图，也没有生成 Widget Blueprint 设计器皮肤。

PIE 结束时控制器调用可重复执行的 `ShutdownPresentation`，取消手势、解除命令回调并显式释放手动构造的两个 CommonUI 栈及其 Slate 缓存，然后清空界面引用。栈不是 WidgetTree 根，不能只依赖父类清理。按钮闭包强持有的 MID 使用 transient package 作为 Outer，避免短暂保留的按钮经材质反向保活 Screen／GameInstance；界面自身 UPROPERTY 持有的材质仍随界面清理。

## 原生覆盖层动态

CommonUI 激活和焦点路由独立于入场动画生效。`SPadmaMotionElement` 使用原生 Slate 不透明度和平移，通过三次缓出曲线运动；立绘通过材质遮罩显现。这些不是在 Widget Blueprint 中制作的 UMG 动画轨道。

| 元素 | 开始延迟 | 动态 |
|---|---:|---|
| 标题 | 0.05 秒 | 从左侧进入，默认持续 0.28 秒 |
| 副标题 | 0.09 秒 | 较短的左侧入场，默认持续 0.28 秒 |
| 立绘 | 0.12 秒 | 左侧入场及斜向材质显现，持续 0.42 秒 |
| 信息 | 0.18 秒 | 从右侧进入，默认持续 0.28 秒 |
| 动作 | 0.25 秒 | 纵向列表依次间隔 35 毫秒，第六个延迟档后不再增加；横向行依次间隔 40 毫秒 |

外层覆盖背景在 0.16 秒内淡入。立绘固定采样纹理区域，显现前沿带窄浅金边，没有时间驱动的图像流动。独立的立绘 MID 保存 `PortraitTexture` 和 `UVMinX/Y`、`UVMaxX/Y`。图集单元 `(column, row)` 使用 `((column+0.01)/3, (row+0.01)/3)` 至 `((column+0.99)/3, (row+0.99)/3)`；单独立绘使用 0..1。材质画刷本身使用完整 UV，避免裁切两次。返回可立即恢复保留的父层；视觉延迟不会推迟输入所有权，也不改变 Encounter／ACT 计时。

## 制作动态 UI 资产

关闭编辑器并确认串行 UE 通道空闲后，使用 RHI 执行新创作脚本：

```powershell
$EngineRoot = 'E:/Epic Games/UE_5.8'
& "$EngineRoot/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "$PWD/DreamOfPadma.uproject" -run=pythonscript -EnablePlugins=PythonScriptPlugin "-script=$PWD/Scripts/Editor/AuthorMotionUI.py" -unattended -nop4 -DDC-ForceMemoryCache -ddc=NoZenLocalFallback
```

`Scripts/Editor/AuthorMotionUI.py` 把 `Content/Padma/UI/SourceArt/T_HandParchment.png`（1448×1086）导入为 `/Game/Padma/UI/Textures/T_HandParchment`。内置图像生成提示词记录在 `Artifacts/TASK052/paper-prompt.txt`。源图是干净细腻的象牙纸纹，没有烘焙边框或倒角。

| 运行时材质实例 | 用途／纹理绑定 |
|---|---|
| `/Game/Padma/UI/Materials/MI_UIPaper` | 纸质标题／底栏；`PaperTexture` → `T_HandParchment` |
| `/Game/Padma/UI/Materials/MI_UIInkPanel` | 平面深蓝面板、细纹理与克制金边 |
| `/Game/Padma/UI/Materials/MI_UIGoldAction` | 哑光浅金动作表面；`PaperTexture` → `T_HandParchment`；悬停扫光 |
| `/Game/Padma/UI/Materials/MI_UIWarLiquid` | 青绿／红色对向流体条与细分界 |
| `/Game/Padma/UI/Materials/MI_UIPortraitReveal` | 固定立绘采样与斜向显现；`PortraitTexture` 默认使用 `T_CardIllustrations` |

每个实例在同目录拥有对应父材质，名称将 `MI_` 前缀替换为 `M_`。五个父材质均使用 UI 材质域和半透明不透明度。

| 标量参数 | 默认值 | 用途 |
|---|---:|---|
| `RevealAmount` | 1 | 0 隐藏，1 完整显示；活动立绘动态更新该值 |
| `HoverAmount` | 0 | 悬停响应；主要按钮进入时设 1，离开时设 0 |
| `AnimationStrength` | 1 | 时间驱动效果的强度；0 移除该动态 |
| `FreezeAnimation` | 0 | 1 使用 `PreviewTime` 替代 GPU Time |
| `PreviewTime` | 0 | 固定着色器时间，单位秒 |
| `ScanAmount` | 0.35 | 金色扫光强度，与悬停及动画强度相乘 |
| `Balance` | 0.5 | 战争材质：0 全红，1 全青绿 |
| `AspectRatio` | 12 | 战争条宽高比，由排列后的实际几何提供 |
| `WaveStrength` | 1 | 战争液面／分界波动强度 |
| `UVMinX`、`UVMinY` | 0 | 立绘区域最小值 |
| `UVMaxX`、`UVMaxY` | 1 | 立绘区域最大值 |

前五项参数共享；其余按表中说明分别属于金色、战争或立绘材质。GPU Time 驱动扫光与流体相位；原生表现更新悬停、平滑份额、几何宽高比和立绘显现。`FreezeAnimation=1` 配合 `PreviewTime=0` 或其他固定秒数，可复现着色器截图；这不会冻结原生 Slate 入场或玩法。`-PadmaMapArtTime=<秒数>` 将运行时着色器时间覆盖应用于地图和适用的皮肤 MID，不改保存的资产。

重复执行会校验并保留已有材质图、源纹理与 MI 覆盖值。唯一显式迁移只处理脚本自有标记、预期节点集合中的已知旧版 `M_UIWarLiquid` Custom 代码哈希；通过校验并成功编译后只修改该代码，保留实例参数。未知／自定义材质图会被拒绝；升级失败时恢复内存中的旧代码，不保存失败版本。资产类型错误、缺失绑定或源哈希变化也会明确失败。UE 5.8 设置结果通过读回验证。`Artifacts/TASK-052/motion-ui.json` 包含源尺寸／哈希、创建／保留／迁移资产、着色器哈希、编译错误及实际标量值。ready 报告仅确认资产创作结果；运行时截图和交互验收仍归于 TASK-052。

## 制作绘画教程资产

关闭编辑器并确认 UE 执行通道空闲后，运行 `Scripts/Editor/AuthorTutorialPresentation.py`。使用 RHI 编译 Custom HLSL；此脚本不加 `-NullRHI`：

```powershell
$EngineRoot = 'E:/Epic Games/UE_5.8'
& "$EngineRoot/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "$PWD/DreamOfPadma.uproject" -run=pythonscript -EnablePlugins=PythonScriptPlugin "-script=$PWD/Scripts/Editor/AuthorTutorialPresentation.py" -unattended -nop4 -DDC-ForceMemoryCache -ddc=NoZenLocalFallback
```

脚本把 `/Game/Padma/UI/Textures/T_TutorialMap` 导入为完整分辨率的 sRGB／UserInterface 贴图，创建 UI 域的 `/Game/Padma/UI/Materials/M_TutorialMap`，并绑定 `/Game/Padma/UI/Materials/MI_TutorialMap`。地图贴图使用 MaxTextureSize 0、LODBias 0，不适用下文小型九宫格贴图的尺寸限制。材质以不变的 UV 采样绘画，叠加局部水纹／水流、缓慢漂移的山间／谷地雾气，以及 UV (0.765, 0.705) 附近的暖色熔炉脉冲。效果由 GPU Time 驱动，不逐帧更新 CPU 参数。

| 参数 | 默认值 | 创作用途 |
|---|---:|---|
| `MapTexture` | `T_TutorialMap` | 源绘画纹理对象 |
| `AnimationStrength` | 0.7 | 总体效果强度；0 返回原始绘画 |
| `CloudStrength` | 0.55 | 云层与谷地雾气强度 |
| `FreezeAnimation` | 0 | 设为 1 后使用固定着色器时间 |
| `PreviewTime` | 0 | 冻结动画时采用的固定秒数 |

需要确定性的氛围截图时，在实例上设置 `FreezeAnimation=1`、`PreviewTime=0`；恢复动画时设置 `FreezeAnimation=0`。使用 `AnimationStrength=0` 可检查减少动态／原始美术。这些控制仅改变表现，不暂停玩法。

开发构建也支持 `-PadmaMapArtTime=0`（或其他秒数），只冻结运行时材质实例以便复现截图，不修改保存的材质资产。

重复运行会校验并保留既有资产及实例覆盖。源文件缺失或资产类型错误会明确失败。脚本识别自身 `Padma.Presentation.Generator` 元数据，可修复自己生成的实例中缺失的父材质／纹理绑定；冲突的自定义绑定会被保留并报错。脚本不替换已有绘画，也不重建自定义材质图。有意修改源美术后，应通过 Unreal 重新导入。创建／校验报告为 `Artifacts/TASK-051/tutorial-presentation.json`；任务证据记于 TASK-051。

## 重建旧地形与保留的九宫格资产

先编译 DreamOfPadmaEditor，关闭编辑器，然后逐个执行命令行资产制作。在项目根目录指定引擎路径：

```powershell
$EngineRoot = 'E:/Epic Games/UE_5.8'
& "$EngineRoot/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "$PWD/DreamOfPadma.uproject" -run=pythonscript -EnablePlugins=PythonScriptPlugin "-script=$PWD/Scripts/Editor/AuthorLandscapeSample.py" -unattended -nop4 -NullRHI -DDC-ForceMemoryCache -ddc=NoZenLocalFallback
& "$EngineRoot/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "$PWD/DreamOfPadma.uproject" -run=pythonscript -EnablePlugins=PythonScriptPlugin "-script=$PWD/Scripts/Editor/AuthorStrategyUI.py" -unattended -nop4 -NullRHI -DDC-ForceMemoryCache -ddc=NoZenLocalFallback
```

地形脚本仅创建/更新自身样板资产及既有表现主题，验证模型轴向和材质槽，并写入 `Artifacts/TASK-050/landscape-assets.json`。有意重新生成样板前，应把自定义美术保存在独立包装资产中。UI 导入脚本保留已有像素内容，将面板／按钮 MaxTextureSize 规范为 256／512，保证九宫格边角紧凑；源 PNG 保留原始分辨率。两者在输入缺失或类型不符时明确报错，均不改图结构、已批准规则与存档结构。

运行截图参数：`-PadmaDemoScenario=details` 打开持有的舞女详情；`layers` 打开菜单 → 图鉴 → 舞女定义；仍支持 `home`、`encounter`、`act`、`roundtrip`。`-PadmaPlayableCapture=<绝对路径.png>` 在启动后截取原生 UI 并退出。检查 `[PadmaLayers]` 激活/停用记录和 `[PadmaMapPCG]` 完成日志。验收证据记录在 TASK-050。
