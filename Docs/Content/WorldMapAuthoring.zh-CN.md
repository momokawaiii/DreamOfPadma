# 世界地图编辑

- 文档 ID：CONTENT-WORLD-MAP-048
- 版本：0.2
- 状态：当前编辑工具与独立标出的离线 Demo 烘焙目标；用户交互验收待进行
- 英文原文：[WorldMapAuthoring.md](WorldMapAuthoring.md)
- 交付证据：[TASK-048](../Production/Tasks/TASK-048-Anchored-World-Maps.zh-CN.md)

地图 Data Asset 保存图结构与配置值，编辑关卡用于移动节点位置，视觉主题负责展示。已开始的游戏使用自身冻结的地图布局和检查点配置。

## 打开编辑工具

构建当前 `DreamOfPadmaEditor`，然后在 Unreal Editor 的 Python 控制台执行 [AuthorWorldMaps.py](../../Scripts/Editor/AuthorWorldMaps.py)：

```python
import runpy, unreal
runpy.run_path(unreal.Paths.project_dir() + "Scripts/Editor/AuthorWorldMaps.py", run_name="__main__")
```

脚本创建缺少的资产，重复执行时保留现有定义；填入目录资产中尚未配置的地图/主题引用，并添加分支对话表。脚本会打开编辑关卡，在自身检查后清除临时预览；再次点击 **Rebuild Preview** 即可显示。

| 用途 | 资产路径 |
| --- | --- |
| 地图定义 | `/Game/Padma/World/Maps/Definitions/DA_FirstRegion_Map` |
| 编辑关卡 | `/Game/Padma/World/Maps/Editing/L_MapAuthoring` |
| 视觉主题 | `/Game/Padma/World/Maps/Presentation/DA_FirstRegion_Theme` |
| 检查点表 | `/Game/Padma/World/Maps/Story/DT_FirstRegion_Checkpoints` |
| 分支对话表 | `/Game/Padma/World/Maps/Story/DT_FirstRegion_Branches` |
| NPC 模型定义 | `/Game/Padma/World/Maps/Models/DA_MapNPC_story`、`/Game/Padma/World/Maps/Models/DA_MapNPC_forge` |
| 运行时内容目录 | `/Game/Padma/MVP/Playable/Definitions/DA_PlayableCatalog` |
| 可玩关卡 | `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld` |

## 编辑图结构与位置

1. 打开 `DA_FirstRegion_Map`，在 `Template.Nodes`、`Template.Edges` 编辑 ID、地形、归属、事件数据和单向连接。`HomeNode`、`BossNode` 引用两个不同且存在的节点 ID；起点初始归属必须为 `player`。`core`、`rulerCore` 等操作类型与这两个终点引用分别配置。
2. 打开 `L_MapAuthoring`，选中其中的 `PadmaMapAuthoringPreview` 主 Actor。指定 `Map`、`Theme`，关闭 **Edit Generated Layout**，以编辑 `Template`。
3. 点击 **Rebuild Preview**。在 Outliner 选择 `Edit node: <ID>`，在水平面移动黄色手柄，再在主 Actor 点击 **Apply Node Transforms**，查看 `Status`。手柄高度不会写入地图坐标。
4. 显式保存地图 Data Asset，再点 **Rebuild Preview** 显示应用后的值。**Clear Preview** 删除临时手柄与渲染 Actor；已应用并保存的定义保留修改。
5. 设置 `bUseGenerated=false` 后点击 **Validate** 检查创作模板，或点击 **Generate** 从模板重新生成输出。保存前检查 `ValidationError`。

场景手柄只编辑位置。增删节点、修改边连接需在 Data Asset 中操作，然后重建预览。移动手柄不会自动应用修改。编辑器和运行时展示使用相同的坐标换算。

生成时，`FixedNPC` 与 `MainStory` 锚点保留 `Id`、`NPCId`、对话身份和精确坐标。作者编辑 `Template` 可以主动设定新的固定位置。启用 **Edit Generated Layout** 后，应用编辑会对照 `Template` 拒绝移动或更改锚点及起终点。

## 生成并选择实际使用的地图

