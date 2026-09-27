# 当前 UE 验证方案

- 文档 ID：TESTPLAN-CURRENT-001
- 版本：1.0
- 更新：2026-09-27
- 状态：只覆盖保留的陈 ACT、回合制地图和 PadmaNPR

当前入口只有三张地图和一个 NPR 插件：

- `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld`
- `/Game/Padma/MVP/Playable/Maps/L_PadmaBattle`
- `/Game/Sandbox/ACT/Training/Maps/L_ChenACT`
- `Plugins/PadmaNPR`

旧 HTML、独立模型预览地图、独立地图编辑地图和通用 ACT 模板已经退休，不再作为测试入口。`MVP/Playable` 的名字是历史目录名，里面保留的是当前回合制实现。

整理后运行：

```powershell
pwsh -NoProfile -File .\Scripts\AuditDocs.ps1
pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict
pwsh -NoProfile -File .\Scripts\AuditAssets.ps1
node .\Scripts\Editor\ExportPlayableData.cjs --check
```

仓库检查不能代替 PIE、GPU 画面、打包、干净安装和完整试玩。陈动作、视觉还原和最终打包仍需单独留证据。

详细依据见[英文原文](CurrentBuildTestPlan.md)。
