# 变更日志

- 英文原文（Agent 阅读）：`Docs/Changelog.md`
- 文档 ID：`CHANGELOG`
- 版本：`Unreleased`
- 状态：`Active`

本文档是已交付实现工作的简洁、面向用户的版本更新摘要。详细范围、验收证据、设计决策和未决问题仍记录在对应 TASK、ADR、模块文档和 `Docs/ProjectState.md` 中。

## Unreleased

### Added

- 2026-09-08 — **TASK-007 SLG 世界选择与切场**
  - 面向用户的结果：新增项目自有 Demo Sandbox，提供一个可选择地块，以及在同一 GameInstance/会话内跨场景保留的节点/场景/出生点强类型切场上下文。
  - 验证：修复后的 UE5.8 Editor 编译、TASK-007 切场上下文自动化测试、DemoSandbox 地图加载 smoke、用户完成的修复后 PIE 选择/高亮/取消/确认/无效 fixture 检查，以及独立 Review `Pass`。
  - 集成：在完成报告提交 `9ea99c7` 后完成本地收尾；未推送远端，未创建发布标签。
  - 范围边界：TASK-010 负责的默认地图配置已存在于独立的本地提交 `d8f107f` 中，未作为 TASK-007 的证据或内容纳入；Encounter 场景、专门的生命周期回归测试以及所有延期的正式世界规则不属于本次交付。

- 2026-09-07 — **双语实现变更日志流程**
  - 目标：要求每个修改代码、数据、UE 资源、配置、构建文件或测试的实现 Goal，都留下简洁的版本更新式摘要。
  - 流程：Primary Agent 提供草稿；Integration Coordinator 在任务进入 `Verified` 或 `Done` 前写入同步的英文和中文条目。
  - 验证：`Scripts/AuditDocs.ps1`、`Scripts/ValidateProject.ps1 -Strict` 和 `git diff --check`。
  - 运行时影响：无；没有改变任何玩法规则。
