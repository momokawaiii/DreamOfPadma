# TASK-007 SLG Demo 世界选择与切场

- 英文原文（Agent 阅读）：`TASK-007-SLG-World-Selection-and-Transition.md`
- 文档 ID：`TASK-007`
- 版本：`0.1`
- 状态：`Review`
- 上级里程碑或集成 Goal：`TASK-006 固定可玩原型垂直切片`
- Primary Role：`World Module Agent`，并获得 Game 组合支持
- Primary Agent：本实现会话由 `module_worker` / World Module Agent 承担
- 分支/worktree：Local 分支 `feature/TASK-007-world-selection-transition`；默认不使用独立 Worktree
- 任务模式：有边界的运行时和 Demo 地图实现；不实现通用世界规则

## Goal

实现原型功能切片的第一个可见 SLG 步骤：PIE 可以显示一个项目自有的 Demo 世界地图，用户可以选择并高亮一个 Demo 地块，确认后产生包含 Demo 节点、场景和出生点身份的强类型切场上下文，供局部场景消费。

本任务可以在该边界后提供一个可替换的直接关卡跳转适配器，但不得决定最终世界图、移动规则或 World Partition 方案。

## 为什么现在做

TASK-006 冻结了固定 fixture 垂直切片及其非正式规则边界。默认地图 PIE 启动已经通过验收，因此下一个可观察结果是一个用户可操作的战略选择流程，它传递稳定上下文，而不是把 Actor 指针或原始坐标交给下一个场景。

## 首先阅读

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Production/Tasks/TASK-006-Playable-Prototype-Cluster.md`
- `Docs/Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/ModuleMap.md`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Design/EN/04_LevelContentPlanner.md`
- `Content/AGENTS.md`
- `Source/AGENTS.md`

## 决策状态

### 已冻结输入

- 本任务使用 TASK-006 明确标记的一个 playtest fixture：一个可选择节点和一个目标局部场景。
- 选择是表现/输入状态；进入所选 Demo 场景必须通过强类型应用请求和切场上下文。
- 稳定 ID 标识 Demo 节点、场景和出生点。坐标与关卡资源路径是表现/配置数据，不是存档身份。
- 上下文可以在切场期间存在于 GameInstance/会话边界，但不能演变成万能玩法管理器。
- 项目自有地图和地块资源位于 `Content/Padma/Demo/World/`。
- 本任务不新增 UE 模块或外部依赖。

### 延期或开放输入

- 通用图拓扑、邻接、路线限制、移动成本、资源成本、归属、发现、占领、设施、世界变化和敌方驻军。
- 敌我共同战斗路线结算，以及沙盘和局部场景是否共用 World Partition。
- 最终场景流送策略、存档模式、正式 ID 命名空间、导入管线和参考项目资源迁移。
- Encounter 行动条规则、超出固定 fixture 的卡牌合法性、随机结果及其他所有 Open/Deferred 设计内容。

如果没有其中一个决定就无法验收，必须返回该决定；不得猜默认值，也不得藏入硬编码规则。

## 范围

### 允许路径和写入集合

Primary Agent 只能修改：

