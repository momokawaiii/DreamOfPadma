# TASK-008 Encounter Demo 运行时、角色与技能数据

- 英文原文（Agent 阅读）：`TASK-008-Encounter-Character-and-Skill-Data.md`
- 文档 ID：`TASK-008`
- 版本：`0.1`
- 状态：`Ready`
- 上级里程碑或集成 Goal：`TASK-006 固定可玩原型垂直切片`
- Primary Role：`Gameplay Module Agent`
- Primary Agent：新实现会话开始时分配 `module_worker`
- 分支/worktree：Local 分支 `feature/TASK-008-encounter-character-skill`；默认不使用独立 Worktree
- 任务模式：有边界的局部场景运行时、角色/技能 fixture 和召唤边界；不实现战斗

## Goal

实现固定原型切片的局部 Encounter 部分。目标场景可以消费 TASK-007 的切场上下文，初始化一个项目自有的 Demo Encounter，加载一个带少量技能列表的数据驱动角色定义，暴露只读的角色/技能查询，并为 TASK-009 的卡牌指令提供权威角色召唤边界。

可观察结果是一个 Encounter 场景和运行时边界：它能显示角色将被召唤的位置，并能返回角色的技能定义。不激活技能，也不结算战斗。

## 为什么现在做

TASK-007 提供稳定的场景上下文。参考项目把卡牌数据和技能放进大型管理器；DreamOfPadma 需要一个最小且可复用的局部运行时边界，让后续 UI 请求召唤并读取同一份角色数据，而不是把规则写进 Widget。

## 首先阅读

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Production/Tasks/TASK-006-Playable-Prototype-Cluster.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.md`
- `Docs/Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaTests/README.md`
- `Content/AGENTS.md`
- `Source/AGENTS.md`

## 决策状态

### 已冻结输入

- 局部场景是 TASK-006/007 选定的一个固定 playtest Encounter fixture。
- 角色身份、技能身份、显示文本和表现引用使用数据驱动，并与存活 Actor 指针分离。
- 角色技能查询是只读的，返回与召唤/运行时边界使用的同一份定义。
- 召唤边界接收稳定角色身份和明确的局部出生上下文，并返回 UI 可以观察的强类型结果/事件。
- Demo 定义可以使用 `Content/Padma/Demo/` 下的小型项目自有 Primary Data Asset 或等价强类型 fixture；这不决定全项目最终导入方案。
- 允许使用占位网格/材质，不要求导入外部原型资源树。

### 延期或开放输入

- 最终 CardDefinition/CharacterDefinition 模式、正式 ID 命名空间、导入管线、Asset Manager 策略和内容本地化策略。
- 技能激活上下文、目标规则、消耗、冷却、动画时序、效果、伤害、状态、AI、Encounter 行动条优先级和战斗结局。
- 抽牌/卡牌生命周期、资源消耗、随机结果、存档/回滚、网络、ACT/GAS 和最终场景流送方案。

任务可以暴露 fixture 专用的查询和召唤操作，但不得把这些未决项目变成正式玩法规则。

## 范围

### 允许路径和写入集合

Primary Agent 只能修改：

- `Source/DreamOfPadma/Public/Demo/Encounter/`
- `Source/DreamOfPadma/Private/Demo/Encounter/`
- `Content/Padma/Demo/Encounter/`
- `Content/Padma/Demo/Definitions/Characters/`
- `Content/Padma/Demo/Definitions/Skills/`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.zh-CN.md`

本任务不拥有 TASK-007 的会话/世界路径、`DreamOfPadma.Build.cs`、`Config/DefaultEngine.ini`、卡牌/UI 路径、共享 ProjectState/index 或最终集成任务。

### 不在范围内

- 完整 Encounter 行动条、先攻、优先级、反应、打断、额外行动、结束回合效果、战斗公式、状态、AI、胜负、存档或战斗事务。
- 技能施放或效果执行；技能列表只是只读表现数据。
- 卡牌手牌、抽牌、生命周期、资源支付、随机选择、合成、ACT、GAS、Debug 面板或万能 Manager。
- 在 Widget 或地图 Actor 中硬编码角色/技能数据。
- 整体复制或修改 `E:\2026ue\padma` 资源。

## 依赖与集成顺序

- 必须先完成：TASK-006/ADR-0003 获批；TASK-007 的切场上下文和 Demo World 契约获批并集成。
- 不依赖 TASK-005；Demo 角色和技能 fixture 没有随机结果。
- `TASK-009` 消费只读角色查询和召唤边界，不得编辑本任务源代码或定义路径。
- `TASK-010` 消费 Encounter 地图和局部运行时，负责端到端 PIE 检查。
- 如果需要新的 Core 公共模式、模块依赖、存档形状或插件，必须停止并请求架构任务/ADR，不得扩大本写入集合。

### 共享冲突

Encounter 地图、角色/技能 Demo 定义和 `Demo/Encounter/` 源路径在独立 Review 完成前只归本任务使用。UE build/Editor/PIE 通道保持串行；实现期间不得有其他写入者修改这些资源或路径。

## 委派计划

