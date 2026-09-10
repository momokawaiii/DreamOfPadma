# 按职责命名运行时代码

- 英文源文档：[RuntimeNaming.md](RuntimeNaming.md)
- 文档 ID：ARCH-RUNTIME-NAMING
- 版本：0.1
- 状态：当前命名约定；迁移验证记录在 TASK-047
- 交付与证据：[TASK-047](../Production/Tasks/TASK-047-Runtime-Naming.zh-CN.md)
- 相关实现：[原生试玩指南](../Content/NativePlayableDemo.zh-CN.md)、[ADR-0006](../Decisions/ADR-0006-Native-HTML-Slice.zh-CN.md)

## 目的与范围

可复用的运行时代码按职责与领域命名。`Demo`、`Playable`、`MVP` 描述交付或夹具阶段，不能说明卡牌结构、本局服务、控制器或页面的职责。TASK-047 将此约定应用到原先位于 `Source/DreamOfPadma/{Public,Private}/MVP` 的原生运行时源码。

本次只在已接受的 Core/Game/Gameplay/Presentation/World/UI 边界内调整实现命名与文件组织，不新增 UE 模块、不改变依赖方向、玩法或序列化字段名，也不重新定义规则。旧 `Source/DreamOfPadma/{Public,Private}/Demo` 夹具不属于此次迁移。

## 名称说明领域与角色

采用 Unreal 类型前缀，再接 `Padma`、领域与角色，例如 `FPadmaCardDefinitionRow`、`FPadmaCardInstance`、`UPadmaRunSubsystem`。语义属于特定模式时保留模式名：Encounter 与 ACT 仍然独立。文件名不含 Unreal 的 `F/U/A/E` 类型前缀，说明主要类型或一组连贯职责。

| 后缀或角色 | 含义与当前示例 |
| --- | --- |
| `DefinitionRow`／`Definition` | 创作阶段的静态身份／配置。`FPadmaCardDefinitionRow` 定义卡牌；`UPadmaModelDefinition` 描述模型引用。二者都不是本局中可变的卡牌实例。 |
| `ParameterRow`、`EffectRow`、`LineRow` | 当行代表平衡参数、已支持效果或对白行时，采用更具体的静态结构名：`FPadmaBalanceParameterRow`、`FPadmaACTSkillEffectRow`、`FPadmaDialogueLineRow`。 |
| `State`／`Instance` | 可变本局值或单个实体身份：`FPadmaRunState`、`FPadmaWorldNodeState`、`FPadmaCardInstance`。保存稳定 ID，不保存 Actor 指针。 |
| `Snapshot`／`View` | 特定读取边界上的值数据：`FPadmaContentSnapshot` 是已校验内容快照；`FPadmaGameView` 为页面投射状态。这些名称不赋予状态修改权。 |
| `Catalog` | 收集并解析创作条目／引用：`UPadmaContentCatalog`。不拥有当前本局，也不结算命令。 |
| `Rules` | 使用状态与定义校验、执行领域命令：`FPadmaRunRules`。 |
| `Subsystem` | 明确的 Unreal 生命周期服务：`UPadmaRunSubsystem` 拥有 GameInstance 生命周期的本局及持久化入口。 |
| `SaveGame` | 带版本的持久化外壳：`UPadmaRunSaveGame`。与执行存读档的服务分离。 |
| `GameMode`／`PlayerController` | Unreal 框架角色：`APadmaGameMode` 配置控制器；`APadmaPlayerController` 通过已有服务组合游戏、输入与视图。 |
| `Actor`／`Screen` | 场景或 UI 表现：`APadmaWorldNodeActor`、`APadmaWorldMapActor`、`UPadmaGameScreen`。发送意图，读取值／回执；表现不负责玩法结算。 |

已有的 `FPadmaACTSkillRow` 是 ACT 创作绑定行，包含技能 ID、槽位和技能资产引用。`FPadmaACTSkillEffectRow` 则是独立的已支持效果／数值结构。两者都涉及 ACT 技能，并不意味着应合并名称或权责。

