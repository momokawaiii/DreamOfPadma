# TASK-045 精简 Agent 工作流

- 英文原文：TASK-045-Lean-Agent-Workflow.md
- 文档 ID：TASK-045
- 版本：0.1
- 状态：Verified
- Primary：root，当前 Local codex/mvp-demo-foundations
- 授权：用户要求 UE 续作前精简流程以减少 token，并解释取舍。

## 结果与范围

使用单一主 Agent、有范围上下文、简短任务证据、按条件审查与可选教学。保留游戏含义、准确写入权限、有效验证、双语文档与 Git 授权。

允许路径（Markdown 均为中英配对）：根 AGENTS；Docs/Agent 的 Workflow、CodexSetup、WorkflowExample；Docs/Learning/Workflow；Docs/Production/TaskTemplate；Docs/ProjectState；Docs/Production/History/ProjectState-2026-09-09；Docs/00_INDEX；Docs/Changelog；本 TASK；三个既有 .agents/skills/padma-*/SKILL 配对；九个既有 .codex/agents/*.toml。保持 .codex/config.toml 与角色沙盒／模型设置不变。不改运行时、UE 资产、构建配置，不新增框架、安装依赖、提交、合并或推送。

仅一个只读 explorer 检查角色重复，root 同时准备文档，不再角色接力或广泛审查。root 是唯一写入者；将已有 ProjectState 全文保存在带日期快照中，再精简开工摘要。

## 验收

- [x] 开工不要求通读历史／任务／设计全库或两种语言。
- [x] 子工作包不接管整个 TASK，不重复父任务开工。
- [x] 常规 UI／文档不自动要求 reviewer 或教学练习。
- [x] 存档／回滚／确定性／GAS 所有权／公共契约仍保留定向测试与风险审查策略。
- [x] 当前状态记录 HTML 验收与用户自述学习，旧事实已保留。
- [x] 中英配对、TOML／Skill 语法和局部 diff 检查通过，既有运行代码／配置不变。

## 开销证据

比较八份英文入口／流程文件的规范化文本字符数：

| 路径 | 修改前 | 修改后 |
|---|---:|---:|
| AGENTS.md | 5144 | 3852 |
| Docs/ProjectState.md | 20655 | 4231 |
| Docs/Agent/Workflow.md | 11090 | 6053 |
| Docs/Agent/CodexSetup.md | 8119 | 3370 |
| Docs/Production/TaskTemplate.md | 3408 | 970 |
| .agents/skills/padma-task-runner/SKILL.md | 2848 | 1457 |
| .agents/skills/padma-review/SKILL.md | 2276 | 1106 |
| .agents/skills/padma-learning-loop/SKILL.md | 2037 | 871 |

合计 55577 → 21910，字符数减少 60.6%。这是可复现的文本体积比较，不是账单 token 实测，也不保证每任务节省同比例。历史归档而非删除。实际开销取决于读取文件、缓存、推理、工具、重试及子 Agent 数量。

## 证据与剩余工作

已应用 37 个范围内的流程文件。AuditDocs 通过（214 份 Markdown／107 对），ValidateProject -Strict 通过。9 个角色 TOML 解析通过，非指令设置保持不变。两份历史正文与修改前原文逐一核对，仅重定位相对链接。清除尾部空格后，范围内 git diff --check 通过。标准 Skill 校验器因缺少 PyYAML 无法启动；以无依赖严格检查验证 3 对 Skill 的双字段普通标量头部及名称／描述约束，没有安装依赖。按新指令核对了纯 UI、存档／GAS 生命周期及子工作包三种执行路径；不宣称已验证运行时行为。当前会话可能保留旧注入提示；用户新开任务后才能观察新角色加载。不宣称已热重载或已测得未来 token 节省。

学习解释：渐进式披露与委派开销。用户自述已学蓝图／数据／UI／GAS，不重新测验或强制练习。精简文字不放宽资源／权限约束。

来源：[官方自定义](https://learn.chatgpt.com/docs/customization/overview)、[子 Agent 与成本](https://learn.chatgpt.com/docs/agent-configuration/subagents)、当前仓库指令。
