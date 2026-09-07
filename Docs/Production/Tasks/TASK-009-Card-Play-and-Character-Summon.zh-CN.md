# TASK-009 Demo 出牌、角色召唤与技能表现

- 英文原文（Agent 阅读）：`TASK-009-Card-Play-and-Character-Summon.md`
- 文档 ID：`TASK-009`
- 版本：`0.1`
- 状态：`Review`
- 上级里程碑或集成 Goal：`TASK-006 固定可玩原型垂直切片`
- Primary Role：`UI and Input Module Agent`
- Primary Agent：仅在 TASK-008 获批并集成后分配 `module_worker`
- 分支/worktree：Local 分支 `feature/TASK-009-card-summon-ui`；默认不使用独立 Worktree
- 任务模式：有边界的 Demo UI/输入和卡牌指令集成；不实现 ACT 或完整卡牌系统

## Goal

完成原型功能路径中可见的卡牌和技能部分。在 Demo Encounter 场景中，卡牌面板显示一张已配置的角色卡；选择它后发送已批准的战斗卡牌指令，成功结果让 TASK-008 的召唤边界生成一个角色；选择该角色后，在只读面板显示其配置的技能列表。

这张卡是符合局部 Encounter 行动状态的固定基础非 A playtest fixture。出牌后可以让它在本次会话的表现上不可用，但本任务不定义正式的弃牌、回牌、冷却、抽牌或消耗规则。

## 为什么现在做

TASK-008 提供局部运行时、角色定义、技能查询和召唤边界。现在需要加入用户可见的交互，让路径具有可玩感，同时保持 UI 只是指令发送者和视图消费者，而不是第二套玩法实现。

## 首先阅读

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Production/Tasks/TASK-006-Playable-Prototype-Cluster.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.md`
- `Docs/Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Content/AGENTS.md`
- `Source/AGENTS.md`

## 决策状态

### 已冻结输入

- 卡牌是局部 Encounter fixture 中的一张固定基础非 A 卡，只在明确符合条件的 Demo 状态中使用。
- UI 输入映射到强类型指令路径；权威服务负责校验和改状态，然后发出结果/事件或只读视图。
- 卡牌引用稳定的 Demo 角色身份；技能面板读取 TASK-008 暴露的角色/技能定义。
- 卡牌和技能面板是 `Content/Padma/Demo/` 下的项目自有 Demo 表现资源。
- 如果本任务需要 UMG，可以在当前临时模块中加入最低限度的 UMG 依赖，并且只有指定写入者可以修改中心 Build.cs。

### 延期或开放输入

- 卡牌生命周期概率、牌库/抽牌规则、资源/消耗策略、合成、随机选择、奖励/失败和持久化。
- Encounter 行动条的精确优先级，以及未决的反应/打断/额外行动窗口出牌额度。
- ACT `Tab` 仓库、五卡槽分页、子弹时间、总页数、ABC 卡局部战斗合法性、GAS 和技能执行语义。
- 最终卡牌/角色模式、正式 ID 命名空间、导入管线、本地化、资源迁移和 Debug 面板数据契约。

如果 UI 必须决定其中内容才能实现，必须保持 fixture 行为显式并返回该决定；不得把正式默认值藏进 Widget 或蓝图。

## 范围

### 允许路径和写入集合

Primary Agent 只能修改：

- `Source/DreamOfPadma/DreamOfPadma.Build.cs`：只添加本任务需要的最低限度 UMG 依赖，不得添加无关依赖
- `Source/DreamOfPadma/Public/Demo/UI/`
- `Source/DreamOfPadma/Private/Demo/UI/`
- `Content/Padma/Demo/Definitions/Cards/`
- `Content/Padma/Demo/UI/Card/`
- `Content/Padma/Demo/UI/Skill/`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-009-Card-Play-and-Character-Summon.md`
- `Docs/Production/Tasks/TASK-009-Card-Play-and-Character-Summon.zh-CN.md`

本任务可以消费 TASK-007/008 的公开接口，但不能修改其源代码、地图、定义、会话路径、`Config/DefaultEngine.ini`、共享 ProjectState/index 或最终集成任务。

### 不在范围内

- 完整卡牌仓库、手牌/牌库/抽牌/弃牌/回牌/冷却生命周期、资源支付、合成、随机、存档或网络行为。
- 技能激活、目标选择、动画、伤害、状态、战斗、Encounter 行动条、ACT、GAS、Debug 面板或外部插件。
- Widget 直接修改玩法状态，或卡牌按钮直接调用 `SpawnActor`。
- 大范围 UMG/Slate 重构、本地化系统或整体迁移 `E:\2026ue\padma`。

## 依赖与集成顺序

- 必须先完成：TASK-006/ADR-0003 获批；TASK-007 和 TASK-008 已实现、Review 并集成。
- 本任务消费 TASK-008 的召唤/查询边界，不得增加第二套生成实现。
- `TASK-010` 负责默认地图、完整路径和手工 PIE 证据。
- Build.cs 修改是中心串行写入。如果需要更大的模块/依赖变化，必须停止并请求架构任务/ADR。
- TASK-005 保持独立；本 Demo 卡没有随机抽牌或随机结果。

