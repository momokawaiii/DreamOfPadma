# MVP 决策登记表

- 英文原文：MVPDecisionRegister.md
- 文档 ID：MVP-DECISIONS-001
- 版本：0.4
- 状态：Active
- Owner：Integration Coordinator
- 任务：[TASK-011](Tasks/TASK-011-Full-MVP-Task-Cluster.zh-CN.md)
- 更新日期：2026-09-10

## 使用方式

本表记录当前决策、所需回答及依赖任务。它不是替用户补全规则的默认值清单。用户回答先写入适用的英文规范来源，再同步中文和受影响任务；Coordinator 更新此表。技术所有者可提出精确结构方案，但涉及玩法含义的选择须由用户确认。

“政策已接受”不表示正式数值、素材路径或结构已决定。D31 最初允许 HTML 使用可修改临时数值；2026-09-09 TASK-046 授权将其扩展到明确标注的 UE Demo 表。正式配置的缺失／冲突值仍逐项确认。未决内容只阻止依赖它的实现，不阻止其他已满足条件的工作。

## 当前 Demo 范围覆盖——2026-09-10

Grill 决策由 [ChapterZero](../Rules/ChapterZero.zh-CN.md) 和 [ADR-0010](../Decisions/ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md) 拥有。下一交付是可安装离线 Demo，Development 与 Shipping 均需测试。D31/TASK-046 临时数值仍只授权原型使用；最终黄金路径要求用户的生产卡牌/规则值。

固定章节地图采用带版本 MapKey 的编辑器生成/烘焙；阶段剧情分配及教程运行/奖励身份为目标契约。Slate 剧情编辑器暂缓。这些范围决定优先于旧文笼统待办/打包要求，不代表全部完整 MVP 决策已解决。Q39–Q45 仍是未回答提案。

## 决策矩阵

