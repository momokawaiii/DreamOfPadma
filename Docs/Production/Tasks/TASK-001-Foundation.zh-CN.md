# TASK-001 基础验证

- 英文原文（Agent 阅读）：`Docs/Production/Tasks/TASK-001-Foundation.md`

## 目标

在添加游戏规则前，验证新仓库可以在开发机器上复现。

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

## 验收标准

- [ ] `Scripts/ValidateProject.ps1` 通过。
- [ ] Git LFS 已安装并在仓库中生效。
- [ ] 空白项目能在记录的 UE 版本中编译。
- [ ] PIE 可以启动默认地图。
- [ ] Windows Development 打包完成，或准确记录阻塞原因。
- [ ] 没有生成目录被加入暂存区。

## 学习目标

- Git 和 Git LFS
- UE 项目与 Target 文件
- `Build.cs` 和 Unreal Automation 入口
- 可复现开发工作流

## 完成报告

在 `Docs/ProjectState.md` 中记录命令、引擎路径、构建结果、打包位置和环境特定阻塞。
