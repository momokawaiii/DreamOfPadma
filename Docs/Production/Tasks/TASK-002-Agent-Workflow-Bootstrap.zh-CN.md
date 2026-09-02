# TASK-002 Agent 工作流初始化

- 英文原文（Agent 阅读）：`TASK-002-Agent-Workflow-Bootstrap.md`
- 状态：`Done`
- 上级里程碑或集成 Goal：M0 基础与协作就绪
- Primary Role：集成协调者
- Primary Agent：当前文档任务
- 分支/worktree：`main`

## Goal

通过项目级 Codex 配置、可复用 Skill、双语操作文档和可验证任务契约，让已经认可的 Goal/worktree/Primary Agent/subagent/Role/Skill 模型能够直接执行。

## 为什么现在做

已冻结的 MVP 基线可以进入实现，但在所有权、写入隔离、Review、学习证据和 Git 交接被明确之前，不应开始并行实现。

## 首先阅读

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Architecture/ModuleMap.md`

## 决策状态

### 已冻结输入

- 仓库是共享记忆；隐藏聊天历史没有权威性。
- 一个可独立合并的写入 Goal 只有一个 Primary Agent，通常对应一份任务契约和一个 worktree。
- subagent 必须得到 Role 和一个有边界的工作包。
- 稳定、可重复的流程沉淀为 Skill；会变化的游戏事实留在 `Docs/`。
- 每个纳入版本管理的 Markdown 都有同步的英文/中文配对。

### 延期或开放输入

- 尚未分配逻辑模块的永久人类或 Agent 负责人。
- 并发上限可在取得真实集成证据后调整；初始策略保持保守。
- 本任务不包含玩法实现决定。

## 范围

### 允许路径和写入集合

- `.codex/`
- `.agents/skills/`
- `.gitattributes`
- `AGENTS.md` 和 `AGENTS.zh-CN.md`
- `README.md` 和 `README.zh-CN.md`
- `Docs/00_INDEX.md` 和 `Docs/00_INDEX.zh-CN.md`
- `Docs/Agent/`
- `Docs/Learning/`
- `Docs/Production/TaskTemplate*`
- 本任务配对和 `Docs/ProjectState*`

### 不在范围内

- UE C++、蓝图、地图、`.uasset`、运行时配置和玩法数据
- 修改已冻结 MVP 策划，或解决延期的游戏规则
- 打包或发布工作
- 创建第一个实现任务

## 依赖与集成顺序

- 依赖已验收的 MVP 策划/架构基线和私有 Git 远端基线。
- 必须先集成本任务，再创建并行实现 Goal。
- 当前没有其他写入者负责本任务的文档/配置写入集合。

## 委派计划

- 官方 Codex 文档定义当前配置、Skill、worktree 和 Goal 契约。
- `skill-creator` 流程初始化了三个项目 Skill。
- 提交前尝试了两次有边界的只读 reviewer 运行。两者都没有在有限等待时间内返回问题，因此已关闭，不能作为验收证据。集成协调者依据官方文档完成真实 diff Review；这个只适用于工作流初始化的 Review 例外不得复制到实现任务。

## 验收标准

- [x] `.codex/` 下存在项目级多 Agent 设置。
- [x] 五类策划、架构师、模块执行者、Review 和学习导师配置都有明确边界。
- [x] `.agents/skills/` 下存在任务执行、Review 和学习 Skill。
- [x] Goal、任务、worktree、Primary Agent、Role、subagent、Skill、Review、集成和 Git 的职责都有双语说明。
- [x] 任务模板能记录写入集合、委派、验证、风险和用户学习证据。
- [x] 存在一个具体但不处于活动状态的并行开发示例。
- [x] 未修改任何 UE 源码或资源文件。

## 验证方案

### 自动检查

- 为三个 Skill 调用 `skill-creator` 校验器。如果其运行时依赖不可用，应记录限制并执行同等的 frontmatter/名称/描述/TODO/目录结构检查，不能声称原校验器通过。
- 解析所有 `.codex/*.toml` 文件。
- 运行 `Scripts/AuditDocs.ps1`。
- 运行 `Scripts/ValidateProject.ps1 -Strict`。
- 运行 `git diff --check` 并检查修改路径。

### 手工检查

- 确认英文文档形成从项目入口、任务、执行、Review、集成、学习到推送的完整 Agent 路径。
- 确认每一份新增的用户说明都有中文镜像。

### 必需证据

- 完成报告中记录通过的命令输出和明确的修改路径审计。

## 学习契约

| 目标 | 所需证据 | 状态 |
|---|---|---|
| 区分 Role、Goal、任务、worktree、subagent 和 Skill | 用户能把一个示例功能分配到正确单元 | `Introduced` |
| 使用安全的 Git 上传闭环 | 用户亲自完成基线后的第一次 status/stage/review/commit/push | `Introduced` |

- 学习者亲手练习：Review 本任务提交后，执行文档中的上传闭环。
- 迁移问题：判断未来的卡牌 UI 调研应该是只读 subagent 工作包、独立写入 Goal，还是串行集成工作。

## 风险与恢复

- Codex 自定义 Agent 配置可能继续演化；修改其模式前应重新核对官方文档。
- 即使文件分开，过高并发仍可能造成语义冲突，因此合并顺序仍是强制要求。
- 本次只有文档和项目本地配置，可通过 Git 恢复。

## 完成报告

- 最终状态：`Done`
- Primary Agent 和 Role：当前 Agent，集成协调者
- 修改文件：项目 Agent 配置、三个 Skill、双语 Agent/Git/学习/任务文档以及状态/索引契约
- 验收证据：已生成结构，并满足上述全部验收项
- 已运行检查及结果：100 个文件/50 对 Markdown 配对审计通过；严格项目验证通过；10 个 TOML 全部解析，8 个只读/1 个写入 Role 策略通过；三个 Skill 通过同等结构检查；本地 Markdown 链接通过；`git diff --check` 和无 UE 路径范围审计通过
- 未运行检查及原因：已调用提供的 `quick_validate.py`，但离线捆绑 Python 无法导入 `PyYAML`，且网络安装被阻止；UE 编译、PIE 和打包不受影响，并且不属于本纯文档任务范围
- 已解决或接受的 Review 问题：协调者 Review 修正了“Codex 管理的 worktree 自动拥有分支”这一错误说法；官方文档确认其初始处于 detached `HEAD`。独立 reviewer subagent 没有产出并已关闭，因此本初始化任务不声称独立 Review 通过
- 剩余风险和未决问题：第一个实现周期必须真正完成一次有边界的独立 Review；Role/并发调整和 subagent 响应性都需要真实证据
- 已更新的中英文文档：是
- Agent 产生的学习证据：可执行工作流、模板和示例
- 用户产生的学习证据：等待第一个真实任务和 Git 推送练习
- 集成提交或交接引用：本任务的文档基线提交