| ID | 状态 | 决策所有者 | 实施前涉及任务 | 已确定内容／仍需回答 |
|---|---|---|---|---|
| D01 | 已接受 | User | [013](Tasks/TASK-013-GAS-Infrastructure.zh-CN.md), [017](Tasks/TASK-017-Encounter-GAS-Playable-Loop.zh-CN.md), [020](Tasks/TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md), [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.zh-CN.md), [031](Tasks/TASK-031-ACT-Repository-and-Bullet-Time.zh-CN.md), [035](Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md) | 角色卡分为两套：回合制卡部署在地图，ACT 卡构成战斗总设置阵容。基础非 A 技能卡仍共享身份／槽位，各模式效果分别配置。 |
| D02 | 政策已接受；资源输入待定 | User / Character Presentation | [014](Tasks/TASK-014-Character-Presentation.zh-CN.md), [033](Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md), [035](Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md) | 用户将在开工后导入占位素材。使用前记录精确路径、骨架／动画兼容性与项目包装。正式动画细节和渲染打磨后做。 |
| D03 | 已接受来源政策；缺失／冲突值待定 | User | [015](Tasks/TASK-015-Combat-Calculation-Contracts.zh-CN.md), [017](Tasks/TASK-017-Encounter-GAS-Playable-Loop.zh-CN.md), [019](Tasks/TASK-019-Resource-and-Global-Ledgers.zh-CN.md), [020](Tasks/TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md), [021](Tasks/TASK-021-ABC-Sandbox-Operations.zh-CN.md), [022](Tasks/TASK-022-Synthesis-Resolution.zh-CN.md), [026](Tasks/TASK-026-Ruler-World-AI.zh-CN.md), [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.zh-CN.md), [032](Tasks/TASK-032-Run-Outcomes-and-Restart.zh-CN.md), [033](Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md), [035](Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md) | 2026-09-09：可采用 mycard.pdf 中明确、无歧义且不与后续已接受规则冲突的数值，记录页码／字段／单位／模式。缺失、含糊或冲突值仍逐项确认；不补造 ACT、经营或移动数值。 |
| D04 | 技术契约待定 | Architect; user for gameplay implications | [012](Tasks/TASK-012-Run-State-and-Contracts.zh-CN.md), [013](Tasks/TASK-013-GAS-Infrastructure.zh-CN.md), [028](Tasks/TASK-028-Battle-Transactions.zh-CN.md), [029](Tasks/TASK-029-Safe-Boundary-Save-Load.zh-CN.md), [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.zh-CN.md) | 记录稳定类型域、参与者／版本规则、定义加载、ASC owner／avatar／授予／移除／重绑定、原生依赖和本地权威基线。模式运行句柄不进入 Core／存档 ID。 |
| D05 | 待定 | User / Combat Planner | [016](Tasks/TASK-016-Encounter-Timeline.zh-CN.md), [017](Tasks/TASK-017-Encounter-GAS-Playable-Loop.zh-CN.md) | Encounter 先攻／行动条公式、同值处理、优先级、反应／打断／额外行动／非回合窗口、回合末处理、窗口出牌额度及逻辑状态／冷却时钟。 |
| D06 | 待定 | User / Combat and Numerical Planners | [015](Tasks/TASK-015-Combat-Calculation-Contracts.zh-CN.md), [016](Tasks/TASK-016-Encounter-Timeline.zh-CN.md), [017](Tasks/TASK-017-Encounter-GAS-Playable-Loop.zh-CN.md), [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.zh-CN.md) | 选定伤害／状态子集、属性池及逐模式数值、叠层／消耗／死亡顺序、修正优先级及未决时代倍率含义。不得将 Shape 与 Returning Ruin 等同。 |
| D07 | 待定 | User / System Planner | [018](Tasks/TASK-018-Calendar-and-Phase-Scheduler.zh-CN.md) | 日历零／一起算约定及本切片所需调度效果顺序。黎明／上午／黄昏、七天和七时代已经接受。 |
| D08 | 待定 | User / System and Numerical Planners | [019](Tasks/TASK-019-Resource-and-Global-Ledgers.zh-CN.md), [021](Tasks/TASK-021-ABC-Sandbox-Operations.zh-CN.md), [022](Tasks/TASK-022-Synthesis-Resolution.zh-CN.md) | 初值／上限／债务数值和权限、收益、操作支付时点、循环成本／风险和补偿参数。逐字段／数值确认，不采用原型示例充当默认值。 熵阈值／灾变触发、概率、造成方归因和后果幅度也逐项确认，由 TASK-019 交付值契约。 |
| D09 | 待定 | User / System Planner | [020](Tasks/TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md), [031](Tasks/TASK-031-ACT-Repository-and-Bullet-Time.zh-CN.md) | 手牌／牌堆／生命周期子集、上限／耗尽、弃牌／返回／焚毁／冷却及随机概率；逐模式效果费用／生命周期归属和明确时钟。 |
| D10 | 待定 | User / System Planner | [021](Tasks/TASK-021-ABC-Sandbox-Operations.zh-CN.md) | A 放置、B 附着／直接使用／场时点与费用、C 部署及选定沙盒技能。附着移动遵循已接受 D29。ABC 技能在局部战斗中的合法性仍未决定。 |
| D11 | 待定 | User / System and Numerical Planners | [022](Tasks/TASK-022-Synthesis-Resolution.zh-CN.md), [033](Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md) | 候选过滤、无共同标签情况、Softmax 单位／归一化、公式系数／截断、B 场输入、是否保留 Sigma 及明确无奖励后果。失败永不发奖励。 |
| D12 | 待定 | User / Level and System Planners | [024](Tasks/TASK-024-Sandbox-Graph-and-Movement.zh-CN.md), [025](Tasks/TASK-025-Node-Operations-and-Story.zh-CN.md), [033](Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md) | 首批节点布局／数量／ID、核心节点解释、移动／类别费用、隐藏道路与地形条件、节点操作和故事后果含义。 |
| D13 | 待定 | User / Combat and Numerical Planners | [026](Tasks/TASK-026-Ruler-World-AI.zh-CN.md), [033](Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md) | 巡逻／发现／潜行条件、威胁事件／每日数值、同分优先级及增援／反攻触发与惩罚。 |
| D14 | 待定 | User / Combat and Level Planners | [027](Tasks/TASK-027-Battle-Routing-and-Travel.zh-CN.md) | 双方、地形和卡片效果如何约束路线；哪条合法路线选择 Encounter／ACT 及其故事后果；目标缺失／不合格时的行为。 |
| D15 | 技术与状态策略待定 | Architect / System Planner; user for state meaning | [028](Tasks/TASK-028-Battle-Transactions.zh-CN.md), [029](Tasks/TASK-029-Safe-Boundary-Save-Load.zh-CN.md) | 完整参与者集合、版本／兼容／失败恢复、安全存档边界及状态提交／恢复规则；明确包含 ACT 阵容／装备及任何战斗可修改的库存／武器状态。 |
| D16 | 待定 | User / Combat Planner; Architect for schema | [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.zh-CN.md) | 首个 ACT 动作、目标／命中规则、阶段／提交／取消／打断／缓冲与时钟语义；仅包含选定角色／武器特性。只有选定动作需要时才要求连段或 Motion Warping。 |
| D17 | 待定 | User / Combat and UI Planners | [031](Tasks/TASK-031-ACT-Repository-and-Bullet-Time.zh-CN.md) | Tab 关闭／再进入、选牌与关闭行为、页数／边界、持续时间及效果／冷却时钟。Tab、虚化、1／10 世界时间、输入拒绝、惯性和可见 1–5 槽保持固定。 |
| D18 | 待定 | User / System and Numerical Planners | [032](Tasks/TASK-032-Run-Outcomes-and-Restart.zh-CN.md), [033](Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md) | 明确整局失败、同时结果优先级与判定时点、胜利边界／事件幅度和重开默认状态。局部战败／退出仍回滚；第七天不意味着强制失败。 |
| D19 | 待定 | User / Level and Numerical Planners | [025](Tasks/TASK-025-Node-Operations-and-Story.zh-CN.md), [033](Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md) | 首时代具名卡牌／角色／武器／Boss、节点／故事内容、用户逐项批准的精确数值配置和验收路线；所有未定值遵循 D03。 |
| D20 | 部分已接受；细节待定 | User / System and ACT Planners | [027](Tasks/TASK-027-Battle-Routing-and-Travel.zh-CN.md), [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.zh-CN.md), [035](Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md) | 已接受：战斗总设置选择 ACT 角色／武器，地形可禁用角色卡的某个特性。仍须确认阵容容量、操控／切换、可用性／费用、预设与本局存档范围、地形之外是否关联触发单位、限制作用对象／时点／反馈和结果投射。特性限制不自动等于整个角色禁用。 |
| D21 | 方向已接受；具体布局未决 | User / World / Architect | [024](Tasks/TASK-024-Sandbox-Graph-and-Movement.zh-CN.md), [025](Tasks/TASK-025-Node-Operations-and-Story.zh-CN.md), [028](Tasks/TASK-028-Battle-Transactions.zh-CN.md), [029](Tasks/TASK-029-Safe-Boundary-Save-Load.zh-CN.md) | 斜俯视 SLG；六边形外观上的节点与预设连线。用户接受静态章节定义资产＋.umap 表现＋SaveGame 本局变化（ADR-0005）。具体节点／边／费用仍属 D12／D03。 |
| D22 | 部分已解决 | User / System and Combat Planners | [015](Tasks/TASK-015-Combat-Calculation-Contracts.zh-CN.md), [018](Tasks/TASK-018-Calendar-and-Phase-Scheduler.zh-CN.md), [020](Tasks/TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md), [022](Tasks/TASK-022-Synthesis-Resolution.zh-CN.md), [033](Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md) | 来源核对：用户决定首版不实现 mycard 6–7 契印沙盘召唤旧效果，后续重做。保留后续无奖励失败、阶段和基础战斗卡策略；其他冲突定义使用前解决。D22d 已由用户解决：A→F、B→D、C→R，修订冲突卡集标注，不自行改写数值／效果。见来源提取 D22a–f。 |
| D23 | 数据结构已授权；内容未决 | User / ACT Planner | [030](Tasks/TASK-030-ACT-GAS-Playable-Loop.zh-CN.md), [031](Tasks/TASK-031-ACT-Repository-and-Bullet-Time.zh-CN.md), [033](Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md), [035](Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md) | PDF 中没有独立 ACT 角色／武器／动作卡集或逐模式基础技能效果。用户尚未设计 ACT 内容，授权先做可填写模型和 ACT 技能配表软引用的数据结构。运行仍需用户内容、受支持的 GAS 执行绑定及逐项确认的数值／费用／持续时间／目标，不自动转换 C 人物卡。 |

