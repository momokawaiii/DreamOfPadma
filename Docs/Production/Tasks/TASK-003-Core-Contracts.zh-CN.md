# TASK-003 核心规则契约切片

- 英文原文（Agent 阅读）：`TASK-003-Core-Contracts.md`
- 文档 ID：`TASK-003`
- 版本：`0.1`
- 状态：`Done`
- 所属里程碑或集成 Goal：`M1 Core rules / 第一个核心规则基础`
- Primary Role：`Integration Coordinator`
- Primary Agent：本串行契约冻结 Goal 由 `Integration Coordinator` 负责；只有本契约获批准后才分配下游实现 Agent
- 分支/worktree：`main`，由集成方拥有的检出；不使用独立 worktree
- 任务模式：仅做任务设计和契约冻结；本任务不授权代码或资源实现

## Goal

为第一个 M1 核心规则基础冻结一份最小的、独立于表现层的共享契约，使后续 Calendar、Resource Ledger 和 Deterministic Random Goal 可以在不重新定义所有权或公共语义的情况下实现和 Review。

可观察交付物是一份双语 TASK 契约和下游测试矩阵，只覆盖已经冻结的概念：稳定身份和值语义、显式日历状态与事件、分离的资源账本语义、确定性计算上下文与追踪元数据，以及类型化的 command/result/error/event 边界。本任务不实现 Calendar、Resource Ledger 或 Deterministic Random 的运行时功能。

## Why now

MVP 策划与架构基线已经验收，但当前项目仍只有生成的 `DreamOfPadma` 模块，也没有项目自有的玩法测试套件。下游实现 Goal 展开前必须先记录共享 Core 契约；否则日历、资源和随机工作可能各自选择不兼容的状态、错误、事件或依赖形状。

这是第一轮 M1 实现的串行前置任务，也遵守“worktree 属于可独立合并的 Goal，而不是默认属于某个逻辑模块”的项目规则。

## Read first

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Production/Tasks/TASK-002-Agent-Workflow-Bootstrap.md`
- `Docs/Rules/Time.md`
- `Docs/Rules/WorldState.md`
- `Docs/Design/EN/02_SystemPlanner.md`
- `Docs/Design/EN/05_NumericalPlanner.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/ModuleMap.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/SaveSchema.md`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaTests/README.md`

## Decision state

### Frozen inputs

本契约只能冻结以下语义：

| 契约领域 | 已冻结边界 |
|---|---|
| Core 所有权 | Core 规则和契约独立于 UMG、Niagara、具体 Actor、地图、输入设备和表现状态。当前生成的 `DreamOfPadma` 模块只是暂存实现位置；本任务不创建新的 UE 模块。 |
| 身份与状态 | 稳定 ID 与显示名、本地化、资源路径、Actor 指针和地图坐标分离。静态定义与可变运行状态分离。ID 是类值对象并可序列化；本任务不选择新的命名空间。 |
| 日历 | 日历状态显式保存 `ChapterIndex`、`EraIndex`、`DayIndex` 和 `Phase`。固定层级为 Dawn → Morning → Dusk、七天一 Era、七 Era 一 Chapter。阶段切换是显式事件；Era 和 Chapter 变化有独立事件。 |
| 资源 | `Flow` 和 `Computation` 是分离的账本。当前值、上限、债务表示、来源和原因不能压成一个无法解释的数字。本任务不选择产品默认值、债务政策、每日获得量或资源循环。 |
| 确定性 | 可复现的计算上下文能够识别运行/master seed、命名 stream、command 或 event 位置、需要时使用的输入定义 ID 与版本，以及 Debug/Test 构建中的 roll 标识和值。未来合成任务使用该接缝时，候选选择和成功/失败仍是分开的随机步骤。 |
| Command 路径 | 会改变状态的工作遵循 command → validation → 权威状态变化 → 类型化 result 和显式 event(s)。无效 command 要暴露可读失败。UI、GM 工具、自动化测试和未来网络调用方使用同一公共路径。 |
| 测试边界 | 测试使用生产契约和规则路径；不能创建第二套会漂移的假规则实现，也不应在 Core 测试足够时依赖加载地图或 Widget。 |

### Deferred or open inputs

本任务只把下列内容记录为非绑定输入，不做任何解决：

