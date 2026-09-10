# 原生可玩 Demo：试玩、配表与代码说明

- 英文源文档：[NativePlayableDemo.md](NativePlayableDemo.md)
- 文档 ID：CONTENT-NATIVE-PLAYABLE
- 版本：0.4
- 状态：原生实现候选；等待用户试玩
- 交付与证据：[TASK-046](../Production/Tasks/TASK-046-Native-UE-HTML-Slice.zh-CN.md)
- 架构决策：[ADR-0006](../Decisions/ADR-0006-Native-HTML-Slice.zh-CN.md)
- 运行时命名与迁移：[RuntimeNaming](../Architecture/RuntimeNaming.zh-CN.md)、[TASK-047](../Production/Tasks/TASK-047-Runtime-Naming.zh-CN.md)

这是原生 UE C++/GAS、CommonUI 页面与三维 Actor 的实现。规则范围是 HTML 0.4 已支持的子集，数值使用用户授权沿用的 UE Demo 临时配置。卡牌覆盖说明仍会标明尚未实现的 PDF 技能。FPS 仅能配置；尚未迁移 Combat 的完整连招系统。编译、自动化和渲染证据统一记录在 TASK-046；本文不代表用户交互验收或打包验证已经完成。

## 打开并试玩

TASK-049 将真实日历／资源放在顶部，节点与驻军集中到右侧，底部提供插画手牌、全部／A／B／C 筛选、图鉴与合成，右下独立推进时段。再次点已选卡牌可取消；查看目的地会保留来源卡组，合法敌方目的地才显示战斗模式与 **移动并进入战斗**。读档和撤退需要确认；Esc 优先关闭详情／弹窗或取消选目标，再离开页面。弹窗遮挡时阻止底层玩法输入。备战／局部场景内导航保留草稿，返回地图放弃未保存预览；高级移动规则独立放在 **菜单 → 试玩移动规则**，修改立即生效。当前界面证据见 [TASK-049](../Production/Tasks/TASK-049-Native-Strategy-HUD.zh-CN.md)。

打开 `DreamOfPadma.uproject`，在内容浏览器打开 `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld`，点击 Play，会自动开始新局。它现为默认地图。`L_PadmaBattle` 通过沙盘移动产生的待处理战斗进入；单独打开它会返回沙盘。TASK-048 已退役旧 `/Game/Padma/Demo` 链路，新三维地图、锚点、种子、编辑关卡与 PCG 主题见 [地图创作指南](WorldMapAuthoring.zh-CN.md)。

最快进入一场战斗的路径：

1. 点击一次 **推进至晌午**，选择 **归处**，选择手牌 **希律的舞女**，再点击 **部署到此处**。
2. 在右侧驻军栏选择已部署的 C 卡，选择 **薪火台**，点击 **移动到此处**；然后继续移动到 **风蚀原**。
3. 选择 **王庭关隘**，在移动命令旁选择 **Encounter** 或 **ACT**，点击 **移动并进入战斗**。通行依照预设连线；六边形在画面上相邻不代表可通行。
4. Encounter 使用移动组中的 C 卡；ACT 使用 **全局备战 → ACT** 已保存的独立角色与武器。默认 Demo 阵容可直接用占位模型试玩。
5. 战斗结束后点击 **返回沙盘**。胜利提交目标位置与存活状态；失败或确认 **退出战斗并回滚** 恢复移动前的完整状态，包括移动费用。点击 **菜单 → 开始新局** 可重复路线测试另一模式。

