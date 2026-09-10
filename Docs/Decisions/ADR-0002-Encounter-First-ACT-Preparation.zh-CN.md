# ADR-0002：Encounter 优先交付与未来 ACT 边界

- 英文原文（Agent 阅读）：`Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.md`
- 状态：Accepted；GAS 接入触发条件由 ADR-0004 局部取代
- 日期：2026-09-05
- 实现状态：延期；本次仅编写文档
- Owner：Lead Programmer / Architect Agent
- TASK 契约：../Production/Tasks/TASK-004-ACT-Architecture-Decision.zh-CN.md

## 背景

Dream of Padma 同时需要条件驱动的回合制 Encounter 路线和后续 ACT RealTimeAction 路线。外部 Combat 原型展示了 GAS、连招、Montage、Motion Warping 和蓝图技术，但也存在重复技能生命周期、动画 Notify 职责过宽、损坏的预输入路径，以及不属于 Padma 的插件依赖。

项目需要在不让当前实现偏离 Encounter 的前提下，为未来 ACT 建立可持续边界；同时明确项目设置、逐角色目录、逐技能定义、运行时状态、C++、蓝图、GAS 和表现各自的归属。

## 决策

1. Encounter 保持当前实现优先级。本 ADR 和 ACT 开发契约都不授权 ACT 代码、资产、GAS 依赖或配置变更。
2. 未来 ACT 使用 C++ 为主、数据驱动的实现。公共技能生命周期和序列解释属于原生 Gameplay Ability 与 Ability Task；Data-only Ability Blueprint 是可选内容适配层。
3. 每个可操作 ACT 角色可以有一个 `CharacterACTCatalog` Primary Data Asset，引用该角色专属的 Ability、Projectile、Summon、Scope 和 Ammo DataTable。所有实例使用共享强类型 RowStruct。
4. Ability Table 行是名册/装配记录，引用一个聚合完整技能定义的 `AbilityDefinition` Primary Data Asset。复杂技能时序和连招拓扑属于 `ACTSequence` Primary Data Asset。这解决了 ACT 专属的高层资产拆分；精确模式和项目整体导入管线仍保持开放。
5. 稳定逻辑 ID 跨越表格和资产边界；运行时解析器通过当前角色目录解析。任何可变状态都不能进入 DataTable、Data Asset 或 INI。
6. 未来可以使用强类型 `UDeveloperSettings` 和自定义配置区段保存项目级默认值、策略、校验、加载、调试和功能开关。它可以要求某类技能必须提供 Enhanced Input、Cooldown、Montage 或其他强类型内容，但不拥有某个具体技能的绑定、时长、Montage、Sequence、Effect 或视听资产。
7. 按 2026-09-08 的 ADR-0004 更新，Encounter 和 ACT 将通过 TASK-013 使用原生 GAS，模式体系／配表独立。运行依赖在该任务执行前仍未实现。外部玩法插件仍需独立 ADR。
8. 权威规则、命中校验、伤害、消耗、冷却和清理位于 C++ 规则/GAS 路径。动画 Notify 只发出职责单一的时机标记；Gameplay Cue 与 UI 只负责表现。若未来批准联网，预测仅用于可逆响应，权威端必须重新校验玩法结果。
9. 连招拓扑使用显式节点和边，再加 Tag 门控。普通连招默认在一次 Ability 激活中保存运行时节点和缓冲输入状态，而不是每一段都激活一个新 Ability。
10. ACT 的精确玩法语义仍由已接受规则文档及其中开放/延期标记管理。ACT 契约里的架构名称不会暗中决定玩法数值或时序规则。

## 后果

- 固定夹具 TASK-007 至 TASK-010 仍不依赖 GAS；后续 Encounter 战斗消费获准原生 GAS 基础，不要求先建 ACT 玩法框架。
- 未来 ACT Agent 获得明确的所有权、数据、权威、蓝图和插件边界。
- 逐角色内容可以扩展，而不需要一张万能 DataTable 或重复 C++ 类。
- INI 可继续承担策略，强类型资产则是具体技能内容的来源。
- 该架构会增加若干小型强类型资产和校验工作，但让所有权、加载、测试和复用更明确。
- 未来任何依赖、模块边界、网络、持久化或外部插件变更，仍需要有范围的任务和架构评审。

## 未采用的方案

- 用一张巨型 DataTable 保存所有技能、连招、命中、Warp、Projectile、Summon、表现和运行时字段。
- 用一份自定义 INI 数据库保存所有技能的玩法数值和资产路径。
- 以蓝图为主直接移植外部 Combat 项目。
- 默认让普通连招每一段激活一次新的 Gameplay Ability。
- 让动画 Notify 蓝图或 Gameplay Cue 承担权威战斗逻辑。

## 延期决策

第一个 ACT 垂直切片、GAS 所有权细节、网络基线、Tag 分类、精确 RowStruct、阶段/取消/缓冲语义、`Scope` 含义、载荷生命周期，以及策划基线中已有的全部开放项仍未解决。它们需要未来任务或 Owner 显式决策。

## 2026-09-08 澄清

[ADR-0004](ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md) 取代仅因 ACT 才接入 GAS 的条件，并明确角色卡集合分离、ACT 战斗总设置阵容及模式专属效果。其余 ACT 资产架构和 Encounter 优先顺序保持有效。TASK-013／017／030／035 前阅读新 ADR；历史 TASK-004 范围不变。
