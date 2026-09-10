# 数据驱动架构

- 文档 ID：ARCH-DATA-001
- 版本：0.4
- 状态：当前目录与已接受 Demo 目标；目标结构不是已实现 API
- 英文原文：[DataDrivenArchitecture.md](DataDrivenArchitecture.md)
- 负责人：主程 / 架构
- 权威：[ProgramArchitecture](ProgramArchitecture.zh-CN.md)、[ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md)

## 当前实现

`UPadmaContentCatalog` 引用类型化软 DataTable 分片及表现/ACT 资产。Game 校验并解析为 `FPadmaContentSnapshot`，Core 消费值数据。重复身份会拒绝加载。当前入口同步加载；软引用和 UPrimaryDataAsset 继承不证明已有异步 Asset Manager 管线。

现有具体契约：[PadmaContentTypes.h](../../Source/DreamOfPadma/Public/Core/Content/PadmaContentTypes.h)、[PadmaContentCatalog.h](../../Source/DreamOfPadma/Public/Game/Content/PadmaContentCatalog.h)、[PadmaRunRules.h](../../Source/DreamOfPadma/Public/Core/Run/PadmaRunRules.h)。原生目录已有对白/检查点绑定，但尚无下述统一章节运行时、阶段剧情池、教程运行/奖励账本或地图清单。

## 数据分层与字段权威

| 层 | 拥有 | 不拥有 |
|---|---|---|
| 定义 | 规则数值、ID、初始配置、内容引用 | 当前 HP、占领、奖励凭据 |
| 生成结果 | 冻结节点/坐标/地形/边/锚点/锁及地图身份 | 可变占领、进度或活跃 Actor |
| 局内状态 | 当前卡牌/资源/占领、旗标、剧情决定和随机流进度 | 可编辑源图或美术资产 |
| 表现 | 通过稳定绑定选择的图片/模型/材质/动效/Cue | 伤害、奖励或分支结算 |
| 制作工具 | 对上述定义的已校验编辑/导入/预览 | 第二份独立规则副本 |

一个字段只有一个来源。例如卡牌基础属性位于规则定义，表现资产绑定美术但不能覆盖攻击力。生成输出是冻结派生结果，不是独立可编辑源。现有地图检查点表通过显式导入更新模板；仅编辑表不会改变运行中的地图。

## 资产选择

| 载体 | 用途 | 项目约束 |
|---|---|---|
| 类型化 DataTable | 可横向比较的卡牌值、对白结构、池、费用和阈值 | 稳定行/定义 ID；明确单位和模式 |
| DataAsset | 嵌套章节配置、成组表现引用 | 不需要受管发现/加载时优先简单资产 |
| PrimaryDataAsset | 已有目录/ACT/模型定义，或确需 Asset Manager 身份 | 不为命名一致转换现有资产 |
| Curve / CurveTable | 带名称和单位的连续关系 | 不能替代未确认公式 |
| StringTable | TextId 背后的本地化文本 | 已接受的剧情目标；现有直接文本需显式迁移 |
| GameplayTag | 注册分类、权限和查询词汇 | 不存数值，不代表可执行行为 |
| SaveGame | 版本化局内值和兼容元数据 | 不作为内容制作数据库 |

当前合成语义标签是带权 `TMap<FName, int32>`，不是 GameplayTagContainer。换成无权标签会改变规则。固定条件/效果注册表已接受；用 GameplayTag 表示其类型 ID 仅出现在未回答的 Q41，尚未冻结。

## 已接受的章节目标

下列是概念上的最小契约。先用现有类型化资产/表流程实现；最终 C++ 结构与迁移归属具体实现任务。

| 契约 | 最少信息 | 消费者 |
|---|---|---|
| ChapterDefinition | ChapterId、ChapterContentVersion、地图绑定、入口/结尾引用、剧情事件索引、教程步骤、池和表现绑定 | Game 章节协调 |
| StoryEvent / StoryBeat | 稳定 EventId、轨道、触发、类型化条件、选项/效果、后继 ID、重复/冷却策略、CueId | Core/Game 剧情执行 |
| TutorialStep | StepId、要求的玩家操作、完成条件、推进/解锁引用 | 教程运行时；完成必须代表实际操作 |
| 对白行 | DialogueId、说话者、TextId、选项 ID 与对白内部跳转 | 对白运行时/UI |
| StoryPool | PoolId/版本、有效时期/条件、带权 EventId 候选、重复策略和明确选择优先级 | 阶段分配服务 |
| PresentationProfile / Cue | CueId；对白/头像/Sequence/音频/Niagara/动作绑定；输入/跳过策略 | UI/Presentation |
| 条件 / 效果条目 | 注册类型和已校验的类型专用参数 | 权威服务；不任意派发 Blueprint 函数 |

