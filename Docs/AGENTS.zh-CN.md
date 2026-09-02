# 文档目录约定

- 供用户阅读的中文镜像：`Docs/AGENTS.zh-CN.md`

- 规范玩法规则放在 `Docs/Rules/`。
- 架构决策放在 `Docs/Architecture/` 和 `Docs/Decisions/`。
- 技术文档和模块 Agent 的所有权遵循 `Docs/Architecture/AGENTS.md` 与 `Docs/Architecture/Modules/AGENTS.md`。
- 每个任务在 `Docs/Production/Tasks/` 下使用一个独立文件。
- `Docs/Design/EN/` 和 `Docs/Design/ZH/` 在用户验收前都是设计草案；其中的开放项不能直接当成最终规则。
- 英文设计草案和中文镜像必须按照文档 ID 和版本同步。
- 每一个 Markdown 都必须有英文原文和中文镜像。Agent 执行任务时读取英文，中文只用于用户阅读。
- 普通文档使用同目录的 `Name.md` 和 `Name.zh-CN.md`；设计草案继续使用 `Docs/Design/EN/` 和 `Docs/Design/ZH/` 配对。
- 同一个任务和提交中必须同时修改 Markdown 配对文件，之后运行文档配对审计。
- 实现代码时不能悄悄改变规范规则；应更新规则文档，并在结构变化时增加 ADR。
- 保持 `Docs/ProjectState.md` 简短且及时。
- 新文档必须链接到 `Docs/00_INDEX.md`。
