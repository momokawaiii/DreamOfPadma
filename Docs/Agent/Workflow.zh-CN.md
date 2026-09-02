# Agent 工作流

- 英文原文（Agent 阅读）：`Docs/Agent/Workflow.md`

## 共享上下文

Codex 对话是工作单元；仓库才是共享事实来源。Agent 不应依赖隐藏的聊天历史，而应使用 Git、任务文件、`ProjectState.md`、ADR 和测试结果沟通。

## 工作拆分

并行工作应使用互不重叠的所有权：

- 核心规则和测试
- 玩法和战斗
- 世界和地图
- UI 和表现
- 工具、构建和文档

不要让多个 Agent 同时编辑同一张 UE 地图、`.uasset`、中心配置文件或公共数据模式；如确有需要，建立独立集成任务。

## 任务生命周期

```text
Backlog -> Ready -> In Progress -> Review -> Verified -> Done
```

每个任务都需要目标、允许路径、非目标、验收标准、测试方案和学习目标。

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
| 主策划 Agent | Design/EN/01_MainPlanner.md | 产品意图、核心循环、范围和验收 |
| 系统策划 Agent | Design/EN/02_SystemPlanner.md、Rules/ | 系统语义、状态转换和规则契约 |
| 战斗/AI 策划 Agent | Design/EN/03_CombatAIPlanner.md | 战斗词汇、遭遇行为和统治者策略意图 |
| 关卡/内容策划 Agent | Design/EN/04_LevelContentPlanner.md | 沙盘图、节点内容、剧情变化和节奏 |
| 数值策划 Agent | Design/EN/05_NumericalPlanner.md | 系数、表格、曲线、夹具和数值证据 |
| 架构师 Agent | Architecture/ProgramArchitecture.md、DataDrivenArchitecture.md、ModuleMap.md | 程序边界、数据管线、依赖、持久化和扩展接口 |
| 模块 Agent | Architecture/Modules/<Module>/README.md | 模块实现、局部契约、测试和学习证据 |
| Review Agent | Architecture、Decisions、Production 和被修改的模块文档 | 跨文档一致性、依赖安全和验收证据 |

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

## 跨模块变更流程

1. 策划 Agent 改变规则含义或验收标准。
2. 更新英文策划/规则源和中文镜像。
3. 架构师 Agent 把变化映射到数据或运行时契约。
4. 每个受影响的模块 Agent 更新自己的 README、代码、数据和测试。
5. 结构性变化添加 ADR。
6. Review Agent 检查依赖、存档/网络影响和双语同步。
7. 集成负责人更新 ProjectState 并关闭任务。

模块 Agent 不应为了记录局部实现细节而修改总体架构基线。架构师 Agent 也不能仅为了实现方便而改变游戏规则。