### 共享冲突

UMG 依赖修改、`Content/Padma/Demo/UI/Card/`、`Content/Padma/Demo/UI/Skill/` 以及 Demo 卡牌定义资源在 Review 完成前只归本任务。UE build/Editor/PIE 通道保持串行。

## 委派计划

| Role | 工作包 | 权限 | 预期输出 |
|---|---|---|---|
| `architect` | 检查指令/视图边界和最低限度 UMG 依赖是否符合当前临时模块架构。 | 只读 | 边界和依赖问题。 |
| `reviewer` | 检查 UI diff、指令分发、失败结果、资源引用以及没有隐藏卡牌/技能规则。 | 只读 | P0-P3 问题和验收状态。 |

不授权第二位写入者修改 Build.cs 或 Demo UI 资源。

## 验收标准

- [ ] 局部 Demo Encounter 显示一张已配置的卡牌。
- [ ] 点击卡牌发送已批准的强类型战斗卡牌请求；Widget 不直接修改状态或生成 Actor。
- [ ] 成功的指令结果/事件通过 TASK-008 的召唤边界生成且仅生成一个 Demo 角色，并可见地更新卡牌本次会话状态。
- [ ] 选择被召唤的角色后，打开只读技能面板，显示 TASK-008 定义/查询中的技能身份、文本和图标数据。
- [ ] 无效、重复或不可用的卡牌请求显示权威失败，且不生成重复角色。
- [ ] UI 在 PIE 的已批准鼠标/键盘输入路径下工作，不实现 ACT `Tab`/分页，也不实现 Debug 面板。
- [ ] 只加入最低限度的 UMG Build.cs 依赖，UI 模块 README 配对记录所有权和指令/视图边界。
- [ ] 不发明卡牌生命周期、资源、随机、战斗或任何 Open/Deferred 规则。
- [ ] 独立 Reviewer 返回 `Pass`，或只留下集成前明确接受的 P3 改进。

## 验证方案

### 自动检查

- 使用最低限度 UMG 依赖编译 UE Editor Target。
- 如果指令分发可以脱离 UMG 测试，运行一条窄范围有效/无效卡牌请求检查；第一切片不引入 UI 自动化。
- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1`。
- `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict`。
- `git diff --check` 和精确路径/依赖审计。

### 手工检查

1. 通过 TASK-007 路径进入 Demo Encounter 场景。
2. 观察卡牌面板中的一张卡。
3. 点击卡牌，观察请求/结果路径和一个被召唤的角色。
4. 再次尝试出同一张卡，观察可读失败且没有重复角色。
5. 选择被召唤角色，观察技能面板中的配置技能列表。
6. 确认没有激活技能，也没有推断出隐藏的资源/卡牌生命周期规则。

### 必需证据

- 编译输出和窄范围指令检查输出（如有）。
- 展示卡牌面板、召唤结果、重复出牌失败和技能面板的 PIE 截图或短视频。
- 变更路径和 Build.cs diff 审计。
- 独立 Review 报告。

## 学习契约

| 目标 | 所需证据 | 状态 |
|---|---|---|
| UI 是指令发送者和视图 | 用户指出卡牌事件、权威请求、结果/事件和视图更新在出牌路径中的位置。 | `Not started` |
| 数据驱动的卡牌-角色-技能映射 | 用户追踪三个稳定身份，并预测哪些配置数据会改变卡牌/技能显示。 | `Not started` |

- 学习者亲手练习：修改 Demo 技能定义中的文本，运行 PIE，并解释为什么 Widget 会变化而不需要修改 Widget 逻辑。
- 迁移问题：如果卡牌未来增加消耗或冷却，哪个服务负责校验，UI 应展示哪个结果/事件？

## 风险与恢复

- 方便的 Widget 回调可能变成真正的召唤实现。恢复：拒绝 diff，改为走 TASK-008 边界。
- 加 UMG 时可能顺带引入 Slate 或无关插件。恢复：退回最低限度模块依赖，更大变化请求架构 Review。
- 卡牌 UI 可能暗示未经批准的生命周期。恢复：把表现状态标记为本次 Demo 会话行为，正式生命周期留在范围外。
- 安全回退是回退本任务实现提交，并只移除本任务拥有的 Demo UI/卡牌资源。

## 完成报告

- 最终状态：`Review`；契约已准备，尚未实现。
- Primary Agent 和 Role：等待批准 / `module_worker` / UI and Input Module Agent。
- 修改文件：尚无；实现路径已在上方列明。
- 验收证据：等待实现。
- 已运行检查及结果：等待实现。
- 未运行检查及原因：本契约处于 Review 时不授权运行时、编译、自动化或 PIE 工作。
- 已解决或接受的 Review 问题：等待实现 Review。
- 剩余风险和未决问题：所有延期输入按设计保持未决。
- 已更新的中英文文档：契约配对已准备。
- Agent 产生的学习证据：等待。
- 用户产生的学习证据：等待。
- 集成提交或交接引用：等待批准；不推送。
