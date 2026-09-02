# Dream of Padma

- 英文原文（Agent 阅读）：`README.md`

《Dream of Padma》是一个 UE5 C++ 学习与开发项目，重点是数据驱动的世界模拟、卡牌合成、战术战斗，以及 RTS/ACT/FPS 表现模式的切换。

## 当前状态

当前仓库包含空白 C++ UE 工程、已经验收的高层 MVP 策划与架构基线，以及带有 Role 配置、任务/Review/学习 Skill、worktree 隔离、双语任务契约和 Git 交接规则的项目级多 Agent 工作流。明确延期的参数仍保持开放。下一个门槛是尚未完成的 PIE 基础检查，然后创建并批准 Core 契约任务，再进入 M1 规则切片。

## 仓库规则

- 开始工作前阅读 `AGENTS.md`。
- 阅读 `Docs/00_INDEX.md`，定位当前设计和架构文档。
- 协调多个 Agent 或 worktree 前，阅读 `Docs/Agent/CodexSetup.md` 和 `Docs/Agent/Workflow.md`。
- 按 `Docs/Agent/GitWorkflow.md` 执行暂存、提交、集成和 GitHub 备份。
- 使用 `Scripts/ValidateProject.ps1` 做快速仓库检查。
- Markdown 修改后使用 `Scripts/AuditDocs.ps1` 检查英文/中文配对。
- 只有配置了 Unreal Engine 根目录后，才使用 `Scripts/RunTests.ps1` 和 `Scripts/PackageDevelopment.ps1`。
- 第三方资源与项目自有资源分开保存。
- 每个 Markdown 都有英文原文和同步中文镜像；Agent 读取英文原文。

## 初始验证

```powershell
Scripts\ValidateProject.ps1
```

项目当前使用 `DreamOfPadma.uproject` 中记录的引擎版本。引擎升级必须在 `Docs/Decisions/` 中记录 ADR。