| 页面 | 操作 |
| --- | --- |
| 沙盘 | 悬浮显示节点提示，单击打开完整检查栏。右侧驻军栏选择已部署卡牌，底部横向滚动手牌。先选卡，再选三维地块，最后明确点击移动。滚轮／中键拖动／WASD／QE 控制镜头，Home 全览。 |
| A/B 合成 | 部署 A，选中场上的 A，再部署 B 完成附着。选择 A，预览并确认合成。A、B 成败均弃置；成功 C 出现在原节点。A 与附着 B 整组移动，逐张校验移动条件。 |
| 归处 | 双击归处或点击 **进入归处**。**节点细节／主角展示／其他模式** 切换现有页面与展示台；**查看局部场景** 单独展示节点实际内容。 |
| 编队 | 选择主角／服装／武器／姿态，或独立 ACT/FPS 配置，然后明确保存。返回地图会放弃未保存的预览。FPS 没有可执行战斗。 |
| 对话 | 占领相应节点后，**与守望人交谈／使用熔炉补给** 打开对白与选项。完成剧情才发放一次性奖励；中途离开不发奖励。 |
| Encounter | 左侧行动顺序与右侧提示标明当前行动者。先点击普攻，再点击敌方作用单位；多目标攻击等待选满指定数量。**取消释放** 不扣费；防御结束该单位行动。 |
| 共享基础卡 | 从底部拖到合法目标，松手后点击 **确认释放**。也可以点击卡牌或按 1–5 开始选目标。非法拖放／取消不扣费；全体卡选择场地。技能来源为卡牌／场景，不归属当前 C 卡。 |
| 单位详情 | 长按战场单位或左侧对应条目约 0.5 秒。敌我都能查看当前属性、已有说明与可选立绘；等级明确显示未配置。 |
| ACT | WASD／方向键移动，点击场地向该点移动，空格普攻。Tab 开关共享卡库；开启时模拟速度为 1/10，禁止新的移动／普攻输入。 |
| 存读档 | 在战斗／对话外使用 **菜单**。原生槽位为 `PadmaDemo046`，与 HTML 存档独立。**开始新局** 打开种子／资产布局确认，提交时重新加载目录。存档保留自己原有的地图与检查点值。 |

## 配表、Data Asset 与存档分别存什么

| 类型 | 职责 | 示例 |
| --- | --- | --- |
| 有类型的 DataTable | 大量同结构条目：规则、数值、ID、已支持效果的参数 | `DT_Cards_C`、`DT_ACTCharacters`、`DT_FPSWeapons` |
| 含软引用的 Data Asset | 某个身份对应的模型、AnimBP、技能定义／技能表、立绘或表现效果 | `DA_Model_card_dancer`、`DA_ACTCharacter_traveller` |
| `DA_PlayableCatalog` | 收集各领域分表，将稳定 ID 解析为表现／ACT 定义 | `CardTables`、`ACTCharacterDefinitions`、`Models` |
| `.umap` | 场景表现与关卡入口；可选的节点展示场景 | `L_PadmaWorld`、`L_PadmaBattle`、自建节点场景 |
| SaveGame | 本局实例、位置、资源、日历、卡堆、已选配置、标记与随机流 | 实例 ID＋定义 ID＋节点 ID；不存 Actor 或模型指针 |

定义表的 `ModelId` 经 `DA_PlayableCatalog.Models` 找到 `UPadmaModelDefinition`。切换归处坐镇角色时保存的是角色身份；展示场景按身份找到对应资产。场景 Actor 只在其生命周期中持有已加载的表现引用。模型本身不会自动带出技能：ACT 角色定义明确引用 ACT 技能表，Encounter 则使用自己的规则表。

当前生成 **17 张领域表**，另有一张 ACT 技能绑定表，均位于 `/Game/Padma/MVP/Playable/Definitions/Tables`：

| 资产 | C++ 行类型 | 用途 |
| --- | --- | --- |
| `DT_Cards_A`、`DT_Cards_B`、`DT_Cards_C` | `FPadmaCardDefinitionRow` | ABC 数值、A→F/B→D/C→R、移动性、已支持说明及 `ModelId` |
| `DT_Nodes`、`DT_Edges` | `FPadmaWorldNodeDefinitionRow`、`FPadmaWorldEdgeDefinitionRow` | 静态图、坐标、地形、初始归属、预设连线与解锁 |
| `DT_Profiles` | `FPadmaBalanceParameterRow` | 明确标注的临时开局资源、费用、上限及战斗参数 |
| `DT_EncounterSkills`、`DT_ACTSkills` | `FPadmaEncounterSkillEffectRow`、`FPadmaACTSkillEffectRow` | 共享基础卡 ID／槽位，各模式独立效果与费用 |
| `DT_ACTCharacters`、`DT_ACTWeapons` | `FPadmaACTCharacterDefinitionRow`、`FPadmaACTWeaponDefinitionRow` | 独立招募角色、已支持地形特性、武器倍率 |
| `DT_HomeCharacters`、`DT_HomeOutfits`、`DT_HomeWeapons`、`DT_HomePoses` | `FPadmaAppearanceDefinitionRow` | 归处展示身份与武器／姿态兼容性 |
| `DT_FPSCharacters`、`DT_FPSWeapons` | `FPadmaFPSCharacterDefinitionRow`、`FPadmaFPSWeaponDefinitionRow` | 未来 FPS 的选择与展示目录 |
| `DT_Dialogues` | `FPadmaDialogueLineRow` | 说话者、对白、续接、选项、完成／取消标记 |
| `DT_ACTAuthoringBindings` | 已有 `FPadmaACTSkillRow` | ACT 角色技能索引：技能 ID、Slot1–5 绑定、技能 DA 软引用 |

