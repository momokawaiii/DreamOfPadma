# TASK-008 Encounter Demo 运行时、角色与技能数据

- 英文原文（Agent 阅读）：`TASK-008-Encounter-Character-and-Skill-Data.md`
- 文档 ID：`TASK-008`
- 版本：`0.2`
- 状态：`Verified`
- 上级里程碑或集成 Goal：`TASK-006 固定可玩原型垂直切片`
- Primary Role：`Gameplay Module Agent`
- Primary Agent：当前 root / Gameplay Module Agent
- 分支/worktree：Local 分支 `codex/mvp-demo-foundations`；默认不使用独立 Worktree
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

### 实现补充（2026-09-08）

用户实现要求授权补齐固定场景 Game 适配器。Session 保存合法上下文后发布代次凭据；延迟切场和消费同时核对凭据、GameInstance 和 World。目标场景 BeginPlay 就绪后才初始化，失败保留上下文且不自动循环重试。Gameplay 只消费值类型，不依赖具体会话子系统。补充旧凭据／F12／重复请求／缺失地图检查。原世界输入和配置保持现状。

使用 mycard 第 29 页维特鲁威人及三个技能描述作为只读来源文本。D22d 固定 C→R，本表现 fixture 不设置属性、费用或主动效果。占位几何大小属于表现参数，来源文本不代表数值批准。

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

- `Source/DreamOfPadma/Public/Demo/Integration/`
- `Source/DreamOfPadma/Private/Demo/Integration/`
- `Source/DreamOfPadma/Public/Demo/Session/DemoTransitionSessionSubsystem.h`
- `Source/DreamOfPadma/Private/Demo/Session/DemoTransitionSessionSubsystem.cpp`
- `Source/DreamOfPadma/Private/Demo/Session/DemoTransitionContextTest.cpp`
- `Scripts/Editor/AuthorDemoEncounter.py`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`

本任务不拥有其他 TASK-007 会话/世界路径、`DreamOfPadma.Build.cs`、`Config/DefaultEngine.ini`、卡牌/UI 路径、共享 ProjectState/index 或最终集成任务。

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

- [x] 局部 Demo Encounter 场景可以从 TASK-007 上下文初始化；上下文缺失时给出可读失败且不留下部分状态。
- [x] 一个项目自有 Demo 角色定义通过强类型数据边界加载，并有稳定 playtest 身份、显示数据、表现引用和至少一项技能定义。
- [x] 只读角色查询返回配置的技能列表，不在 Widget 或 Actor 实例中复制数据。
- [x] 运行时暴露有边界的召唤操作，接收 Demo 角色身份和明确出生点上下文，创建/返回占位运行时表现，并发出强类型成功/失败结果或事件。
- [x] 未知角色或无效出生上下文会被拒绝，不生成残留对象，也不修改无关状态。
- [x] 不执行技能，不选择战斗规则，不发明任何 Open/Deferred 玩法决定。
- [x] 模块 README 配对记录所有权、数据路径、召唤/查询边界、测试和剩余风险。
- [x] 独立 Reviewer 返回 `Pass`，或只留下集成前明确接受的 P3 改进。

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

- 最终状态：Verified；root／Gameplay Module Agent，Local codex/mvp-demo-foundations。
- 已实现：强类型角色／技能定义、只读查询、经校验的世界级召唤服务、Encounter 场景、带发布凭据的固定 Game 切场适配器，以及不生成默认 Pawn 的场景 GameMode。显式 smoke 截图等待八个实际帧，正常游玩不自动召唤。
- 内容：L_DemoEncounter.umap、DA_Demo_VitruvianMan 及三个来源技能资产。AuthorDemoEncounter.py 创建缺失资产并保留已有内容。
- 验证（2026-09-08）：UE5.8.2 Editor 编译通过；automation-reviewed/index.json 记录四项项目测试通过，测试警告／失败均为零。覆盖旧／F12／重复凭据、失败初始化、零缩放／无效旋转、定义更新、无效／重复召唤、出生变换保留与 ACT 配置。
- 真实运行证据：travel-render-reviewed.log 记录 DemoSandbox → OpenLevel → 消费凭据 → 一个维特鲁威人表现 → 三个配置技能，Pending=0。encounter-reviewed.png 在第 11 帧采集，已检查斜俯视镜头、可读标题／状态、中心出生标记和唯一占位角色。
- 资产检查：author-encounter.log 与 author-finalize.log 的 commandlet 错误／警告均为零；两个脚本对已有内容重跑通过。编译和验证产物位于 C:/Users/lenovo/.codex/visualizations/2026/09/08/01a07f68-77cf-7fe0-8479-93a66fb8a4f6/runtime-review。
- Review 修正：逐轴变换校验、测试失败安全清理、正确 Sphere 资产、旧凭据保护、延后帧截图及去除额外默认 Pawn。独立只读 Reviewer /root/full_mvp_contract_review 于 2026-09-08 检查最终构建、四项测试报告、资源脚本重跑和渲染图后返回 Pass，无剩余 P0–P2 问题。
- 环境说明：中文文化的引擎启动 smoke 有 13 条 Condition failed 断言，在改动前 Saved/Logs/DreamOfPadma.log 和 2026.09.07-18.08.16 备份中已存在。最终测试进程使用 -culture=en 后没有这些断言；未改变项目／用户语言设置。旧的测试世界上下文警告在最新报告中已消失。自动审批曾拒绝生成输出命令，补充仓库脚本规则后已允许 UE 正常生成，当前无审批阻塞。
- 用户复现：打开 DemoSandbox，进入 PIE，点击地块，按 Enter 切场，再按 F8 请求 fixture 召唤。角色／技能定义位于 Content/Padma/Demo/Definitions。卡牌和技能面板仍属 TASK-009。
- 文档：任务与模块双语同步。最终交付通过 AuditDocs（186 份 Markdown／93 对）、ValidateProject -Strict、git diff --check、946 个本地链接、25 份任务元数据／依赖检查，以及 136 个精确路径审计（108 个 Markdown／28 个实现路径）。受保护配置／构建／项目文件的哈希与预检基线一致。
- Agent 学习证据：代码／测试展示定义、查询、实例分离，以及校验命令和凭据生命周期。不宣称用户已完成 PIE 验收或复述。
- Changelog：Coordinator 已完成双语 Added 条目 — 来源 Encounter 场景／查询／召唤与固定切场。本任务没有实现技能效果、GAS 战斗、卡牌生命周期、完整图／地图存储、日历、经营或存档。
- 集成：仅工作区候选；未提交、合并或推送。
