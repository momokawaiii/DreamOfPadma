# Codex 项目配置与角色

- 英文原文（Agent 阅读）：`Docs/Agent/CodexSetup.md`

## 目的

本仓库把 Codex 配置为一个可协调的开发环境，而不是一组依靠隐藏记忆共享上下文的对话。新的任务可以从版本管理文件中发现同一套项目契约、Role 配置和 Skill。

## 目录职责

| 位置 | 含义 | 应保存的内容 |
|---|---|---|
| `AGENTS.md` 和嵌套 `AGENTS.md` | 强制指令 | 安全、边界、阅读顺序、语言契约和目录专属规则 |
| `Docs/` | 会变化的项目事实 | 策划、规则、架构、ADR、任务契约、当前状态、Review 证据和学习证据 |
| `.codex/config.toml` | 项目多 Agent 设置 | 启用状态和保守并发默认值 |
| `.codex/agents/*.toml` | 项目自定义 Role 配置 | Role 名称、路由描述、核心指令和默认沙箱模式 |
| `.agents/skills/<skill>/SKILL.md` | 可复用操作流程 | 任务执行、Review 和学习的稳定步骤 |
| `Docs/Production/Tasks/` | Goal 契约 | 每个可观察交付 Goal 对应一对双语 `TASK-xxx` |
| Git 分支/worktree | 写入隔离与历史 | 一个可独立合并的写入 Goal 及其提交；受管理 worktree 在创建分支前处于 detached 状态 |

不要在 Role 文件或 Skill 中复制当前规则和进度。这些事实会变化，应该放在 `Docs/` 中；配置和 Skill 只负责把 Agent 指向规范文档。

## 项目 Agent 设置

`.codex/config.toml` 启用多 Agent、保留被中断时的上下文，并允许每个父会话最多存在四个被拉起的 subagent。项目不锁死模型或推理强度，因此每个任务可以继承当前受支持模型，或在任务级明确选择。

四个并发 subagent 是容量上限。项目初期最多同时使用两个可写 worktree，并且只有一条 UE 构建/Editor/PIE 通道。

Codex 会自动检测仓库 Skill 的变化；如果 Skill 没有出现，请重启 Codex。修改项目 Agent 配置后，在依赖新配置拉起会话之前，应重启或新建任务。

## 可用 Role 配置

| Codex 名称 | 默认权限 | 用途 | 禁止事项 |
|---|---|---|---|
| `chief_planner` | 只读 | 产品意图、核心循环、MVP 范围、玩家侧验收 | 解决延期规则或决定实现架构 |
| `system_planner` | 只读 | 系统语义、状态转换、不变量、卡牌、资源、时间和结局 | 发明缺失数值或混淆规则与实现 |
| `combat_ai_planner` | 只读 | Encounter/ACT 行为、时序、AI 意图和战斗路线场景 | 补全延期的行动优先级或卡牌生命周期 |
| `level_content_planner` | 只读 | 沙盘图、节点、遭遇、剧情变化和内容节奏 | 抢占共享地图/资源或发明未决设定 |
| `numerical_planner` | 只读 | 公式定义域、系数、固定样例、平衡假设和漏洞分析 | 把未测试数值当成定稿或覆盖固定公式 |
| `architect` | 只读 | 依赖、契约、数据流、存档/网络接口和 ADR Review | 为了实现方便改变游戏含义 |
| `module_worker` | 工作区可写 | 在一个独占写入集合中执行一项已批准任务 | 扩大范围、修改其他 Goal 所有文件、合并或推送 |
| `reviewer` | 只读 | 按任务、规则、架构、测试和文档进行独立 Review | 不检查证据就相信执行者总结，或在 Review 时编辑 |
| `learning_tutor` | 只读 | 零基础解释、预测、练习、证据、复述和迁移 | 代替学习者完成练习或扩大功能范围 |

内置 `explorer` 仍适合快速只读探索仓库。Role 配置用于描述专家能力，不代表永久模块负责人。