内容增加后，可以新建 **相同行类型** 的表，例如 `DT_Cards_C_Chapter02`，再加入 `DA_PlayableCatalog.CardTables`。仅增加分表不需要新增 C++ 文件。同一领域所有分表中的 ID 必须唯一，`FName` 不区分大小写；重复 ID 会验证失败，不会依表顺序覆盖。行名保持与 ID 一致；新增相应条目时同步更新 `CardOrder`、`NodeOrder` 或 `SkillOrder`。共享技能顺序必须同时匹配两套模式表。已有存档使用的 ID 不应直接改名，需要迁移方案。

这是进入 Demo 时校验并建立的内容快照，并非每帧查表或异步流式加载。新增数值、模型，以及已支持行为的变体可以通过配表完成。新卡牌机制、任意地形条件、ACT 连招或 FPS 开火体系仍需要代码；技能描述文本不会自己变成可执行效果。

## 怎样填入美术素材

手牌使用 `/Game/Padma/UI/Textures/T_CardIllustrations`，源图为 `Content/Padma/UI/SourceArt/T_CardIllustrations.png`。3×3 图集按行依次为 dawn、earth、banquet／cherry、reed、cleopatra／vitruvian、dancer、door。`Scripts/Editor/AuthorStrategyUI.py` 仅在资产不存在时导入并设置 UI 压缩／纹理组，重跑保留已有资产；修改 PNG 后需主动重新导入。新增卡牌身份需在 `PadmaPlayerControllerUI.cpp` 配置卡面索引。此概念图集没有将三维世界模型替换为参考图中的山水画面。

生成目录为 `/Game/Padma/MVP/Playable/Definitions/{Tables,Models,ACT}`、`/Game/Padma/MVP/Playable/Presentation`、`/Game/Padma/MVP/Playable/Maps`；`DA_PlayableCatalog` 直接位于 `Definitions`。第三方原始素材放在 `Content/ThirdParty`，项目包装资产、AnimBP、材质放在 `Content/Padma`。