TASK-034 的 ALL 门槛表示它使用的所有决策必须闭合，或有明确不影响完整 MVP 验收的延期记录；不得把仍需实现的必要功能标为延期来通过验收。

## 推荐提问批次

1. TASK-012／013 前：D04 技术契约；TASK-014 前请用户指出已导入占位素材路径。
2. 首场 Encounter 前：D05／D06，按字段列出需要用户决定的公式、时钟和数值。
3. 日历／资源／卡牌前：D07／D08／D09；避免一次询问整份游戏百科。
4. 沙盒／合成／世界 AI 前：D10–D14 和相关 D19 内容。
5. ACT 阵容与战斗前：D20／D16／D17；角色卡与基础技能卡须在问题中分别称呼。
6. 存档／整局／内容验收前：D15／D18／D19，确认完整状态与验收路线。

补充来源与地图决策见 [MVP 来源提取](MVPSourceExtraction.zh-CN.md) 和 [ADR-0005](../Decisions/ADR-0005-World-Map-Storage.zh-CN.md)。D21 只关闭拓扑方向与存储分层，不关闭移动费用；D22 不授权改写已接受规则。

## 回答记录格式

每次记录：决策 ID、用户原意、日期、受影响字段及具体值／单位、范围（仅测试配置或正式规则）、英文规范路径、中文伴随路径、受影响 TASK、剩余未决项。留空不等于零，不可用不等于免费，未确认不等于默认同意。