| 控件 | 效果 |
| --- | --- |
| `Seed` + `GenerationSettings` | 配置可复现的支路数量、长度、偏移幅度及生成节点的地形选项。 |
| **Generate** | 检查模板，成功后替换 `GeneratedLayout` 并设置 `bUseGenerated=true`；失败保留原输出。 |
| **Validate** | 检查当前启用的布局；生成模式还会对照模板检查固定身份与位置。 |
| **Restore Template** | 用 `Template` 覆盖 `GeneratedLayout`，并设置 `bUseGenerated=false`；会替换对生成输出的手工修改。 |
| `bUseGenerated` | 选择资产实际使用的地图；编辑器主 Actor 的编辑开关独立选择正在编辑哪一份布局。 |

工具初始设置为 1–3 条支路，每条一个节点，偏移幅度为地图坐标 `120`。支路与现有地块重叠时，谨慎调整偏移幅度。生成器保留创作节点和边，添加明确的单向路径；同一模板、设置和种子可复现结果。

验证覆盖 ID、有限坐标、单向图环路、结构可达性、检查点引用，以及道路解锁标记是否存在可达的产生来源。种子会改变拓扑，而不只是布景。支路不会自动成为所有剧情或胜利共用的门槛。

可玩界面的 **创建地图新局** 提供按种子生成新局或使用当前配置地图。开始后会替换当前未保存的进度。编辑资产不会改写已开始或已保存的地图；要使用新定义，请开始新局。存档读取及战斗回滚沿用该局冻结的布局。

## 配置检查点分支

`DT_FirstRegion_Checkpoints` 使用 `FPadmaStoryCheckpointRow`：

| 字段 | 含义 |
| --- | --- |
| `Id`、`NodeId` | 唯一检查点 ID 与抵达节点；每个节点只配置一个检查点。 |
| `NPCId` | 要检查的已有锚点 NPC 身份。 |
| `bRequireCompletion` | `true` 检查人物事件已完成；`false` 检查已见过该 NPC。 |
| `MatchedFlag`、`MissedFlag` | 两个不同且非空的结果标记；抵达时只记录其中一个。 |
| `MatchedDialogueId`、`MissedDialogueId` | 可选的分支对话；在目录引用的对话表中配置对应台词。 |

1. 编辑并保存检查点 DataTable，把它加入地图资产的 `CheckpointTables` 数组。
2. 点击 **Import Checkpoint Tables**，显式复制行到 `Template.Checkpoints`，验证后切回创作模板模式。导入失败时模板不变。
3. 如需让生成地图使用新检查点，重新生成并保存地图资产。只修改表格不会更新冻结模板、已有生成输出或现有游戏局。

工具中的示例在抵达 `gate` 时检查 NPC `watcher`：已完成则选择 `watcher-aid` / `gate-met`，否则选择 `watcher-absent` / `gate-missed`。此前绕过岔路或尚未访问 NPC，不会提前结算检查点。

抵达时记录一次分支。对话完成与奖励仍遵循晌午操作规则；取消的对话可在之后符合条件时重开，已记录的分支不会重判。节点对话完成时产生 `CompletionFlag`；未配置显式标记的旧 `story` 操作产生 `story-road`。只有边的 `UnlockFlag` 明确引用时，该标记才成为道路条件。

## 镜头、模型与 PCG

- 在没有 UI 遮挡的地图区域：滚轮缩放，中键拖动平移，WASD/方向键平移，Q/E 旋转，Home 或 **地图全览** 自动取景。悬停、选中和可出发道路高亮分别更新，不重建静态地表。
- `DA_FirstRegion_Theme` 的 `TerrainVisuals` 按精确地形 ID 配置 `Color`、`Height` 及可选的 `GroundMaterial`、`GroundMesh`。工具配置了 `城镇`、`平原`、`林地`、`丘陵`、`山地`。
- `NodeBindings` 按稳定 `NodeId` 指定 `GroundModel`、`BuildingModel`、`NPCModel`。每项引用 `UPadmaModelDefinition`，在其中设置网格与 `ModelTransform`。缺少绑定时使用低多边形占位模型；工具的两个 NPC 模型使用引擎圆柱体。
- `DecorationMeshes`、`DecorationMaterial`、数量和缩放控制实际 PCG Static Mesh Spawner。独立种子流在节点、道路避让区之外生成装饰点，避让距离计入网格尺寸。PCG 不结算移动、不生成游戏规则中的边，也不消耗游戏随机数。
- `[PadmaMapPCG] requested` 与 `completed ... instances=...` 是运行时证据入口。普通悬停/状态刷新时，`[PadmaMap] layout-build=...` 应保持不变；列出这些入口不代表本指南的验证已经通过。

