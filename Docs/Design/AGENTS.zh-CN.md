# 策划基线约定

- 供用户阅读的中文镜像：`Docs/Design/AGENTS.zh-CN.md`

- `Docs/Design/EN/` 存放面向 Agent 的英文策划基线。
- `Docs/Design/ZH/` 存放供用户评审的同步中文镜像。
- Agent 执行任务时读取英文策划基线；中文文件只为用户阅读方便。
- 五份 MVP 文档中的高层范围和规则理解已于 2026-09-02 通过用户验收，是实现基线。
- 每一对语言文件保持相同的文档 ID、版本、状态和含义。
- 规则或范围发生变化时，在同一修改中同步更新英文和中文。
- 标记为“Open”“Proposed”或“To confirm”的内容不能直接实现为最终规则。
- 如果与 `mygame.pdf`、`mycard.pdf` 或用户后续澄清冲突，应先记录并解决评审意见，再修改规范规则文档。
- 已验收基线发生变化时，更新 `Docs/Rules/`、`Docs/Architecture/` 和 `Docs/ProjectState.md`；结构性决定需要创建 ADR。
- 新设计文档必须链接到 `Docs/00_INDEX.md`。