| Role | 工作包 | 权限 | 预期输出 |
|---|---|---|---|
| `architect` | 检查只读定义/查询和召唤边界是否符合 Core/Gameplay 所有权及未来存档/网络边界。 | 只读 | 边界问题或确认。 |
| `reviewer` | 检查实际运行时/数据 diff、fixture 限制、资源加载和 Open/Deferred 保留。 | 只读 | P0-P3 问题和验收状态。 |

不授权第二位写入者修改 Encounter 地图、角色定义、技能定义或召唤源代码。

## 验收标准

- [ ] 局部 Demo Encounter 场景可以从 TASK-007 上下文初始化；上下文缺失时给出可读失败且不留下部分状态。
- [ ] 一个项目自有 Demo 角色定义通过强类型数据边界加载，并有稳定 playtest 身份、显示数据、表现引用和至少一项技能定义。
- [ ] 只读角色查询返回配置的技能列表，不在 Widget 或 Actor 实例中复制数据。
- [ ] 运行时暴露有边界的召唤操作，接收 Demo 角色身份和明确出生点上下文，创建/返回占位运行时表现，并发出强类型成功/失败结果或事件。
- [ ] 未知角色或无效出生上下文会被拒绝，不生成残留对象，也不修改无关状态。
- [ ] 不执行技能，不选择战斗规则，不发明任何 Open/Deferred 玩法决定。
- [ ] 模块 README 配对记录所有权、数据路径、召唤/查询边界、测试和剩余风险。
- [ ] 独立 Reviewer 返回 `Pass`，或只留下集成前明确接受的 P3 改进。

## 验证方案

### 自动检查

- 编译 UE Editor Target。
- 如果边界可以脱离完整地图运行，执行一条窄范围数据/查询及无效召唤检查；不创建 UI 自动化套件。
- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1`。
- 共享项目结构变化时运行 `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict`。
- `git diff --check` 和精确变更路径审计。

### 手工检查

1. 使用 TASK-007 上下文进入 Demo Encounter 场景。
2. 验证局部场景找到预期出生点和场景身份。
3. 检查 Demo 角色定义，确认至少存在一项技能。
4. 通过公开的指令/服务边界调用有边界的召唤操作。
5. 确认一个占位角色出现在预期位置，并确认无效身份给出可读失败且没有额外角色。

TASK-009 负责围绕该边界绘制卡牌和技能面板。

### 必需证据

- 编译输出和窄范围查询/召唤测试输出（如有）。
- 展示局部场景、占位角色和定义证据的截图或短 PIE 录像。
- 变更路径列表和独立 Review 报告。
- 明确说明没有执行技能或实现未决规则。

## 学习契约

| 目标 | 所需证据 | 状态 |
|---|---|---|
| 定义与运行时状态的区别 | 用户指出哪些字段属于不可变角色/技能定义，哪些字段属于生成后的运行时实例。 | `Not started` |
| 只读查询与权威召唤 | 用户追踪 UI 为什么请求服务召唤并读取视图，而不是修改 Actor 或 Widget 自己保存的副本。 | `Not started` |

- 学习者亲手练习：修改 Demo 角色定义中的技能描述，预测在不改变召唤指令的情况下，运行时/视图会出现什么变化。
- 迁移问题：如果未来技能增加冷却，配置的默认值和每场战斗的当前值分别应放在哪里？

## 风险与恢复

- 临时 fixture 类型可能被误认为最终卡牌/角色模式。恢复：标记为 playtest-only，任何正式模式请求返回架构 Review。
- 召唤服务可能膨胀成万能管理器。恢复：限制在 Demo Encounter 边界及强类型请求/结果内。
- 资源加载可能直到 PIE 才暴露缺失引用。恢复：初始化时校验定义并返回可读失败。
- 安全回退是回退本任务实现提交，并只移除本任务拥有的 Demo Encounter 资源。

## 完成报告

- 最终状态：`Ready`；TASK-007 已在 2026-09-08 通过独立 Review 后完成本地集成，因此本已批准契约已解锁新的实现会话；尚未实现。
- Primary Agent 和 Role：新会话开始时分配 / `module_worker` / Gameplay Module Agent。
- 修改文件：尚无；实现路径已在上方列明。
- 验收证据：前置门槛已由本地集成的 TASK-007 切场上下文满足；TASK-008 运行时验收仍待实现。
- 已运行检查及结果：TASK-007 集成期间已验证契约和仓库文档门槛；不宣称任何 TASK-008 运行时结果。
- 未运行检查及原因：TASK-008 尚未开始运行时、编译、自动化或 PIE 工作；推进到 Ready 本身不代表已有实现证据。
- 已解决或接受的 Review 问题：当前范围的契约级 Review 门槛已完成；Primary Agent 交付后仍必须进行实现 Review。
- 剩余风险和未决问题：所有延期输入按设计保持未决。
- 已更新的中英文文档：契约配对已准备。
- Agent 产生的学习证据：等待。
- 用户产生的学习证据：等待。
- 集成提交或交接引用：TASK-007 本地收尾提交 `9ea99c7` 已满足前置门槛；TASK-008 已准备在 Local 分支 `feature/TASK-008-encounter-character-skill` 开始；尚无实现提交，不推送。
