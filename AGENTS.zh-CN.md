# Dream of Padma Agent 项目约定

- 供用户阅读的中文镜像：`AGENTS.zh-CN.md`

## 项目目的

这是《Dream of Padma》的 UE5 C++ 学习和开发基础工程。之前的原型位于 `E:\2026ue\padma`，只能作为参考资料。除非任务明确要求，不得修改或批量复制原型内容。

## 阅读顺序

修改文件前，先阅读：

1. `Docs/00_INDEX.md`
2. `Docs/ProjectState.md`
3. `Docs/Production/Tasks/` 下的任务文件，或用户当前请求
4. 与目标目录相关的模块和目录约定

## 信息源

- 规则：`Docs/Rules/`
- 架构：`Docs/Architecture/`
- 生产状态：`Docs/ProjectState.md` 和 `Docs/Production/`
- 学习证据：`Docs/Learning/`
- 运行时代码：`Source/`
- UE 资源：`Content/Padma/`
- 外部资源：`Content/ThirdParty/`

如果代码、数据和设计文档不一致，应停止并报告冲突，不得默默创造规则。

## 文档语言约定

- 每个纳入版本管理的 Markdown 文档都必须有英文原文和中文镜像。
- 英文是 Agent 和实现决策的规范源。
- Agent 执行任务时读取相关英文文档；中文镜像只为用户阅读方便，不是独立信息源。
- 普通 Markdown 使用同目录配对：`Name.md` 和 `Name.zh-CN.md`。
- 设计草案使用现有配对：`Docs/Design/EN/Name.md` 和 `Docs/Design/ZH/Name.md`。
- 任何 Markdown 修改都必须在同一个任务和提交中同步修改配对文件，包括规则、状态、链接和错字修正。
- 如果用户通过中文镜像提出语义修改，应先反映到英文原文，再同步中文内容。
- 如果文档有文档 ID、版本和状态，两个版本必须保持一致且含义一致。

## 边界规则

- `PadmaCore` 不得依赖 UMG、Niagara、具体 Actor、地图或 `UGameplayStatics`。
- UI 发送命令，不能直接修改世界或玩法状态。
- 世界和玩法状态变化必须经过服务/命令，并产生事件。
- 卡牌、节点、遭遇、任务和存档记录使用稳定 ID。存档中不保存 Actor 指针。
- 需要复现的规则结果使用带种子的随机流。
- 不要把新的全局状态塞入万能 Manager。
- 不要原地编辑 `Content/ThirdParty/`；在 `Content/Padma/` 下创建项目包装资源。
- 不得修改生成目录：`Binaries/`、`DerivedDataCache/`、`Intermediate/`、`Saved/` 和 `.vscode/`。

## Agent 修改流程

1. 说明目标范围和受影响文件。
2. 做最小且完整的修改。
3. 运行最窄范围的验证脚本或测试。
4. 更新相关文档和 `Docs/ProjectState.md`。
5. 报告修改文件、检查结果、剩余风险和学习目标。

Markdown 发生变化时，配对的英文或中文文件必须在同一次操作中同步修改，并在报告中列出两个路径。
Markdown 修改后运行 `Scripts/AuditDocs.ps1`，确认没有缺失的语言镜像。

不要把无关重构和功能任务混在一起。没有明确迁移任务和重定向器检查，不要成批重命名或移动 UE 资源。

## 完成标准

- 满足验收标准。
- 相关测试或可复现的手工检查通过。
- 没有已知的新编译、反射、资源加载或打包错误。
- 文档和任务状态已更新。
- 每一个变更的 Markdown 都有同步的语言镜像。
- 生成文件和本地文件没有被提交。