- 资源和核心值的初始值与上限；债务上限、每日获得量、转换成本、循环保护以及任何随机补偿情形。
- 如果现有实现尚未确定，索引采用从零还是从一；以及超出架构已要求内容之外的序列化形状和定义版本政策。
- 最终稳定 ID 命名空间与内容命名约定，以及最终采用 CSV、JSON、DataTable、Primary Data Asset 或混合导入管线。
- 合成候选过滤语义、Softmax 过滤/归一化细节、公式系数、概率钳制与单位、B 场积累时机/成本、无共享标签 fallback，以及 Sigma 骰子/后果。
- 卡牌生命周期概率、ABC 主动技能在局部战斗中的合法性、Encounter 行动条优先级与窗口出牌额度，以及延期的 ACT 页面/切换/边界细节。
- 战斗、世界图、战斗事务、结局、存档迁移、UI 和表现层实现细节。

如果实现为了满足契约而需要其中任何一项，必须停止并把问题退回对应的 System Planner、Numerical Planner、Architect 或用户/策划决策方。不得为了继续推进而添加默认值。

## Scope

### Allowed paths and write set

本任务设计 Goal 只允许编辑以下路径：

- `Docs/Production/Tasks/TASK-003-Core-Contracts.md`
- `Docs/Production/Tasks/TASK-003-Core-Contracts.zh-CN.md`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`

本任务不得写入 `Source/`、`Content/`、`Config/`、`Build/` 或生成目录。以下未来实现路径仅用于交接说明，不代表本 Goal 获得编辑权限：

- `Source/DreamOfPadma/Public/Core/`
- `Source/DreamOfPadma/Private/Core/`
- `Source/DreamOfPadma/Private/Tests/Core/`
- `Docs/Architecture/Modules/PadmaCore/README.md` 和 `README.zh-CN.md`

### Out of scope

- 任何 C++、Blueprint、UE 模块、`Build.cs`、Config、地图、`.uasset` 或内容实现。
- 日历、资源账本或随机流运行时代码；自动化测试代码；数据资源；导入/验证工具；存档代码；或新的模块拆分。
- 卡牌定义实现、卡组/生命周期、合成服务、预览/执行一致性、Softmax、成功公式、Sigma 骰子或失败侧内容。
- Encounter、ACT/RealTimeAction、战斗、世界图、节点变化、战斗路线、战斗事务、结局、UI、音频、VFX、相机或输入实现。
- 选择数值默认值、概率钳制、ID 命名约定、导入格式、序列化格式或任何其他 Open/Deferred 规则。
- 创建下游任务文件或 worktree；改变 canonical 策划/规则/架构语义；合并、打标签、推送或打包。

## Dependencies and integration order

- 必要前置：`TASK-002-Agent-Workflow-Bootstrap` 状态为 `Done`；2026-09-02 的已验收 MVP 与架构基线是策划权威。
- 直接输入：**Read first** 中列出的英文规则、策划、架构、模块和学习文档。中文 companion 只镜像英文来源，不增加语义。
- 下游消费者：未来 Calendar、Resource Ledger 和 Deterministic Random Goal；以及后续 Card/Synthesis、World、Gameplay、Save、UI 和 PadmaTests 工作。
- 集成顺序：Review 本契约并获得用户/策划决策方批准后，才能创建下游写入 worktree。任何公共契约变更必须在实现继续前返回这个串行集成 Goal。
- 共享冲突：公共 Core 头文件、中心 `DreamOfPadma.Build.cs`、模块依赖、公共数据 schema、`Docs/ProjectState.md`、任务索引、模块 README 以及唯一的 UE build/Editor/PIE 通道继续由集成方拥有或保持串行。本任务不授权这些路径存在并发写入者。
- 所有权问题按来源文档指定的 Role 路由：System Planner 负责规则含义，Numerical Planner 负责系数/表，Architect 负责数据/依赖/schema 边界，用户/策划决策方负责尚未决定的玩法含义。

## Independent worktree decision

**不适合。本 Goal 不适合使用独立 worktree。** 它冻结下游 Goal 必须一致消费的共享公共契约、依赖方向、任务索引和项目状态路由。应在集成方拥有的检出（`main`）中由一位写入者串行完成。该契约完成 Review 并获批准后，才能在写入集合保持独占且合并顺序明确的前提下，评估是否把 Calendar、Resource Ledger 和 Deterministic Random 拆成独立 worktree Goal。

## Delegation plan

不授权可写 subagent。Integration Coordinator 对契约承担全部责任；批准前可以请求以下有限的只读检查：

| Role | Work package | Authority | Expected output |
|---|---|---|---|
| System Planner | 检查日历/资源/确定性陈述是否符合已接受的规则含义，并确认每个未决输入仍保持未决。 | 只读 | 带来源引用的不变量与开放项 Review。 |
| Architect | 检查 Core 所有权、依赖方向、公共契约范围、存档/回放接缝和下游建议写入集合。 | 只读 | 阻塞项、ADR 需求和集成顺序建议。 |
| Learning Tutor | 检查两个学习目标是否要求用户产出证据，并且仍限制在本切片内。 | 只读 | 练习、预测、复述和迁移问题 Review。 |

## Acceptance criteria

- [x] 英文和中文 TASK 文件作为同步配对存在，Document ID、版本、状态和验收边界一致。
- [x] Goal 是一个可观察的串行结果：冻结共享 Core 契约和下游测试矩阵；不暗示运行时实现。
- [x] 稳定 ID/值语义、显式日历状态/事件、分离的 Flow/Computation 账本、确定性追踪元数据、command/result/error/event 边界均已说明，且没有发明数值或玩法政策。
- [x] 所有可能影响本切片的 Open/Deferred 输入都作为非绑定内容列出并带有升级路径；没有一个被转换成默认值或隐藏规则。
- [x] 下游测试义务覆盖日历边界、资源事务隔离/来源、确定性复现和可读的 command 失败，并且不依赖 UI 或地图。
- [x] 确切写入集合、串行冲突列表、依赖顺序、Primary Agent 所有权和不使用独立 worktree 的决定均已明确。
- [x] `Docs/00_INDEX.md` 和 `Docs/00_INDEX.zh-CN.md` 链接新任务，对应的项目状态条目保持同步。
- [x] 当前仅设计的这一轮没有修改 source、资源、config、build、生成文件或测试实现文件。

## Tests

本任务只做契约，不实现游戏规则。因此只运行文档和范围检查，不运行玩法或 UE 自动化测试。以下矩阵是下游实现 Goal 必须遵守的测试契约：

| 未来切片 | 必须测试 | 预期观察 | 约束 |
|---|---|---|---|
| Calendar | 覆盖三个阶段切换，以及第一天/第七天、下一 Era、下一 Chapter 边界。 | 显式日历字段一致推进，并在正确边界发出 phase/Era/Chapter 事件。 | 不得用只在玩法代码中计算的 modulo 取代显式状态；不得假定未经批准的索引起点。 |
| Resource Ledger | 分别对 Flow 和 Computation 执行有效与无效事务。 | 一个账本不能修改另一个；result/event 记录 delta、来源、原因和 command 身份；无效输入不改变权威状态。 | 使用命名的测试 fixture，不使用产品默认值；不得发明债务或循环政策。 |
| Deterministic Random | 使用相同 seed、stream、position 和输入重复同一 fixture。 | 可复现的 result/trace 一致，包括 Debug/Test 构建所需的身份元数据和 roll 证据。 | 不得耦合无关 stream，也不得在本任务选择合成公式。 |
| Shared command path | 通过同一个公共 service path 执行一个有效 command 和一个 validation failure。 | result 和显式 event 描述状态变化；失败可读且不产生隐藏 mutation。 | 测试调用生产契约，不得复制规则逻辑。 |

### 本契约设计任务的检查

- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1` —— 文档配对和链接通过。
- `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict` —— 项目结构保持有效。
- `git diff --check` —— 无空白错误。
- 变更路径审计确认只有以上六个允许的 Markdown 路径发生变化。
- 手工 Review 确认英文/中文语义一致，且没有解决任何 Open/Deferred 项目。

