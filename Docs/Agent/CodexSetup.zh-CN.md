# Codex 配置与角色

- 英文原文：CodexSetup.md
- 仅配置角色／工具时读取；日常执行流程见 Workflow.md。

## 分工

AGENTS＝稳定约束；ProjectState＝当前事实；TASK＝一次交付；Role＝专家视角；Skill＝重复操作流程。每条指令放在负责它的层，只链接、不复制。英文为规范，中文方便用户阅读。

当前主 Agent 可同时协调与串行实现。实际有多个独立交付需要集成时，再安排单独的集成协调者。不为激活角色而新开聊天或 worktree。

## 可用角色

| Role | 用途 |
|---|---|
| module_worker | 准确写入权限内的有界补丁 |
| explorer | 具体的仓库问题 |
| reviewer | 按 Workflow.md 的有范围正确性／风险审查 |
| architect | 真实的依赖、契约、生命周期或持久化问题 |
| chief_planner | 尚未明确的产品范围／验收 |
| system_planner | 尚未明确的游戏规则含义 |
| combat_ai_planner | Encounter／ACT 时间线或 AI 语义 |
| level_content_planner | 节点、遭遇、剧情与节奏内容 |
| numerical_planner | 配置数值、曲线与可复现平衡证据 |
| learning_tutor | 用户请求的讲解或练习 |

保持现有自定义角色名称和沙盒默认值：module_worker 为 workspace-write；策划、architect、reviewer、tutor 为 read-only。实际操作仍受运行时权限约束。角色是可用专长，不是每次任务都要启动的常驻团队。

## 配置

.codex/config.toml 当前启用 agents，上限为不含 Primary 的四个子线程，并保留中断消息。TASK-045 不改变容量、默认模型、推理强度或权限。用子任务的实际价值控制委派，不安排固定角色接力。

有界子任务避免复制完整聊天历史，使用 Workflow.md 的短工作包和明确来源。更多 Agent 可节省耗时，但会增加总 token；增加并发前先看收益。

文件已落盘，但当前会话可能已注入旧指令。本任务遵循用户当前修订；用户以后新开任务时再验证新 Role／Skill 的加载。未观察到热重载就不宣称已热重载。

## Skills

- padma-task-runner：执行／更新 TASK；子工作包不重复父任务开工与收尾。
- padma-review：按请求或风险触发；核对实际 diff，只复查受影响部分。
- padma-learning-loop：按需讲解／练习；旧学习字段不自动触发测验。

仅在用途匹配时加载 Skill；支持材料也按需读取。现有 YAML 界面元数据保持不变。

## 参考

以下说明工具机制，不新增项目审批门槛：
- [自定义与渐进式披露](https://learn.chatgpt.com/docs/customization/overview)
- [子 Agent、成本与自定义角色](https://learn.chatgpt.com/docs/agent-configuration/subagents)

2026-09-09 核对官方文档。语法有效不表示正在运行的会话已重载角色。