## 地图与全局配置决策

- D24 — 已接受（用户，2026-09-09；024／027／037）：点击只查看地块全部信息，不移动／扣费／开战。局部视图右侧独立“移动到此处”按钮提交选中 ABC 等卡牌实例的移动请求。场景可用性仍由配置决定。
- D25 — 已接受首版全公开（用户，2026-09-09；024／026／037）：地块内敌方全部内容公开，包括身份、卡牌、数量、附着与状态。隐藏情报的策略深度后续设计；不改变统治者自身 AI 侦测规则。
- D26 — 已接受（用户，2026-09-09；033／034／038）：FPS 不在首版可玩，ACT 是首版可玩动作模式；Encounter 保留既定 MVP 要求。FPS 配置页只保留未来能力状态，不声称可出战。
- D27 — 开放（用户；035／038／039）：何处／何时开启和修改、持续时间、未保存修改和全局预设／本局归属。浏览不是装备／授予／推进命令，不继承 ACT Tab 子弹时间。
- D28 — 要求关系增益能力，具体规则开放（用户／System／Numerical；025／033／034／039）：具体 NPC／事件／增益、目标（含可能的 B 卡类别）、阈值／幅度、授予／移除／叠加／时机与全局／本局持久化。好感不等于合成相性，增益不全是 ACT 效果。

D24–D26 已于 2026-09-09 回答，画面设计已同步。TASK-037／038／039 的其他前置仍待完成。

- D29 — 移动政策已接受，逐卡配置开放（用户，2026-09-09；012／020／021／024／035／040）：各卡库新增可移动／不可移动／条件移动，以及独立未配置状态。A 与附着 B 成组移动，逐张校验。失败不部分移动／扣费；仍检查归属、起点、连线及已配置费用。具体条件、各卡政策和费用不推定。
- D30 — 模型绑定已授权（用户／Architect，2026-09-09；014／037／040）：需要模型的内容有 Data Asset 软引用来源。场景槽位绑定实例和带领域的定义 ID，Game 解析选定来源及其模型／技能。ACT 保留既有唯一来源。静态预览不是本局占位或存档。

## HTML 优先与交互续作（2026-09-09）

以下更新优先于矩阵中早期的待回答描述，适用范围不自动扩大到 UE 正式实现。

- D31 — 已接受（用户；042，046 修订）：用户已接受 HTML 0.4 子集并授权原生 UE 实现；2026-09-09 明确允许把其临时数值放入清楚标注的 UE Demo 配置，覆盖之前仅限 HTML 的交付边界。D03／D08 逐值确认仍适用于正式配置，见 ADR-0006。
- D32 — 已接受（用户；021／022／042；规范见 Rules/Synthesis 与 Time）：合成尝试成败均弃置参与 A／B；每天进入晌午时 A 倾向累加到附着 B 一次；成功 C 直接在原料当前所在节点生成，不进手牌、不二次部署扣费。
- D33 — 已接受（用户；015／020／027／031／042；见 Rules/Combat）：共享基础卡进入独立弃牌堆，耗尽洗回并保持共享槽位；HTML 可手动选择 Encounter／ACT。C 克制影响基础卡物理伤害，真伤不受其减半影响。
- D34 — 已接受方向（用户；037／038／042）：双击归处进入“节点细节／主角展示／其他模式”编队页；单击查看和原地块场景预览仍独立。坐镇主角、ACT 手下、FPS 手下使用独立名册。主角可预览服装、武器品种、兼容姿势；FPS 只验证选角，仍不可战斗。
- D35 — 已接受入口，正式剧情待定（用户；025／039／042）：熔炉／守望人点击后进入剧情对话。本版用原创占位台词与选项，最终选择才结算已有一次性效果，取消无发放。好感与新奖励未被代定。
- D36 — UE 技术建议，未实施／未接受 ADR：CommonUI 页面栈配可复用 3D 展示舞台；剧情状态与命令独立于 UI／Sequencer。详见 [HTML 指南](../../Prototypes/HTML/README.zh-CN.md)。永久预设归属、真实资产与正式剧情仍待后续决策。
