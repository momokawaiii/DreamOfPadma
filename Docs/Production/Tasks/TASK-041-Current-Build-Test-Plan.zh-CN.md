# TASK-041 当前版本测试方案

- 英文原文：TASK-041-Current-Build-Test-Plan.md
- 文档 ID：TASK-041
- 版本：0.1
- 状态：Verified
- Primary Agent / Role：root，唯一文档写入者
- 分支：Local codex/mvp-demo-foundations；不新增 worktree
- 授权：用户于 2026-09-09 要求现有 UE 与 HTML 功能的测试方案。

## 目标与范围

交付当前 UE 候选与既有 HTML 草图的可复现双语测试方案，明确启动入口、准备／恢复、用例 ID、步骤、预期结果、证据和严重程度。区分现有行为、已知规则差异、未配置样例与未来功能。此次仅授权方案文档，不修改玩法，也不产生新一轮运行测试结论。

读取 AGENTS.md、Docs/00_INDEX.md、ProjectState、TASK-007／008／036／040、ModelAuthoring、ACTAuthoring、相关实现／测试及两份本地 HTML。以用户最新决定为准。

## 允许路径

- `Docs/Production/CurrentBuildTestPlan.md`
- `Docs/Production/CurrentBuildTestPlan.zh-CN.md`
- `Docs/Production/Tasks/TASK-041-Current-Build-Test-Plan.md`
- `Docs/Production/Tasks/TASK-041-Current-Build-Test-Plan.zh-CN.md`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`

当前代码、地图、资产、脚本、HTML、配置与无关工作区变更均只读。没有重叠写入者，不委派子 Agent。本方案交付不运行 UE／浏览器、不构建、不创建测试资产、不提交／合并／推送。

## 验收与验证

- [x] UE／HTML 入口和实际操作方式有代码／资产依据。
- [x] 覆盖手动冒烟、失败／恢复、模型／ACT 配置及既有自动化。
- [x] 明确 HTML 已知差异与未来玩法，不将历史证据当作本轮测试结果。
- [x] 每项用例有 ID、起点、操作、预期结果和证据约定。
- [x] 双语文档、索引链接、任务状态及范围 Markdown 检查一致。

执行 AuditDocs.ps1、ValidateProject.ps1 -Strict、范围差异／空白／链接检查、用例 ID 配对及只读源码核对。测试方案包含可选实操观察，不宣称用户学习或玩法验收完成。

## 完成报告

- 状态：Verified，仅指测试方案交付；按方案执行仍待完成。
- 修改路径：上方六份允许的 Markdown，含同步方案与任务配对、两个索引。方案见 [CurrentBuildTestPlan](../CurrentBuildTestPlan.zh-CN.md)。
- 覆盖：16 项 UE 手动用例、13 项 HTML 用例、六项既有项目自动化完整名称、四项已知差异、启动／恢复步骤、可选隔离样例及结果／严重程度记录。
- 来源核对：准确 UE 操作、样例恢复时保留选择的行为、当前模型／技能归属、六项测试注册、UE5.8.2 Build.version 和两个 HTML SHA-256；独立包装页含完全一致的源片段。
- 验证：AuditDocs（202 份 Markdown／101 对）、ValidateProject -Strict、范围 git diff --check 通过；自定义用例 ID／顺序、自动化名称、空白和本地链接检查通过；两段 PowerShell 示例语法解析通过。
- 方案调整：文件系统拒绝创建 Docs/Testing，故改用既有 Docs/Production 目录，无须新增权限或修改配置。
- 复核边界：root 完成文档／源码检查，不宣称独立玩法验收或新运行时测试结论。
- 本文档任务未执行运行时测试、未收集用户实测证据；此前六项测试通过保持为历史证据。
- 未修改玩法、资产、HTML、脚本或配置，未提交／合并／推送。