节点类型（普通/篝火/熔炉/锚点）、剧情轨道（主线/支线/NPC/随机）、触发、池策略和表现是独立维度，不能合并为一个互斥枚举。曾建议的“主线 > NPC > 支线 > 随机”不是最终通用规则；内容冲突时需明确配置适用顺序。

执行权威不能重复：对白行拥有对白内部跳转，事件后继拥有事件之间转换；选项结果只跨越该边界一次。LevelSequence 和未来编辑器图不重复实施规则效果。

Q36/Q38 已接受固定注册表与表现分离。Q39–Q45 中参数编码、跨事件原子事务设计、教程结果事件的准确词汇仍属提案。实现应保持已有命令/回滚不变量，不得宣称未回答设计已获批。

## 地图定义目标

| 契约 | 字段 / 职责 |
|---|---|
| MapKey | ChapterId + Seed + GeneratorVersion |
| GeneratorManifest | 不可变版本身份，覆盖地形算法、PCG Graph、主题/生物群系、地图配置与生成结构 |
| 生成输入 | MapKey、固定玩法锚点/显式边/锁、地形设置及独立装饰 Seed |
| 冻结地图结果 | MapKey、稳定节点、坐标、规则所需地形/高程/水域值、边和检查点 |
| 烘焙表现元数据 | 一致的 MapKey 与已 Cook 的地形/主题/装饰资产绑定 |

清单变化产生新 GeneratorVersion，不能原地修改旧清单。对白修改提升 ChapterContentVersion，不改变 MapKey；若同时改变地图配置/锁路则另论。现有整数生成器版本和 `MapId` 不会自动成为新清单或 ChapterId。

生成、烘焙、校验顺序由 [WorldMapAuthoring](../Content/WorldMapAuthoring.zh-CN.md) 拥有。存档字段、兼容与可信查找由 [SaveSchema](SaveSchema.zh-CN.md) 拥有。玩法拓扑保持权威；PCG 不能移动锚点、增加边或消耗玩法随机流。

## 保留的模式与表现契约

- [ADR-0004](../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md)：Encounter 与 ACT 的行、验证、执行及时钟分离。回合制实例、ACT 角色/武器阵容、共享基础技能身份不同。仅限特性的约束不能变成角色禁用。
- [ACTAuthoring](../Content/ACTAuthoring.zh-CN.md)：ACT 角色模型/AnimClass/类型化技能表、武器表现及相同骨架校验。仅配置执行器 ID 不证明已有运行时技能。
- [ModelAuthoring](../Content/ModelAuthoring.zh-CN.md)：每个带领域的表现键只有一个来源；通用模型资产保存视觉引用/变换。移动性元数据属于 Core 值，运行时许可仍需规则验证；视觉就绪与技能详情就绪分开。
- [Combat](../Rules/Combat.zh-CN.md)、[Synthesis](../Rules/Synthesis.zh-CN.md)、[Time](../Rules/Time.zh-CN.md)、[WorldState](../Rules/WorldState.zh-CN.md) 拥有规则语义。架构不再复制公式、用卡许可及完整未决数值清单。

## 加载、校验与制作工具

`作者定义 → 校验 → 类型化快照 → 服务 → 结果/状态 → 表现`

校验唯一 ID、引用、单位/范围、模式/行兼容、曲线定义域、地图连通/锁路和已知效果类型。错误提供资产/字段/原因。缺失最终值表示未配置，不是零。计算预览使用执行端求值器，但不修改状态或消耗随机流；候选抽取和成功判定仍是不同随机步骤。

目标还需校验 MapKey 与烘焙匹配、章节/地图节点引用、阶段池资格、对白/TextId 引用与教程奖励身份。这些是实现验收需要，不代表已存在完整 Cook 门禁。Q45 中未决验证器在对应实现获授权时确定范围。

用户将提供最终卡牌数据。已标识的原型临时数值可供开发，但不能满足最终 Demo 内容验收。内容记录应保留来源/字段/单位/模式/版本；本文不批准新数值默认值。

Slate 剧情制作、UEdGraph 源资产、图编译、文案/本地化工具和 Cue 编辑均 **暂缓**。保留未来 `ChapterSourceAsset → 编译后的 ChapterDefinition` 分离方案，采用受限 DAG、类型化执行连线且 Shipping 不含编辑器对象。现有 Python 制作和地图预览仍可用；通用一键配表工具不是前置条件。