1. **人物／建筑外观：** 打开 `Definitions/Models` 对应的 `DA_Model_*`，填写 `StaticModel` 或 `SkeletalModel` 其中一种；需要动画时填写骨架兼容的 `AnimationClass`。在展示台调整 `ModelTransform`。战场缩放单独检查：当前战斗表现使用自己的胶囊／模型偏移。材质在模型／材质资产中设置，本版没有通用材质覆盖表。
2. **立绘：** 在 `DA_PlayableCatalog.Models` 找到对应 `Id`，填写 `Portrait`，战斗长按详情会读取。敌方占位身份为 `guard`，C 卡如 `card.dancer`。`Pose` 为展示器可选使用的 AnimSequence。
3. **归处服装／姿态／武器：** `DT_Home*` 行通过 `ModelId` 选择资产。配置骨骼服装后会替换展示身体，尚不是模块化衣物／布料模拟系统。姿态绑定的 `Pose` 要使用相同骨架。武器目前在人物旁展示，展示台不会自动把武器贴合到手部姿态。
4. **ACT 身体：** 打开 `Definitions/ACT/DA_ACTCharacter_<id>`，填入 `Model`、`AnimationClass` 与 `SkillTable`。这些引用是 ACT 运行时的权威配置；`ModelId` 绑定保留为后备模型／表现元数据入口。生成的技能表指向五个已支持的原生 Demo 绑定。混用骨架前先重定向动画；使用 Montage 时需要在 AnimBP 中提供 Montage Slot。
5. **ACT 武器：** 打开 `DA_ACTWeapon_<id>`，在 `StaticModel`／`SkeletalModel` 中选一种，`AttachmentSocket` 填角色骨架上实际存在的 Socket；运行时武器挂在这里，并使用该行 `ModelId` 对应模型资产的 `ModelTransform` 作为相对挂点的位移／旋转／缩放。若希望独立编队预览也显示相同武器，还需替换该模型资产。伤害／距离／间隔倍率在 `DT_ACTWeapons` 调整。
6. **特效：** 在 `DA_PlayableCatalog.Effects` 的 `cut`、`rain`、`attack` 等条目填写 `Niagara`／`LevelSequence`。战斗结果回执在作用单位处生成 Niagara 并播放已配置的 Sequence，同时保留可读的轨迹／落雨与数值反馈。C 卡的 `vitruvian`、`dancer`、`door` 绑定可填写普攻 `Montage`；ACT 技能 DA 则有自己的可选 `Montage`。这里负责表现播放，不自动完成 Sequencer 来源／目标绑定、命中时机或伤害逻辑。`Icon` 已有配表字段，但当前原生页面尚未读取。
7. **节点场景：** 在 `NodePresentation` 找到稳定节点 ID，填写 `GroundModelId`、`BuildingModelId`，可选填 `Scene`。后者作为关卡实例加载到展示台位置，场景内容应围绕自身局部原点搭建。规则与已部署卡牌仍来自本局状态，静态场景不要重复承担游戏结算。节点 `EffectId` 当前预留，不会自动启动节点特效。
8. **验证：** 保存资产，停止 PIE 再 Play，查看对应节点／归处页面后进入战斗。用真实资产检查骨架／姿态兼容、挂点、缩放和性能。保留 ID 与目录的来源／版本标记。修改现有 UE 资产不需要重新导出 HTML。

第一批实用素材可以是：一个归处身体与展示姿态、一个 ACT 身体与 AnimBP、一把武器及挂点、能区分的 C 卡／守卫模型、小型地表建筑套件、五种清晰的技能特效、人物立绘。对话当前仅显示文本与选项；语音、对话立绘和演出绑定需要后续表现接入。

首次启动不要求手写玩法蓝图。当前蓝图杂项主要是 AnimBP、Montage Slot、兼容姿态／挂点，以及材质特效资产。页面继承 CommonUI，但布局使用原生 Slate；改成设计器制作的 Widget Blueprint 仍需要明确接入视图绑定，当前控制器没有可直接替换的 WBP 皮肤属性。替换页面后，规则仍应通过命令执行。

## 代码文件与层级

下表代码路径相对于 `Source/DreamOfPadma`。成对文件的 `.h` 在 `Public/`，`.cpp` 在 `Private/`，再接表中职责路径；只有实现文件的条目明确包含 `Private/` 前缀。Core、Game、Gameplay、Presentation、World、UI 是既有 `DreamOfPadma` 模块内的逻辑目录。运行时源码不再以 `MVP/` 或 `Playable/` 作为上层目录。