## 源码位置与包含路径

模块仍为 `Source/DreamOfPadma`。Public 头文件公开契约，Private 源文件实现契约。存在成对文件时，在 `Public/` 与 `Private/` 下镜像职责路径。包含路径从 Public 根开始，例如 `#include "Game/Framework/PadmaGameMode.h"`；公开包含路径不写入 `Public/`、`Private/` 或交付阶段目录。反射头文件使用与自身文件名匹配的 `.generated.h`。

| 目录 | 职责 |
| --- | --- |
| `Core/Content`、`Core/Run`、`Core/Cards` | 定义行、值快照、可变本局契约与确定性规则；不依赖 Actor、UMG、Niagara、地图或 UGameplayStatics。 |
| `Game/Content`、`Game/Run`、`Game/Save` | 创作目录加载、本局生命周期与持久化外壳。 |
| `Game/Framework` | GameMode 与 PlayerController 组合；控制器的 UI、战斗实现文件仍属于同一个控制器。 |
| `Game/Presentation`、`Presentation/Models` | 预览组合／目录，以及可复用的模型定义／表现。 |
| `Gameplay/Combat`、`Gameplay/Encounter`、`Gameplay/ACT` | 战斗执行／契约与各模式独立的技能和创作配置。 |
| `World/Map`、`World/Combat` | 世界地图／节点 Actor 与纯表现战斗反馈。 |
| `UI/Screens` | 游戏页面、值视图与用户意图。 |

### TASK-047 文件映射

下列路径均相对于 `Source/DreamOfPadma`，逐一列出 39 个迁移文件。机器可读映射见 [RuntimeNaming047.json](../../Scripts/Migrations/RuntimeNaming047.json)。

