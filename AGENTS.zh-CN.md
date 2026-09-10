# Dream of Padma Agent 约定

- 英文原文：AGENTS.md

## 入口与依据

- 默认使用当前 Local 工作区。旧 E:/2026ue/padma 原型只作只读参考。
- Primary 在任务首次实质工作时：把 Docs/00_INDEX.md 作为查找入口，读取 Docs/ProjectState.md、当前 TASK 或用户请求，再读相关目录指令与来源段落。上下文已有且未变化的内容不重复读取；恢复工作时核对 Git 和受影响文件。
- 用户决定优先于旧项目文字。保留已接受规则；真实的代码／数据／规则矛盾需要报告，等待相关决定时继续不受影响的工作。
- 规则在 Docs/Rules；架构在 Docs/Architecture 与 Docs/Decisions；当前状态在 Docs/ProjectState.md；任务证据在 Docs/Production/Tasks。实现与 UE 资产在 Source 和 Content/Padma。
- 常规执行按 Docs/Agent/Workflow.md。只有配置角色／工具时读取 CodexSetup；请求教学时读取学习材料。旧 TASK 的读取清单是查找索引，不表示必须加载所有链接；具体技术验收、检查和未决事项仍有效。

## 执行边界

- 一个 Primary 负责一项连贯交付；串行工作中，同一 Agent 可同时协调和实现。小补充留在同一 TASK。新建顶层聊天需要用户提出。
- 声明范围和准确允许路径，检查 Git，保留无关修改。文件、接口、地图和资产不得有重叠写入者。UE 构建／Editor／PIE 保持单通道。
- 只委派有独立价值的工作包。提供结果、来源路径／段落、准确读写权限与必要证据。子 Agent 按工作包及适用目录规则执行，不重复全项目开工流程，不关闭父任务。
- Review 与教学按 Workflow.md 的条件策略执行。它替代旧模板中通用的强制 reviewer／教学／worktree 套话，不替代具体技术验收和授权要求。
- 未经用户明确授权，不合并、强推、打标签或推送。不破坏性清理无关文件。不修改生成目录 Binaries、DerivedDataCache、Intermediate、Saved、.vscode。
- Content/ThirdParty 外部资产保持原样，项目包装放入 Content/Padma。

## 游戏架构不变量

- PadmaCore 不依赖 UMG、Niagara、具体 Actor、地图或 UGameplayStatics。
- UI 发出命令，服务负责校验、状态修改与事件；表现层不负责游戏结算。
- 卡牌、节点、遭遇、任务和存档使用稳定 ID；不把 Actor 指针写入存档。
- 可复现结果使用带种子的随机流；不新增万能 Manager 全局状态。
- 保持已接受的模式和数据边界。结构改变需要 ADR；现有边界内的常规实现无需另开 ADR。

## 文档与完成

- 英文为规范来源。每份 Markdown 配套中文：Name.md／Name.zh-CN.md，或既有 Design/EN／Design/ZH。ID、版本、状态、含义同步。Agent 读英文作决定；更新或核查翻译时才读中文。
- 范围、验收和证据在 TASK 记录一次。只更新事实变化的文档；ProjectState 是简短现状，不是工作流水账。
- 执行 Workflow.md 选定的局部检查；改 Markdown 后运行 AuditDocs.ps1。完成须满足请求行为和相关检查，未验证的运行表现如实报告。
- 最终交接：结果、修改路径、检查、剩余用户操作／风险。未请求教学时不例行提交学习报告。