- `Source/DreamOfPadma/Public/Demo/Session/`
- `Source/DreamOfPadma/Private/Demo/Session/`
- `Source/DreamOfPadma/Public/Demo/World/`
- `Source/DreamOfPadma/Private/Demo/World/`
- `Content/Padma/Demo/World/`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaWorld/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.zh-CN.md`

`Config/DefaultEngine.ini`、`DreamOfPadma.Build.cs`、`Docs/00_INDEX*`、`Docs/ProjectState*`、Encounter 地图和卡牌/技能 UI 路径不属于本任务。

### 不在范围内

- 通用六边形世界图、寻路、移动动画、资源扣除、节点变化、敌方驻军、战斗路线解析器、存档/回滚或 World Partition 决策。
- Encounter 玩法、角色定义、卡牌手牌、技能面板、出牌、角色生成、战斗或 Debug 面板。
- UI 直接修改世界状态、用 Actor 指针作为切场身份，或把固定关卡名分支当作玩法规则。
- 整体复制或编辑 `E:\2026ue\padma` 内容。

## 依赖与集成顺序

- 必须先完成：TASK-006 已为 `Done`，ADR-0003 已为 `Accepted`；本任务已获用户批准并设为 `Ready`。
- 不依赖 TASK-005；本 fixture 没有随机结果。
- TASK-001 的默认地图 PIE 证据是前置基线。
- `TASK-008` 消费本任务的切场上下文和目标身份，不得修改 TASK-007 的源代码或世界资源路径。
- `TASK-009` 只消费后续 Encounter 任务暴露的只读局部状态。
- `TASK-010` 负责默认地图配置和端到端集成证据。
- 本任务不拥有共享索引、ProjectState、中心 Build 文件或默认地图配置。

### 共享冲突

本任务的 Demo 世界地图及 `Content/Padma/Demo/World/` 下的二进制资源，在独立 Review 完成前只归本任务使用。UE build/Editor/PIE 通道保持串行；期间不得有其他写入者修改这些路径。

## 委派计划

Primary Agent 对整体负责。只能针对一个明确问题请求只读专家：

| Role | 工作包 | 权限 | 预期输出 |
|---|---|---|---|
| `level-content-planner` | 检查一个 Demo 地块和一个明确局部场景映射是否足以展示选择，同时不定义正式世界图。 | 只读 | Fixture Review 与排除规则列表。 |
| `architect` | 检查会话上下文所有权、稳定 ID、切场边界和依赖方向。 | 只读 | 边界问题或确认。 |

不授权第二位写入者修改地图、会话上下文或世界源代码路径。

## 验收标准

- [x] 项目自有 Demo Sandbox 地图可以在 PIE 中加载，且不修改生成目录。
- [x] 一个 Demo 地块有稳定的 playtest 身份，可以被选择、高亮和取消选择。
- [x] 选中地块通过强类型查询/请求暴露 Demo 场景和出生点上下文，而不是用 Actor 指针作为身份。
- [x] 确认选择后产生可在切场后消费的上下文；fixture 缺失或无效时给出可读失败，且不留下部分切场状态。
- [x] 实现不扣除资源、不解析通用路线、不修改持久世界状态，也不发明正式节点/ID/导入策略。
- [x] 世界地图和源实现只位于精确允许路径，World/Game 模块文档配对记录新的边界。
- [ ] 独立 Reviewer 返回 `Pass`，或只留下在集成前明确接受的 P3 改进。

## 验证方案

### 自动检查

- 实现后编译 UE Editor Target。
- 如果上下文可以脱离地图测试，运行一条窄范围 fixture/上下文测试，证明稳定 ID 和失败无突变；不创建 UI 自动化。
- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1`。
- 共享项目结构变化时运行 `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict`。
- `git diff --check` 和精确变更路径审计。

### 手工检查

1. 在 PIE 中启动已批准的默认地图。
2. 观察 Demo Sandbox 地图和一个可选择地块。
3. 点击地块，观察高亮和选择状态。
4. 确认进入，观察包含节点、场景和出生点身份的切场请求/上下文。
5. 验证无效或缺失 fixture 会报告失败，并保持原状态不变。

固定地图还提供一个窄范围 fixture 诊断路径供 Review 使用：`F10` 清除当前选中 fixture 的场景 ID，按 `Enter` 验证可读失败和 pending 上下文不变，`F11` 恢复已编写 fixture，`F12` 消费 pending 上下文。这不是通用 Debug 面板，也不是 UI 自动化套件。

后续 TASK-010 负责进入实际 Encounter 地图的完整路径。

### 必需证据

- 编译输出。
- 一份简洁的自然语言 PIE 观察记录，覆盖地图、地块选择/高亮、取消选择、确认和无效 Fixture 结果；截图或视频为可选项。
- 若实现窄测试，提供上下文/失败测试输出。
- 变更路径列表和独立 Review 报告。

## 学习契约

| 目标 | 所需证据 | 状态 |
|---|---|---|
| 稳定身份与表现坐标 | 用户预测地图重建后哪些值仍应相同，并解释坐标/Actor 指针为什么不是存档身份。 | `Introduced` |
| 跨场景指令/上下文所有权 | 用户追踪从地块输入到会话上下文的路径，并指出每项状态由谁允许修改。 | `Introduced` |

- 学习者亲手练习：只改变 Demo 地块的显示位置，不改变稳定身份，然后预测哪些切场字段应保持相同。
- 迁移问题：如果两个不同的表现地图代表同一个节点，哪些数据必须共享，哪些数据可以不同？

## 风险与恢复

- 直接关卡跳转可能把资源名泄漏到玩法规则。恢复：通过强类型上下文路由，并把关卡资源保留为 fixture 表现数据。
- 地图 Actor 可能成为存档/事实来源。恢复：把稳定 ID/上下文留在会话边界，并将改动退回 Review。
- 任务可能扩张为世界图实现。恢复：停在一个 fixture，另建 World 任务。
- 安全回退是回退本任务实现提交，并只移除本任务拥有的 Demo World 资源；不得删除无关 Content。

## 完成报告

