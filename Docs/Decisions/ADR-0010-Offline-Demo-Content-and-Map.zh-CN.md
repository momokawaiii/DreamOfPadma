# ADR-0010：离线 Demo 内容与地图烘焙边界

- 文档 ID：ADR-0010
- 版本：1.0
- 状态：目标已接受；待实现
- 日期：2026-09-10
- 英文原文：[ADR-0010-Offline-Demo-Content-and-Map.md](ADR-0010-Offline-Demo-Content-and-Map.md)
- 决策归属：用户负责范围/语义，架构负责技术映射
- 来源/证据：[TASK-053](../Production/Tasks/TASK-053-Grill-Technical-Docs.zh-CN.md)

## 背景与优先级

当前原生子集、绘景教程、实时 3D/PCG 样板是可用原型。用户最新决定以可安装的离线第零章 Demo 为目标，暂缓自定义剧情编辑器。[ChapterZero](../Rules/ChapterZero.zh-CN.md) 拥有已接受体验。

本决定覆盖旧路线图“完整 MVP 之后才打包”的优先级，不退役当前地图、不修改序列化 ID，也不将原型数值标为生产值。ADR-0004 模式隔离和 ADR-0008 激活边界保持不变；ADR-0007/0009 在实现经评审迁移前仍是当前地图/存档契约。

## 已接受决定

1. 章节内容、地图生成结果和可变局内状态分别拥有。ChapterDefinition 统筹类型化对白/规则表和表现引用。
2. 第零章固定 Seed、锚点与显式边/锁路。在编辑器生成地形及装饰 PCG，验证并烘焙供 Development/Shipping 使用，打包游玩加载结果。Q21 覆盖 Q16 较早的混合运行时生成选项。
3. MapKey 为 ChapterId + Seed + GeneratorVersion；GeneratorVersion 指向不可变组合清单。烘焙元数据和存档都保留 MapKey，内容版本独立。
4. 保存冻结玩法地图值与可变状态。加载不能静默重生成不同布局；PCG 不改变玩法边/锚点/锁，也不消耗玩法随机流。
5. 剧情运行时分离触发/条件/事件/选择/效果/状态，持久化阶段剧情分配与锁定检查点决定，读档不重抽。
6. 教程完成/跳过/重玩采用独立状态和按运行记录的奖励身份；新次重玩可获奖，同次重试不重复。
7. 条件/效果使用固定注册类型。目标对白结构采用 DataTable，文本采用 StringTable；PresentationProfile/Cue 绑定播放，LevelSequence 不结算奖励/旗标/分支。
8. 未来剧情源资产与运行时 ChapterDefinition 分离，受限类型化 DAG 从编辑器数据编译。Slate/UEdGraph 工具、图编译及相关制作 UI 暂缓，不是 Demo 前置。

地图生成与装饰随机独立于玩法；剧情/战斗/合成的详细随机流拆分是实现设计要求，不代表已有随机结构迁移。

## 唯一归属与影响

| 契约 | 主文档 |
|---|---|
| 数据层、字段与唯一数据源 | [DataDrivenArchitecture](../Architecture/DataDrivenArchitecture.zh-CN.md) |
| 剧情/教程执行与模式转换 | [RuntimeFlow](../Architecture/RuntimeFlow.zh-CN.md) |
| MapKey、分配、奖励凭据与迁移 | [SaveSchema](../Architecture/SaveSchema.zh-CN.md) |
| 生成/校验/冻结/烘焙流程 | [WorldMapAuthoring](../Content/WorldMapAuthoring.zh-CN.md) |
| 顺序与双构建验收 | [Roadmap](../Production/Roadmap.zh-CN.md)、[BuildMatrix](../Production/BuildMatrix.zh-CN.md) |

此 ADR 不改变源码、资产或存档格式。后续实现需解决可信清单查找、烘焙资产/Cook 收录、旧档兼容、生产卡牌输入和恢复证据。现有制作按钮尚不实现这条完整管线。

## 保留的未决项

Q39–Q45 没有用户回答：特殊节点派发机制、可复用子图组织、准确参数/Tag 编码与原子效果设计、教程事件词汇、详细卡牌引用制作、Sequence 绑定实现及完整 Shipping 编译门禁仍是提案。此前已接受的运行时/表现隔离继续适用。

当前绘景向目标表现的渲染选择/迁移、具体菜单/结尾内容、最终卡牌/奖励数值、重玩背包归属、失败恢复策略及可测量视觉/性能验收仍需实现或用户内容决定，不能补成新的获批默认值。