| 原文件 | 当前文件 |
| --- | --- |
| `Private/MVP/Core/Cards/PadmaCardMobility.cpp` | `Private/Core/Cards/PadmaCardMobility.cpp` |
| `Private/MVP/Core/Content/PadmaDemoContent.cpp` | `Private/Core/Content/PadmaContentTypes.cpp` |
| `Private/MVP/Core/Run/PadmaDemoRunRules.cpp` | `Private/Core/Run/PadmaRunRules.cpp` |
| `Private/MVP/Game/Content/PadmaDemoCatalog.cpp` | `Private/Game/Content/PadmaContentCatalog.cpp` |
| `Private/MVP/Game/Playable/PadmaBattleComposition.cpp` | `Private/Game/Framework/PadmaPlayerControllerCombat.cpp` |
| `Private/MVP/Game/Playable/PadmaPlayableGame.cpp` | `Private/Game/Framework/PadmaPlayerController.cpp` |
| `Private/MVP/Game/Playable/PadmaPlayableInterface.cpp` | `Private/Game/Framework/PadmaPlayerControllerUI.cpp` |
| `Private/MVP/Game/Presentation/PadmaNodePreview.cpp` | `Private/Game/Presentation/PadmaNodePreview.cpp` |
| `Private/MVP/Game/Presentation/PadmaPresentationCatalog.cpp` | `Private/Game/Presentation/PadmaPresentationCatalog.cpp` |
| `Private/MVP/Game/Run/PadmaDemoRunSubsystem.cpp` | `Private/Game/Run/PadmaRunSubsystem.cpp` |
| `Private/MVP/Gameplay/Combat/PadmaCombatAttributes.cpp` | `Private/Gameplay/Combat/PadmaCombatAttributes.cpp` |
| `Private/MVP/Gameplay/Combat/PadmaCombatComponent.cpp` | `Private/Gameplay/Combat/PadmaCombatComponent.cpp` |
| `Private/MVP/Gameplay/Combat/PadmaCombatUnit.cpp` | `Private/Gameplay/Combat/PadmaCombatUnit.cpp` |
| `Private/MVP/Gameplay/Encounter/PadmaEncounterAbility.cpp` | `Private/Gameplay/Encounter/PadmaEncounterAbility.cpp` |
| `Private/MVP/Gameplay/ACT/Authoring/PadmaACTAuthoring.cpp` | `Private/Gameplay/ACT/Authoring/PadmaACTAuthoring.cpp` |
| `Private/MVP/Gameplay/ACT/Runtime/PadmaACTAbility.cpp` | `Private/Gameplay/ACT/Runtime/PadmaACTAbility.cpp` |
| `Private/MVP/Presentation/Models/PadmaModelPresentation.cpp` | `Private/Presentation/Models/PadmaModelPresentation.cpp` |
| `Private/MVP/UI/Playable/PadmaPlayableScreen.cpp` | `Private/UI/Screens/PadmaGameScreen.cpp` |
| `Private/MVP/World/Playable/PadmaCombatFeedback.cpp` | `Private/World/Combat/PadmaCombatFeedback.cpp` |
| `Private/MVP/World/Playable/PadmaPlayableWorld.cpp` | `Private/World/Map/PadmaWorldMapActor.cpp` |
| `Public/MVP/Core/Cards/PadmaCardMobility.h` | `Public/Core/Cards/PadmaCardMobility.h` |
| `Public/MVP/Core/Content/PadmaDemoContent.h` | `Public/Core/Content/PadmaContentTypes.h` |
| `Public/MVP/Core/Run/PadmaDemoRunRules.h` | `Public/Core/Run/PadmaRunRules.h` |
| `Public/MVP/Game/Content/PadmaDemoCatalog.h` | `Public/Game/Content/PadmaContentCatalog.h` |
| `Public/MVP/Game/Playable/PadmaPlayableGame.h` | `Public/Game/Framework/PadmaPlayerController.h` |
| `Public/MVP/Game/Presentation/PadmaNodePreview.h` | `Public/Game/Presentation/PadmaNodePreview.h` |
| `Public/MVP/Game/Presentation/PadmaPresentationCatalog.h` | `Public/Game/Presentation/PadmaPresentationCatalog.h` |
| `Public/MVP/Game/Run/PadmaDemoRunSubsystem.h` | `Public/Game/Run/PadmaRunSubsystem.h` |
| `Public/MVP/Gameplay/Combat/PadmaCombatAttributes.h` | `Public/Gameplay/Combat/PadmaCombatAttributes.h` |
| `Public/MVP/Gameplay/Combat/PadmaCombatComponent.h` | `Public/Gameplay/Combat/PadmaCombatComponent.h` |
| `Public/MVP/Gameplay/Combat/PadmaCombatTypes.h` | `Public/Gameplay/Combat/PadmaCombatTypes.h` |
| `Public/MVP/Gameplay/Combat/PadmaCombatUnit.h` | `Public/Gameplay/Combat/PadmaCombatUnit.h` |
| `Public/MVP/Gameplay/Encounter/PadmaEncounterAbility.h` | `Public/Gameplay/Encounter/PadmaEncounterAbility.h` |
| `Public/MVP/Gameplay/ACT/Authoring/PadmaACTAuthoring.h` | `Public/Gameplay/ACT/Authoring/PadmaACTAuthoring.h` |
| `Public/MVP/Gameplay/ACT/Runtime/PadmaACTAbility.h` | `Public/Gameplay/ACT/Runtime/PadmaACTAbility.h` |
| `Public/MVP/Presentation/Models/PadmaModelPresentation.h` | `Public/Presentation/Models/PadmaModelPresentation.h` |
| `Public/MVP/UI/Playable/PadmaPlayableScreen.h` | `Public/UI/Screens/PadmaGameScreen.h` |
| `Public/MVP/World/Playable/PadmaCombatFeedback.h` | `Public/World/Combat/PadmaCombatFeedback.h` |
| `Public/MVP/World/Playable/PadmaPlayableWorld.h` | `Public/World/Map/PadmaWorldMapActor.h` |

另外新增四个文件，分离框架／存档职责并实现存档兼容：