- 最终状态：`Review`；实现已准备交给独立 Review。用户已于 2026-09-07 批准契约。
- Primary Agent 和 Role：`module_worker` / World Module Agent。
- 修改文件：`Source/DreamOfPadma/Public/Demo/Session/`、`Source/DreamOfPadma/Private/Demo/Session/`、`Source/DreamOfPadma/Public/Demo/World/`、`Source/DreamOfPadma/Private/Demo/World/`、`Content/Padma/Demo/World/DemoSandbox.umap`、PadmaWorld README 双语对、PadmaGame README 双语对以及本 TASK-007 双语对。
- 验收证据：项目自有 `DemoSandbox.umap` 已在 unattended runtime smoke 中加载，`ADemoSandboxWorld::BeginPlay` 输出 TASK-007 初始化提示；强类型请求/上下文路径和 GameInstance 会话边界已实现；无效会话替换和缺失 fixture 字段由窄范围自动化测试覆盖；用户已手工观察最终带光照地图的选择、取消、确认、稳定 ID 状态和无效 fixture 诊断流程。根据用户的证据决定，这份自然语言 PIE 观察记录已经足够，截图或视频为可选项。
- 已运行检查及结果：依赖方向重构后的 UE5.8 `DreamOfPadmaEditor Win64 Development` 编译通过；窄范围 `DreamOfPadma.Demo.TASK007.TransitionContext` 自动化测试在临时 `-DDC-ForceMemoryCache -ddc=NoZenLocalFallback` 启动参数下通过，结果为 `succeeded=1 / failed=0`；`UnrealEditor-Cmd.exe ... /Game/Padma/Demo/World/DemoSandbox -game -nullrhi ... -ExecCmds=quit` 成功加载最终地图、进入 Play 并输出 `[TASK-007]` BeginPlay 日志；`pwsh -NoProfile -File .\Scripts\AuditDocs.ps1` 通过，共检查 124 个 Markdown 文件和 62 对语言文档；`pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict` 通过；`git diff --check` 通过；精确变更路径审计通过。
- 未运行检查及原因：当前环境无法控制原生应用，因此未执行 Agent 侧视觉捕获；根据用户的证据决定，截图或视频为可选项，改用自然语言 PIE 观察记录。独立 Reviewer 证据尚未取得。第一次通过 `RunTests.ps1` 的包装调用在测试启动前遇到机器上的不可写 Installed DDC 图；之后使用临时内存 DDC 参数直接重跑并通过，没有修改项目配置。
- 已解决或接受的 Review 问题：最终带光照的 `DemoSandbox.umap` 已在原实现提交 `859fe5a` 之后通过本地提交 `f80f727` 纳入分支；已移除 World -> 具体 Game/Session 的依赖，World 现在暴露强类型发布/消费回调，GameInstance 会话子系统在地图中的 Actor 完成初始化后绑定这些回调。这也修复了只在 `PostLoadMap` 阶段绑定造成的 PIE 生命周期竞态：绑定发生在 Sandbox Actor 初始化之前，导致按下 Enter 时发布器不可用。第一次测试暴露的直接构造 `UGameInstanceSubsystem` Outer 无效问题仍通过可脱离地图的 `FDemoTransitionContextStore` 委托解决。Changelog 表述已限定为只在同一 GameInstance/会话内保留。集成前仍需独立实现 Review。
- 剩余风险和未决问题：默认地图配置和 Encounter 场景仍由 TASK-010/TASK-008 负责；所有 Open/Proposed/Deferred 的世界、路线、ID 命名空间、导入、存档和 World Partition 决策保持不变。独立 Review 待完成。
- 已更新的中英文文档：是；两个模块 README 双语对和 TASK-007 双语对已同步。
- Changelog 草稿：日期 `2026-09-08`；TASK/Goal `TASK-007 SLG World Selection and Transition`；类别 `Added`；面向用户摘要 `增加项目自有 Demo Sandbox，提供一个可选择地块，以及可在同一 GameInstance/会话内跨场景保留的节点/场景/出生点强类型切场上下文`；影响区域 `Demo World 表现、Demo Session 切场交接、PadmaWorld/PadmaGame 模块边界`；验证证据 `依赖方向重构后的 UE5.8 Editor 编译、TASK-007 上下文自动化测试 Success、unattended DemoSandbox 地图加载 smoke、用户 PIE 观察`；未解决/延期事项 `独立 Review、默认地图集成、Encounter 场景以及所有延期世界规则仍保持开放`。
- Agent 产生的学习证据：稳定身份与表现坐标、跨场景所有权两个目标均为 `Introduced`：前者由强类型 ID 和自动化断言体现，后者由 World selection model -> 强类型 session store -> GameInstance subsystem 路径体现。不宣称用户已完成讲解回授。
- 用户产生的学习证据：待补充；用户仍需完成只改显示位置的练习、比较三个身份字段并回答迁移问题，之后目标才可标记为 `Demonstrated`。
- 集成提交或交接引用：Local 分支 `feature/TASK-007-world-selection-transition`，提交为 `859fe5a` 和 `f80f727`，未合并、未推送；可交给独立 Review。原有 `AGENTS.md` 双语修改仍单独保存在 `stash@{0}`，未混入本 TASK。
