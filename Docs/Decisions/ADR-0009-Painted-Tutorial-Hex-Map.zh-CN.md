# ADR-0009 绘制教程六边形地图

- 英文主文档：ADR-0009-Painted-Tutorial-Hex-Map.md
- 状态：Accepted
- 日期：2026-09-09
- 范围：TASK-051 与 TASK-052；用户批准的固定教程绘画、可占领荒野与单击固定侧栏。

## 决策

教程采用固定绘画，玩法图另行定义。Core 中的 `FPadmaTutorialMapGenerator` 生成纯值布局 `tutorial-painted-v1`，并设置 `bHexWilderness=true`。新生成器 v3 保留 v2 的 55 格网格，并将归处直连的薪火台（fire）初始改为王庭驻守 1 人。已保存的 v1（162 格）及 v2（55 格）保持原驻军和归属；三版均包含七个既有关键节点，保留其 ID、剧情角色与手工定义的有向连接。带种子的荒野生成补充地形与显式相邻连接。这是显式启用的玩法拓扑契约；ADR-0007 的装饰 PCG 对其他布局仍只承担表现职责。

`FPadmaWorldNodeDefinitionRow` 承载 `HexQ`、`HexR`、`bWilderness`、`bHabitable`、`bTraversable` 与 `Elevation`。绘画坐标范围为 0–100，原点位于左上角。生成器为每个关键节点预留一个六边形地块，同时精确保留其显示地标位置：

| 稳定 ID | X | Y |
|---|---:|---:|
| `home` | 10.5 | 48.0 |
| `fire` | 21.4 | 71.8 |
| `story` | 48.8 | 27.4 |
| `plain` | 48.2 | 51.5 |
| `boss` | 85.0 | 15.0 |
| `gate` | 77.5 | 41.5 |
| `forge` | 76.5 | 70.5 |

平原、林地、丘陵、荒原、山地与水域都是显式数据。生成器在同一剧情解锁分区内修复必要的山口／浅滩通道，并把这些通道标记为宜居地面。荒野相邻连接不能跨分区，也不能绕过手工定义的锁定路线。绘画像素、可见道路与 GPU 动画均不授予移动权限。画中的装饰建筑不会新增关键节点。

`FPadmaRunRules::PreviewWildernessOccupation` 检查阵营、地形、归属、已开放有向边以及不存在防守驻军。玩家占领经由已校验的移动及其既有费用；空的中立或王庭荒野可变为我方领地。`AdvanceWildernessFrontier` 在黄昏最多新增一块王庭领地，使用独立确定性决策流，从相邻、空置、中立、宜居荒野中选择。此边疆扩张功能不新增攻击玩家领地、单位或经济模拟。荒野占领不奖励战争天平、核心生命或资源。既有关键节点结算继续保持权威。规范玩法记于 [WorldState](../Rules/WorldState.zh-CN.md) 与 [Time](../Rules/Time.zh-CN.md)。

本局冻结完整 `FPadmaMapLayout`，包含地图 ID／版本／种子、六边形字段、地形与边；运行时归属仍位于 `FPadmaRunState::Nodes`。存读档恢复这些数据，不根据图片或当前种子重新生成。既有布局默认保留 `bHexWilderness=false`。新局由控制器显式选择教程，除非使用 `-PadmaLegacyMap`，或由 `encounter`、`act`、`roundtrip` 截图场景请求旧路径。继续既有运行局时保留其布局。

启动和 `StartTutorialMap` 在生成教程前保留原始手工布局副本。`UPadmaRunSubsystem::Initialize`／`StartNewMap` 将该引用传给 `FPadmaRunRules::Initialize(Content, AuthoredReference)`。即使当前内容已在手工／教程布局间切换，恢复仍使用这个可信配置；教程兼容性由已批准配置推导，不由存档节点定义自身确立。非法锚点或未知地图身份不能替换当前运行局。仅初始化教程而未提供手工引用时，不会自行编造该引用。`AuthoredAndTutorialRestoreOrder` 覆盖两种读取顺序与原子拒绝。

## 网格配置版本（TASK-052）

布局保存 `FPadmaHexGridConfig`：半径、纵向投影、原点、轴坐标边界与水平裁切范围。生成、边线绘制与鼠标命中共享 `CellCenter/CellCorner/ContainsPoint`。v2／v3：半径 10、投影 0.65、原点 (5,4)、Q -5..6、R 0..9、U 3..97。旧 v1：半径 5.7、Q -9..10、R 0..16，其余相同。旧反射存档缺少配置字段时使用 v1 默认值，保留原尺寸。

Map ID 与布局 Version 仍为 tutorial-painted-v1 / 1；GeneratorVersion 选择可信的 55／162 格预设。校验拒绝未知版本或改写的投影字段，并从独立配表参照推导兼容性。v1 签名格式保持不变，v2／v3 将配置纳入签名。单独在编辑器中改半径不代表批准一个新地图版本。固定地标显示坐标仍采用上表，所属轴坐标可随版本不同。首次在全览状态平移时先放大至 1.12 倍，再移动；绘画与命中投影同步，避免露出空白边缘；Home 恢复精确全览。

## 表现与输入边界

`SPadmaTutorialMap` 绘制背景、领地边界、节点和移动预览，并将指针位置转为稳定节点 ID。`UpdateMapHover` 只更新 `HoverNode`，不改 `PreviewNode` 或排队详情刷新。`SelectMapTarget` 在单击时同时更新 `SelectedNode`／`PreviewNode`，`MapDetailNode` 提供固定侧栏目标。经过其他地块不改变该目标；动作携带明确 ID 进入既有命令路径。模态与拖动继续阻挡地图输入，A／B 点击仍进入确认流程。

深蓝／金色 HUD 使用等比缩放的 1672×941 设计画布与 1774×887 源绘画。材质 Time 驱动局部水纹、云雾和熔炉光效，不移动绘画地标 UV，也不需要逐帧 CPU 参数更新。冻结／减少动态控制见 [StrategyPresentation](../Content/StrategyPresentation.zh-CN.md)。这是参考图引导的重建，不声称逐像素复刻。原生渲染／输入已检查；最终氛围对比与用户视觉验收继续记于 TASK-051。

[ADR-0008](ADR-0008-Activatable-Presentation-Layers.zh-CN.md) 继续负责 CommonUI 页面／覆盖层激活、输入路由及详情计时。绘画地图与单击详情不增加新的蓝图激活层级，也不决定尚未解决的 D27 备战计时问题。

## 影响

Core 拥有生成、合法性与已存拓扑；UI 拥有命中测试与绘制；资产拥有绘画与 GPU 氛围。仅显式教程契约支持六边形环路，其他地图继续使用旧地图校验。确定性拓扑、锁定路线保留、占领费用／奖励、存档恢复与单击／悬停输入隔离需要针对性回归检查。固定视觉地理限制了教程对任意生成地图的复用；未来布局需要自己的手工位置与表现契约。