- `Public/Game/Framework/PadmaGameMode.h`：`APadmaGameMode`，从旧 `Public/MVP/Game/Playable/PadmaPlayableGame.h` 提取。
- `Private/Game/Framework/PadmaGameMode.cpp`：GameMode 配置，从旧 `Private/MVP/Game/Playable/PadmaPlayableGame.cpp` 提取。
- `Public/Game/Save/PadmaRunSaveGame.h`：`UPadmaRunSaveGame`，从旧 `Public/MVP/Game/Run/PadmaDemoRunSubsystem.h` 提取。
- `Private/Game/Save/PadmaRunSaveGame.cpp`：为存档类改名实现当前／旧封装读取。

`PadmaPlayerController.cpp`、`PadmaPlayerControllerUI.cpp`、`PadmaPlayerControllerCombat.cpp` 实现的是 `PadmaPlayerController.h` 中声明的同一个控制器，文件名区分组合、UI 与战斗职责。`PadmaContentTypes.h` 有意汇集相关内容行契约与快照，不将它们误称为同一个可变对象。

## 反射类型迁移

`Config/DefaultEngine.ini` 使用精确 CoreRedirects 覆盖八个类与 31 个结构。所有条目均位于 `/Script/DreamOfPadma`；Unreal 反射名省略 C++ 的 `A/U/F` 前缀。例如首个类条目将 `/Script/DreamOfPadma.PadmaDemoCatalog` 映射到 `/Script/DreamOfPadma.PadmaContentCatalog`。清单保存每个完整路径，不使用子串或整包范围的重定向。

### 类：8 个

| 原 C++ 类型 | 当前 C++ 类型 |
| --- | --- |
| `UPadmaDemoCatalog` | `UPadmaContentCatalog` |
| `UPadmaDemoSaveGame` | `UPadmaRunSaveGame` |
| `UPadmaDemoRunSubsystem` | `UPadmaRunSubsystem` |
| `APadmaPlayableGameMode` | `APadmaGameMode` |
| `APadmaPlayableController` | `APadmaPlayerController` |
| `APadmaPlayableHex` | `APadmaWorldNodeActor` |
| `APadmaPlayableWorld` | `APadmaWorldMapActor` |
| `UPadmaPlayableScreen` | `UPadmaGameScreen` |

### 结构：31 个

| 原 C++ 类型 | 当前 C++ 类型 |
| --- | --- |
| `FPadmaDemoCardRow` | `FPadmaCardDefinitionRow` |
| `FPadmaDemoNodeRow` | `FPadmaWorldNodeDefinitionRow` |
| `FPadmaDemoEdgeRow` | `FPadmaWorldEdgeDefinitionRow` |
| `FPadmaDemoProfileRow` | `FPadmaBalanceParameterRow` |
| `FPadmaDemoACTCharacterRow` | `FPadmaACTCharacterDefinitionRow` |
| `FPadmaDemoACTWeaponRow` | `FPadmaACTWeaponDefinitionRow` |
| `FPadmaDemoDisplayRow` | `FPadmaAppearanceDefinitionRow` |
| `FPadmaDemoFPSCharacterRow` | `FPadmaFPSCharacterDefinitionRow` |
| `FPadmaDemoFPSWeaponRow` | `FPadmaFPSWeaponDefinitionRow` |
| `FPadmaDemoSkillRow` | `FPadmaSkillEffectRowBase` |
| `FPadmaDemoEncounterSkillRow` | `FPadmaEncounterSkillEffectRow` |
| `FPadmaDemoACTSkillRow` | `FPadmaACTSkillEffectRow` |
| `FPadmaDemoDialogueChoice` | `FPadmaDialogueChoice` |
| `FPadmaDemoDialogueRow` | `FPadmaDialogueLineRow` |
| `FPadmaDemoCalendar` | `FPadmaCalendarState` |
| `FPadmaDemoResources` | `FPadmaResourceState` |
| `FPadmaDemoRunCard` | `FPadmaCardInstance` |
| `FPadmaDemoRunNode` | `FPadmaWorldNodeState` |
| `FPadmaDemoMobilityPolicy` | `FPadmaCardMobilityPolicy` |
| `FPadmaDemoPreparation` | `FPadmaPreparationState` |
| `FPadmaDemoSynthesisCandidate` | `FPadmaSynthesisCandidate` |
| `FPadmaDemoSynthesisRecord` | `FPadmaSynthesisRecord` |
| `FPadmaDemoRunLog` | `FPadmaRunLogEntry` |
| `FPadmaDemoBattleRecord` | `FPadmaBattleRecord` |
| `FPadmaDemoRunState` | `FPadmaRunState` |
| `FPadmaDemoModelBinding` | `FPadmaModelBinding` |
| `FPadmaDemoEffectBinding` | `FPadmaEffectBinding` |
| `FPadmaDemoNodePresentationBinding` | `FPadmaNodePresentationBinding` |
| `FPadmaDemoACTCharacterBinding` | `FPadmaACTCharacterBinding` |
| `FPadmaDemoACTWeaponBinding` | `FPadmaACTWeaponBinding` |
| `FPadmaDemoACTSkillBinding` | `FPadmaACTSkillBinding` |

