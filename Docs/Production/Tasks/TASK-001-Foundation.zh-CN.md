# TASK-001 基础验证

- 英文原文（Agent 阅读）：`Docs/Production/Tasks/TASK-001-Foundation.md`

## 目标

在添加游戏规则前，验证新仓库可以在开发机器上通过检查、完成编译并启动 PIE。根据产品负责人决定，打包延期到 MVP 完成之后。

## 首先阅读

- `AGENTS.md`
- `Docs/ProjectState.md`
- `Docs/Architecture/ModuleMap.md`
- `Docs/Production/BuildMatrix.md`

## 允许路径

- `Docs/ProjectState.md`
- `Build/Profiles/`
- `Scripts/`

## 不在范围内

- 玩法实现
- 旧原型迁移
- 批量导入资源
- 引擎版本升级
- Windows Development 打包

## 验收标准

- [x] `Scripts/ValidateProject.ps1` 通过。
- [x] Git LFS 已安装并在仓库中生效。
- [x] 空白项目能在记录的 UE 版本中编译。
- [ ] PIE 可以启动默认地图。
- [x] Windows Development 打包已明确延期到 MVP 完成之后，不作为 TASK-001 的门槛。
- [x] 没有生成目录被加入暂存区。

## 学习目标

- Git 和 Git LFS
- UE 项目与 Target 文件
- `Build.cs` 和编辑器构建入口
- 可复现开发工作流

Unreal Automation、Cook、Stage、打包和独立构建验证属于 MVP 完成后的学习目标。

## 完成报告

在 `Docs/ProjectState.md` 中记录命令、引擎路径、构建结果、PIE 结果和环境特定阻塞。
