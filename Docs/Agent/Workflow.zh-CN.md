# Agent 工作流

- 英文原文（Agent 阅读）：`Docs/Agent/Workflow.md`

## 共享上下文

Codex 对话是工作单元；仓库才是共享事实来源。Agent 不应依赖隐藏的聊天历史，而应使用 Git、任务文件、`ProjectState.md`、ADR 和测试结果沟通。

权威顺序依次是：用户当前请求、`AGENTS.md`、已验收英文策划/规则文档、架构与 ADR、当前任务契约、实现证据。下层可以细化，但不得静默违背上层。中文文档是供用户 Review 的英文源镜像。

## 协作词汇

| 单元 | 用途 | 所有权 |
|---|---|---|
| Milestone | 把能形成一项项目能力的结果组织在一起 | 集成协调者 |
| Integration Goal | 协调存在依赖的 Goal，并拥有最终仓库状态 | 一位集成负责人 |
| Goal | 一个可观察、可独立验收的结果 | 一位 Primary Agent |
| TASK 契约 | 为一个 Goal 版本化记录范围、决定、写入集合、验收、验证和学习证据 | Primary Agent 负责；集成负责人关闭 |
| Worktree | 为一个可独立合并的写入 Goal 提供 Git 隔离 | 一位 Primary Agent；临时存在，不是永久模块目录 |
| Role | 专业视角和权限边界 | 可被多个 Goal 或 Agent 复用 |
| Subagent | Goal 内负责一个有限工作包的临时工作者 | 父 Agent/Primary Agent 继续承担责任 |
| Skill | 针对重复工作的可复用流程 | 仓库级流程资产，不是当前项目事实 |

层级如下：

```text
项目
└─ Milestone
   └─ Integration Goal / 集成负责人
      ├─ Goal + TASK + worktree + Primary Agent
      │  ├─ 只读专家 subagent
      │  └─ 写入集合独占时的有限 worker subagent
      └─ Goal + TASK + worktree + Primary Agent
```

## 工作拆分

并行工作应使用互不重叠的所有权：

- 核心规则和测试
- 玩法和战斗
- 世界和地图
- UI 和表现
- 工具、构建和文档

不要让多个 Agent 同时编辑同一张 UE 地图、`.uasset`、中心配置文件或公共数据模式；如确有需要，建立独立集成任务。

创建工作前使用以下路由判断：

| 形态 | 适用条件 | 结果负责人 |
|---|---|---|
| 独立 Goal/worktree | 产物有自己的验收标准、跨多个回合、需要写文件，并能独立 Review/合并 | 对应 Primary Agent |
| Subagent 工作包 | 问题边界明确，为父 Goal 提供证据、提案、测试设计或一个独占写入集合的小补丁 | 父 Agent/Primary Agent |
| 串行集成工作 | 涉及共享模式、地图、`.uasset`、中心配置、模块依赖、迁移或同一写入集合 | 集成负责人或一位指定写入者 |

初始并发策略：

- `.codex/config.toml` 允许每个父会话最多同时存在四个被拉起的 subagent。这是安全上限，不是目标。
- 在真实集成证据证明边界稳定前，同时可写 worktree 不超过两个。
- 同一时间只使用一条 UE 构建/Editor/PIE 通道。该通道工作时可以继续只读分析，但其他 Agent 不得修改构建正在使用的文件。
- `Docs/ProjectState.md`、路线图状态、共享任务索引、中心模式、地图和共享 UE 资源只有一位集成写入者。

## 任务生命周期

```text
Backlog -> Ready -> In Progress -> Review -> Verified -> Done
```

每个任务都需要目标、允许路径、非目标、验收标准、测试方案和学习目标。

- `Backlog`：只记录想法，未授权实现。
- `Ready`：当前范围需要的决定已经记录；依赖、写入集合、验收、验证和学习目标完整。
- `In Progress`：一位 Primary Agent 拥有 Goal/worktree，并已声明写入集合。
- `Review`：实现稳定；独立 Review 者检查真实 diff 和证据。
- `Verified`：阻塞问题已解决或被明确接受，所需检查通过。
- `Done`：批准的结果已经集成，任务完成报告和共享状态已更新，提交可以备份到远端。

不能通过猜测开放规则让 Goal 进入 Ready。未决内容若影响验收或公共契约，应停止并询问指定决定者；若不影响当前切片，则把它列为延期并排除在范围外。

## Primary Agent 与 subagent 契约

即使进行了委派，Primary Agent 仍对整个 Goal 负责。每个 subagent 请求必须包括：