TASK-050 将挤出六边形地块场替换为连续程序化地表、缓坡道路和贴合地表的受控 PCG。项目自有树石、地标及材质样板见[策略表现创作](StrategyPresentation.zh-CN.md)。它不是 ALandscape 雕刻资产；自定义图编辑器、对称地图及竞技公平性保证仍不在交付范围内，生成支路间距和自定义模型比例仍需创作检查。

源码入口：[地图资产](../../Source/DreamOfPadma/Public/Game/WorldMap/PadmaWorldMapAsset.h)、[编辑适配器](../../Source/DreamOfPadma/Public/Game/WorldMap/PadmaMapAuthoringPreview.h)、[生成器](../../Source/DreamOfPadma/Public/Core/WorldMap/PadmaWorldMapDefinition.h)、[视觉主题](../../Source/DreamOfPadma/Public/World/Map/PadmaMapVisualTheme.h)。

## 第零章烘焙目标——当前工具尚未实现

[ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md) 为离线 Demo 选择编辑器生成/烘焙。现有 **Generate** 按钮生成图值，并非完整地形/PCG 烘焙与 Cook 操作。

| 路径 | 当前行为 | 在目标中的作用 |
|---|---|---|
| 默认绘景教程 | 固定绘景、GPU 氛围及版本化六边形值 | 现有原型；视觉替换/映射仍需实现和美术验收 |
| 旧作者/生成式 3D 地图 | 连续程序表面及运行时装饰 PCG | 可复用样板组件；还不是发行烘焙管线 |
| 目标第零章 | 固定 Seed/锚点/边、算法地形、编辑器生成装饰及烘焙结果 | Development 与 Shipping 加载同一冻结内容 |

旧工具可以添加种子玩法支路；该能力不授权随机改变第零章固定教学路线。保留七个已有地标身份（`home/fire/story/plain/gate/forge/boss`）及角色。新地形渲染器中的准确坐标需制作并版本化，不能静默把绘景坐标当世界单位。

### 制作流程

1. 配置章节锚点、显式边、锁、固定 Seed 与地图设置。地形生成前先建立受保护路线走廊。
2. 分阶段构造地形值：水陆轮廓、起伏、气候/地形分类、河流/水域及植被/资源适宜性。全程尊重固定地点与合法路径；这是建议的算法拆分，不要求复制 Civ6 代码或指定某一种噪声。
3. 校验玩法结果：稳定 ID、锚点位置、可用路径、可达性与锁路分区。漂亮地形不能新增通行边或绕过锁。
4. 冻结值布局与 MapKey。验证失败保留此前有效输出，报告违规节点/边/配置。
5. 用独立装饰 Seed/主题运行 PCG：采样已接受地形，排除水域/陡坡和节点/道路禁布区，放置模型/贴花。调整密度/比例不改变玩法状态。
6. 将所需地形/材质/装饰结果烘焙为项目资产，保留匹配 MapKey 元数据和不可变生成清单引用。确保打包游玩无需重跑 PCG 即可加载结果。
7. 验证打包资产收录，在 Development/Shipping 打开同一地图。保存冻结玩法布局/状态，按匹配烘焙元数据恢复。

本文不冻结具体地形表示（程序网格、Landscape、作者地块或其他渲染器）、烘焙 API、模型/材质预算及最终资产。高程、表面视觉和移动分类应一致；装饰不拥有地形规则。

### 版本与迭代约定

- MapKey/清单字段由 [DataDrivenArchitecture](../Architecture/DataDrivenArchitecture.zh-CN.md) 拥有；加载/不匹配行为由 [SaveSchema](../Architecture/SaveSchema.zh-CN.md) 拥有。
- 影响冻结地图输出的输入变化产生新的不可变清单版本。只有 Seed 不够；纯对白修改使用 ChapterContentVersion。
- 源配置与生成/烘焙输出分离。重建预览不覆盖玩家已保存地图，不推进玩法随机流。
- 后续章节仅可通过获批玩法生成器改变非锚点地形/拓扑；PCG 始终只负责装饰。
- 所需证据：相同输入结果确定性、锚点/锁路验证、装饰排除与随机流隔离、烘焙元数据匹配及无运行时生成的包加载。本文不声称已有完整烘焙证据。