## 顶层任务、Primary Agent 与 subagent

一个顶层 Codex 任务是用户可见、可长期协作的空间。Goal 需要写出一个可独立合并的结果时，在同一项目中建立独立任务/worktree。开场提示词应包含：

```text
Primary Role: module_worker
Goal: <一个可观察结果>
Task contract: Docs/Production/Tasks/TASK-xxx-Name.md
Worktree：<项目任务开始时选择>
Branch：<结果需要长期保存或推送前，在任务标题栏创建>
Allowed paths: <确切写入集合>
Required checks: <测试/手工证据>
Learning targets: <一至两个目标>
Stop if: <规则、所有权或依赖停止条件>
```

指定 Primary Role 是告诉任务采用哪个专业视角；`TASK-xxx` 文件才是可执行契约。一位 Primary Agent 从预检到交接始终拥有这个 Goal。Codex 管理的 worktree 从 detached `HEAD` 开始；结果需要以分支保存或推送前，使用 **Create branch here**。

在任务内部，Primary Agent 可以按自定义 Role 拉起 subagent。委派提示词不能只有角色名：

```text
Role: architect
Work package: 检查拟议的日历契约是否跨越 PadmaCore 边界。
Inputs: TASK-xxx 以及任务点名的架构和模块文档。
Output: 带有确切文档引用的问题和最小建议。
Authority: 只读。
Non-goals: 不重新设计日历规则，不编辑文件。
Return before: 实现开始之前。
```

父 Agent 继续负责检查并采用结果。若一个结果需要自己的分支、验收标准、多个回合或由用户独立管理，应新建顶层任务，而不是用 subagent。

## 项目 Skill

- `$padma-task-runner`：预检、执行、验证、教学并交接一份任务契约。
- `$padma-review`：独立 Review 任务、分支、diff、策划或架构修改，先报告问题。
- `$padma-learning-loop`：把任务学习目标变成预测、练习、证据、复述和迁移。

Agent 可以从 `.agents/skills/` 发现这些 Skill；用户或父 Agent 也可以显式点名调用。英文 `SKILL.md` 是执行指令，`SKILL.zh-CN.md` 是同步的用户阅读镜像。

## 启动集成任务

在保存项目的直接 checkout 中使用一个任务作为集成协调者。它负责准备契约、创建或协调独立 worktree、跟踪结果、安排 Review、集成提交、运行共享验证并更新项目状态。它不应同时成为所有模块的默认实现者。

适合的开场提示词是：

```text
担任 Dream of Padma 集成协调者。阅读 AGENTS.md、Docs/00_INDEX.md、
Docs/ProjectState.md、Docs/Agent/Workflow.md 和当前集成任务。
不要在此 checkout 中实现模块工作。冻结任务契约，检测写入集合重叠，
把独立 Goal 路由到 worktree，委派有限只读专家 Review，只在证据齐全后合并，
并保持双语状态同步。
```

## 权限与安全规则

- 有意识地选择父任务权限模式；被拉起的 Agent 可能继承当前运行时权限覆盖。
- 策划、架构师、Review 和导师配置默认保持只读。
- 只有获得已批准确切写入集合的 worker 才能写入。
- 永远不要让多个 Agent 同时写同一文件或二进制 UE 资源。
- 推送远端、创建标签、发布、破坏性 Git 操作或实质扩大范围前必须获得用户授权。
- 用户仍是未决游戏设计的决定者，也是可玩结果的最终验收者。

## 官方机制参考

- [Codex subagent 与自定义 Agent](https://learn.chatgpt.com/docs/agent-configuration/subagents)
- [为 Codex 构建 Skill](https://learn.chatgpt.com/docs/build-skills)
- [Git worktree](https://learn.chatgpt.com/docs/environments/git-worktrees)
- [长期工作与 Goal](https://learn.chatgpt.com/docs/long-running-work)