## Verification plan

### Automated checks

- 从仓库根目录运行 **本契约设计任务的检查** 中的三个命令。
- 检查 `git diff --name-only`，并与精确允许写入集合比较。
- 搜索最终 diff 中的 `Source/`、`Content/`、`Config/`、`Build/`、生成目录、数值默认值，以及将 Open/Deferred 项目变成决定的措辞。

### Manual checks

- 将每条冻结陈述追溯到已接受的英文规则或架构来源。
- 用英文文件对照阅读中文 companion，确认 Document ID、版本、状态、链接和验收边界一致。
- 走查拟定的 Calendar、Resource Ledger 和 Deterministic Random 下游 Goal，确认它们只能在本契约批准后获得不重叠的写入集合。
- 确认没有为这个共享公共契约 Goal 分配独立 worktree。

### Required evidence

- Audit、validation 和 `git diff --check` 输出。
- 精确的变更路径列表。
- 协调人对 frozen 与 open/deferred 陈述的 Review。
- 在创建任何下游实现 Goal 或 worktree 前，取得用户/策划决策方批准。

## Learning contract

最多选择两个主要目标。

| Target | Evidence required | Status |
|---|---|---|
| 先契约的所有权与集成路由 | 用户预测为什么共享公共契约必须串行，指出一个边界的所有者，并在不重叠写入集合的前提下把 Calendar/Resource/Random 路由到后续 Goal。 | `Not started` |
| 确定性 fixture 推理 | 用户预测当 seed、命名 stream、position 和输入重复时哪些内容必须一致，并解释 trace 如何暴露不一致。 | `Not started` |