| 文件 | 功能 |
| --- | --- |
| `Core/Cards/PadmaCardMobility.h`／`.cpp` | 卡牌移动性定义与确定性移动资格判断。 |
| `Core/Content/PadmaContentTypes.h`／`.cpp` | 反射定义／效果行及 `FPadmaContentSnapshot`；验证字段、引用、顺序并查询参数。 |
| `Core/Run/PadmaRunRules.h`／`.cpp` | 可存档状态、命令／预览／结果契约；日历资源、卡堆、部署、整组移动、合成、对话编队、种子随机流与战斗提交／回滚。 |
| `Game/Content/PadmaContentCatalog.h`／`.cpp` | `UPadmaContentCatalog` 的有类型软引用表数组与美术／ACT 绑定；加载合并分表、验证内容快照和解析资产。 |
| `Game/Run/PadmaRunSubsystem.h`／`.cpp` | `UPadmaRunSubsystem` 拥有 GameInstance 生命周期的本局服务：新局、安全存读档、版本与状态校验。 |
| `Game/Save/PadmaRunSaveGame.h` / `.cpp` | 独立的 `UPadmaRunSaveGame` 外壳及精确匹配当前／旧类名的读取入口，保留格式／应用标识及本局持久化值。 |
| `Game/Framework/PadmaGameMode.h`／`.cpp` | 独立的 `APadmaGameMode` 声明与控制器类配置。 |
| `Game/Framework/PadmaPlayerController.h`／`.cpp` | `APadmaPlayerController` 声明、临时选择／页面状态、启动、地图／镜头／展示台组合、输入、切关与失败恢复。 |
| `Private/Game/Framework/PadmaPlayerControllerUI.cpp` | 根据值构造页面，将按钮／拖曳／长按意图转为规则或战斗命令。 |
| `Private/Game/Framework/PadmaPlayerControllerCombat.cpp` | 组合模式参战单位与定义，接通扣费／结算、目标选择和表现回执。 |
| `Game/Presentation/PadmaPresentationCatalog.h`／`.cpp` | 为已创作的预览内容解析表现身份及模型／角色／武器引用。 |
| `Game/Presentation/PadmaNodePreview.h`／`.cpp` | 节点场景定义、预览绑定、预览 Actor 与模型预览 GameMode。 |
| `Gameplay/Combat/PadmaCombatTypes.h` | 共用战斗设置、单位规格／快照、来源／目标回执；无 `.cpp`。 |
| `Gameplay/Combat/PadmaCombatAttributes.h`／`.cpp` | GAS 生命／最大生命、归墟、抗衡属性及属性变化约束。 |
| `Gameplay/Combat/PadmaCombatUnit.h`／`.cpp` | 临时战斗 Character／ASC、属性效果、表现与已授予技能清理。 |
| `Gameplay/Combat/PadmaCombatComponent.h`／`.cpp` | 战斗接口、Encounter 时间线、ACT 移动／攻击、基础卡来源、伤害、回执与生命周期。 |
| `Gameplay/Encounter/PadmaEncounterAbility.h`／`.cpp` | Encounter 独立 GameplayAbility 入口与激活限制。 |
| `Gameplay/ACT/Runtime/PadmaACTAbility.h`／`.cpp` | ACT 独立 GameplayAbility 入口与激活限制。 |
| `Gameplay/ACT/Authoring/PadmaACTAuthoring.h`／`.cpp` | ACT 角色、武器、技能创作资产，以及独立的 `FPadmaACTSkillRow` 绑定结构。 |
| `Presentation/Models/PadmaModelPresentation.h`／`.cpp` | 模型定义、加载与场景表现；模型资产不构成第二份规则权威。 |
| `World/Map/PadmaWorldMapActor.h`／`.cpp` | `APadmaWorldMapActor` 与 `APadmaWorldNodeActor`：六边形／图表现、几何、标签和稳定节点点击身份。 |
| `World/Combat/PadmaCombatFeedback.h`／`.cpp` | 来源到目标的轨迹、落点与飘字，不负责规则结算。 |
| `UI/Screens/PadmaGameScreen.h`／`.cpp` | `UPadmaGameScreen` 读取 `FPadmaGameView` 并发送意图；Slate 面板、弹窗／立绘、长按、拖曳、模糊，不直接修改本局状态。 |

Core 不依赖 Actor、地图、UI 与特效，事务规则因此可以脱离渲染场景测试。Game 负责组合与生命周期；Gameplay 执行临时 GAS 战斗；Presentation、World、UI 展示状态并发送意图。Encounter 与 ACT 共享中立战斗词汇，同时保留独立能力类和行类型。已接受的边界保持不变；目录落地决策记录在 [ADR-0006](../Decisions/ADR-0006-Native-HTML-Slice.zh-CN.md)。

以下配套文件路径相对于项目根目录，源码路径明确包含模块目录：