1. Role 和确切工作包；
2. 必需输入和规范文档；
3. 预期输出和证据格式；
4. 约束、非目标和停止条件；
5. 只读权限或确切写入权限；
6. 与其他工作的依赖关系以及返回时限/顺序。

只有人格设定不构成工作包。父 Agent 在采用 subagent 输出前，必须用仓库证据验证。subagent 不得改变 Goal 范围、关闭任务、合并或更新共享里程碑状态。

## 完成报告

工作者需要报告修改文件、执行的命令/测试、必要时的截图或打包证据、未解决风险和文档变化。集成任务负责更新 `Docs/ProjectState.md`。

## 双语同步

Agent 执行任务时读取英文文档。每次 Markdown 修改都必须在同一个任务和提交中更新中文镜像。中文文件是供用户阅读的镜像，不是独立事实来源。

## 决策控制

模块依赖、存档模式、规范规则、资源布局或构建目标发生变化时，必须在实现前或同一次修改中创建 ADR。

## 角色与文档路由

项目采用角色所有权，而不是由一个 Agent 修改所有文档：

| 角色 | 主要文档 | 主要职责 |
|---|---|---|
| 集成协调者 | ProjectState、Production、Tasks、Git 证据 | Goal 路由、写入集合隔离、合并顺序、共享验证和最终状态 |
| 主策划 Agent | Design/EN/01_MainPlanner.md | 产品意图、核心循环、范围和验收 |
| 系统策划 Agent | Design/EN/02_SystemPlanner.md、Rules/ | 系统语义、状态转换和规则契约 |
| 战斗/AI 策划 Agent | Design/EN/03_CombatAIPlanner.md | 战斗词汇、遭遇行为和统治者策略意图 |
| 关卡/内容策划 Agent | Design/EN/04_LevelContentPlanner.md | 沙盘图、节点内容、剧情变化和节奏 |
| 数值策划 Agent | Design/EN/05_NumericalPlanner.md | 系数、表格、曲线、夹具和数值证据 |
| 架构师 Agent | Architecture/ProgramArchitecture.md、DataDrivenArchitecture.md、ModuleMap.md | 程序边界、数据管线、依赖、持久化和扩展接口 |
| 模块 Agent | Architecture/Modules/<Module>/README.md | 模块实现、局部契约、测试和学习证据 |
| Review Agent | Architecture、Decisions、Production 和被修改的模块文档 | 跨文档一致性、依赖安全和验收证据 |
| 学习导师 Agent | Learning/Curriculum.md、Learning/Workflow.md 和任务学习契约 | 零基础定位、练习、证据、复述和迁移 |

英文文档是 Agent 使用的规范源。中文镜像用于用户阅读，不能独立成为设计权威。

## 文档依赖链

    设计意图
        -> 规则和系统契约
        -> 程序与数据架构
        -> 模块技术 README
        -> 生产任务
        -> 代码 / 资产 / 测试证据
        -> 更新 ProjectState

设计层回答做什么、为什么做。架构层回答怎么做、放在哪里、谁负责。模块 README 回答一个具体边界如何实现。任务文件记录一次可执行的变更和验收证据。

## 集成流程

1. 并行写入开始前，集成负责人在集成分支记录或批准任务契约。
2. 冻结下游 Goal 需要的共享契约。展开并行后若要改契约，必须回到串行集成工作。
3. 每个可独立合并的写入 Goal 使用一个 worktree，并分配一位 Primary Agent。
4. Primary Agent 可以委派有限分析或独占小补丁，然后验证自己的分支并生成完成报告。
5. 独立 Review Agent 检查真实 diff、验收证据、依赖、规则一致性和双语文档。
6. 集成负责人按依赖顺序合并，不能按“谁先做完”决定。
7. 在唯一构建/Editor 通道运行集成验证。集成缺陷应建立新的有限任务，或把原 Goal 退回 `In Progress`。
8. 更新 `Docs/ProjectState.md`，关闭任务，提交集成状态；只有用户授权后才推送。

交接不能依赖聊天摘要，而应使用提交哈希、任务完成报告、Review 结果、测试证据和明确的未决问题。

## 跨模块变更流程

1. 策划 Agent 改变规则含义或验收标准。
2. 更新英文策划/规则源和中文镜像。
3. 架构师 Agent 把变化映射到数据或运行时契约。
4. 每个受影响的模块 Agent 更新自己的 README、代码、数据和测试。
5. 结构性变化添加 ADR。
6. Review Agent 检查依赖、存档/网络影响和双语同步。
7. 集成负责人更新 ProjectState 并关闭任务。

模块 Agent 不应为了记录局部实现细节而修改总体架构基线。架构师 Agent 也不能仅为了实现方便而改变游戏规则。