- 学习者动手练习：在下游实现开始前，为一个资源事务画出 command → validation → state → result/event 路径，并预测第七天边界和一个重复的确定性 fixture；之后亲自运行指定自动化测试并检查输出。
- 迁移问题：如果未来合成任务需要新的公共字段，或改变了资源的含义，应该原地修改本契约、创建新的有所有权任务，还是请求 ADR？需要什么证据来决定？

## Risks and recovery

- 范围漂移可能把共享契约冻结变成日历、资源、随机或合成实现。恢复方式：拆成新的有所有权 Goal，保持本任务停留在契约边界。
- 实现中可能发现某个 Open/Deferred 规则是必要依赖。恢复方式：停止，记录精确依赖和决策所有者，退回串行集成；不得猜测。
- 公共头文件或中心配置可能被并发编辑。恢复方式：保持本 Goal 串行，只在 Review 后由 Integration Coordinator rebase/merge。
- 最终稳定 ID 命名和导入/版本政策尚未完全固定。恢复方式：只使用不透明的测试 fixture；生产内容需要该政策时另建决策任务。
- 安全回滚方式是 Git revert 本任务的文档 pair、索引 pair 和项目状态 pair；不需要破坏性文件系统操作。

## Completion report

- Integration boundary note：前置检查点 `9bf49fa` 同时携带了用户此前已有的 TASK-001 双语 PIE 完成编辑。这两个文件已在该检查点中单独标识，并不是 TASK-003 所有的写入；本次 TASK-003 关闭 diff 限定在本任务允许的路径内。
- Final status：`Done`；契约已完成 Review 并获批准，尚未开始实现。
- Primary Agent 和 Role：当前 Integration Coordinator / Integration Coordinator。
- Changed files：`TASK-003-Core-Contracts.md`、`TASK-003-Core-Contracts.zh-CN.md`、`Docs/00_INDEX.md`、`Docs/00_INDEX.zh-CN.md`、`Docs/ProjectState.md` 和 `Docs/ProjectState.zh-CN.md`。
- Acceptance evidence：双语任务 pair、索引链接和同步的项目状态条目均已存在；TASK-003 关闭 diff 只包含本 TASK 允许的 Markdown 路径，没有 source、资源、config、build、生成文件或测试实现文件变化。前置检查点中独立的 TASK-001 pair 已在上方记录，并继续位于本任务写入集合之外。
- Checks run and results：`Scripts/AuditDocs.ps1` 通过 108 个 Markdown 文件和 54 对语言文件；`Scripts/ValidateProject.ps1 -Strict` 通过；`git diff --check` 和 `git diff --cached --check` 通过且无输出；TASK-003 关闭路径的精确暂存范围审计通过。
- Checks not run and reason：由于这是仅设计任务，玩法、UE 自动化、编译、PIE、打包和实现测试有意不运行。
- Review findings resolved or accepted：独立只读 Review 发现检查点 `9bf49fa` 携带了此前已有的 TASK-001 pair；上方已明确记录该边界，TASK-003 关闭 diff 复审后没有阻塞问题。用户已于 2026-09-07 批准本契约。
- Remaining risks and open questions：**Deferred or open inputs** 下所有项目按设计继续未决定。
- English/Chinese documentation updated：是。
- Agent-produced learning evidence：仅有任务契约和测试/学习计划。
- User-produced learning evidence：等待下游动手练习。
- Integration commit or handoff reference：TASK-003 关闭提交；最终 hash 由 Integration Coordinator 在交接中报告。本任务不包含合并或推送。