### 非反射契约：9 个

这些 C++ 契约在调用处同步改名，不额外增加反射重定向。

| 原 C++ 类型 | 当前 C++ 类型 |
| --- | --- |
| `FPadmaDemoContent` | `FPadmaContentSnapshot` |
| `FPadmaDemoRunRules` | `FPadmaRunRules` |
| `EPadmaDemoCommandEffect` | `EPadmaRunCommandEffect` |
| `FPadmaDemoCommandResult` | `FPadmaRunCommandResult` |
| `FPadmaDemoMoveCheck` | `FPadmaCardMoveCheck` |
| `FPadmaDemoMovePreview` | `FPadmaCardMovePreview` |
| `FPadmaDemoPendingBattle` | `FPadmaBattleRequest` |
| `FPadmaDemoDialogue` | `FPadmaDialogueState` |
| `FPadmaPlayableView` | `FPadmaGameView` |

## 序列化兼容与夹具名称

已有的 `/Game/Padma/MVP/Playable` 资产包根目录保持不变，包含 `DA_PlayableCatalog` 与两张地图。目录资产现在属于 `UPadmaContentCatalog` 类型，资产包名不等于原生类名。稳定玩法 ID、序列化属性名、存档槽位 `PadmaDemo046`、应用标记 `padma-native-demo` 和存档格式版本 1 均保持不变。

`Scripts/Editor/MigrateRuntimeTypes.py` 校验清单，并通过 Unreal 使用重定向后的类型加载／重存已有创作资产。加载与重存必须保留创作值和资产包位置。执行、旧存档兼容、资产／地图检查与运行时验证统一记录在 TASK-047；重定向条目或本文本身不代表迁移检查已经通过。

`Private/Game/Save/PadmaRunSaveGame.cpp` 另实现存档类名兼容读取。UE 通用 `LoadGameFromMemory` 不会对存储的类名字符串应用类型重定向。本局服务通过这个指定类型的读取入口加载字节：UE 解析 GVAS 文件头，其末尾序列化类名必须精确匹配当前或旧封装，随后用 UE 属性归档读取原始内容。原有应用标记／版本／状态校验继续生效；读取不会重写槽位。

关闭编辑器并重新编译后，在项目根目录使用项目 UE 安装执行资产迁移：

```powershell
& 'E:/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/2026ue/DreamOfPadma/DreamOfPadma.uproject' -unattended -nop4 -nullrhi -ExecutePythonScript='E:/2026ue/DreamOfPadma/Scripts/Editor/MigrateRuntimeTypes.py'
```

脚本先把已有测试资产包备份到被忽略的 `Artifacts/TASK-047/content-before`，校验行数据和地图 GameMode 引用，再原位重存并检查行类型选择器标签，不移动资产包。

交付阶段词仍可用于历史 TASK 描述、本 Demo 试玩指南、夹具内容根目录、`Private/Tests/MVP/TASK-046`、旧 Demo 夹具及其创作脚本；不能重新作为可复用运行时类型、源文件或职责目录的名称。夹具名和需保留的序列化标识是运行时命名约定的明确例外。