| 路径 | 功能 |
| --- | --- |
| `Source/DreamOfPadma/Private/Tests/MVP/TASK-046/DemoCatalogTest.cpp` | 导出夹具、有类型分表、重复拒绝、目录原子发布。 |
| `Source/DreamOfPadma/Private/Tests/MVP/TASK-046/RunRulesTest.cpp` | 规则事务、HTML 合成夹具、日历、移动、对话、非法存档拒绝与回滚。 |
| `Source/DreamOfPadma/Private/Tests/MVP/TASK-046/GASCombatTest.cpp` | 原生模式／来源／生命周期行为。 |
| `Source/DreamOfPadma/DreamOfPadma.Build.cs` | 内置 GAS、CommonUI/UMG/Slate、程序化几何与表现依赖。 |
| `DreamOfPadma.uproject` | 启用对应内置插件。 |
| `Config/DefaultEngine.ini` | CommonUI 视口类型与 TASK-047 精确 CoreRedirects；保留原有默认地图。 |
| `Config/DefaultGame.ini` | 现有内容根目录的 Cook 覆盖与限定 GameplayCue 路径；Cook 配置不等于已经通过打包。 |
| `Scripts/Editor/ExportPlayableData.cjs` | 将已接受的 HTML 夹具导出为本地 JSON，供首次资产生成。 |
| `Scripts/Editor/AuthorPlayableContent.py` | 创建有类型配表、模型／ACT 定义和目录；保留已有内容，仅补充缺少的生成守卫／攻击表现入口。 |
| `Scripts/Editor/AuthorPlayableDemo.py` | 调用内容生成，创建表现材质与两张配置原生 GameMode 的地图；已有地图需符合预期 GameMode，且会被保留。 |
| `Scripts/Migrations/RuntimeNaming047.json` | 源文件／类型的精确迁移映射及兼容标识。 |
| `Scripts/Editor/MigrateRuntimeTypes.py` | 校验迁移清单，通过重定向后的运行时类型加载／重存已有创作资产；执行与证据见 TASK-047。 |

## 运行时命名与既有资产

当前目录类是 `UPadmaContentCatalog`；`DA_PlayableCatalog` 仍是既有资产的包名称。`/Game/Padma/MVP/Playable` 资产包、存档槽位 `PadmaDemo046`、应用标记 `padma-native-demo` 及存档格式版本 1 保持兼容。它们是序列化身份，并非运行时类名。CoreRedirects 精确地将八个旧类与 31 个旧结构映射到当前类型。已有资产通过 Unreal 加载／重存完成迁移，不移动资产包，也不改变玩法规则。

[运行时命名规范](../Architecture/RuntimeNaming.zh-CN.md) 定义按职责命名的约定与新旧映射。Demo/MVP/Playable 标签仍可用于夹具内容、测试路径和本试玩指南的交付描述。迁移验证统一记录在 [TASK-047](../Production/Tasks/TASK-047-Runtime-Naming.zh-CN.md)；下方创作命令用于补建缺失的 Demo 资产，不能证明迁移验收通过。

## 重新生成缺失的 Demo 资产

已生成资产可以直接编辑。脚本用于 **创建／保留**，不是覆盖刷新：重新导出 HTML 不会覆盖已有表或模型。更新表时，应明确编辑／重导入该表并验证 ID 与引用。

编译／命令行编辑器运行前先关闭 Editor。在项目根目录 PowerShell 中填写本机 UE 5.8 路径与 Node 可执行文件：

```powershell
$EngineRoot = 'E:/Epic Games/UE_5.8' # 替换为本机安装的引擎根目录。
$ProjectRoot = (Get-Location).Path
$ProjectFile = Join-Path $ProjectRoot 'DreamOfPadma.uproject'
$NodeExe = (Get-Command node -ErrorAction Stop).Source # 也可直接填写绝对路径。
& (Join-Path $EngineRoot 'Engine/Build/BatchFiles/Build.bat') DreamOfPadmaEditor Win64 Development $ProjectFile -WaitMutex -NoHotReloadFromIDE
& $NodeExe 'Scripts/Editor/ExportPlayableData.cjs'
& (Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe') $ProjectFile -unattended -nop4 -nosplash -nullrhi -nosound '-EnablePlugins=PythonScriptPlugin,EditorScriptingUtilities' "-ExecutePythonScript=$ProjectRoot/Scripts/Editor/AuthorPlayableDemo.py"
```

只有 Editor 编译成功后才运行资产生成。检查日志中的 `[TASK-046]` 目录验证／地图就绪信息，并确认没有 Python 错误；仅进程退出码不能证明 Python 脚本成功。脚本不修改 `Content/ThirdParty`，不复制 Combat 插件，也不覆盖项目启动地图。
