# 变更日志

- 英文原文（Agent 阅读）：`Docs/Changelog.md`
- 文档 ID：`CHANGELOG`
- 版本：`Unreleased`
- 状态：`Active`

本文档是已交付实现工作的简洁、面向用户的版本更新摘要。详细范围、验收证据、设计决策和未决问题仍记录在对应 TASK、ADR、模块文档和 `Docs/ProjectState.md` 中。

## Unreleased

### Added

- 2026-09-07 — **双语实现变更日志流程**
  - 目标：要求每个修改代码、数据、UE 资源、配置、构建文件或测试的实现 Goal，都留下简洁的版本更新式摘要。
  - 流程：Primary Agent 提供草稿；Integration Coordinator 在任务进入 `Verified` 或 `Done` 前写入同步的英文和中文条目。
  - 验证：`Scripts/AuditDocs.ps1`、`Scripts/ValidateProject.ps1 -Strict` 和 `git diff --check`。
  - 运行时影响：无；没有改变任何玩法规则。
