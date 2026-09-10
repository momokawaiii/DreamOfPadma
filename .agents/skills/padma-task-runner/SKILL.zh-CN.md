---
name: padma-task-runner
description: 执行一个 Dream of Padma TASK 或明确委派的工作包。用于实现与交付，不用于只读解释。
---

# padma-task-runner

Primary：按 AGENTS.md 与 Docs/Agent/Workflow.md 的有范围读取和检查策略执行，已在上下文的指令不重读。读取现状、TASK 与受影响来源，核对 Git 并声明准确写入范围。记录当前用户授权，不因旧 TASK 为 Backlog 再问一次。只处理影响本次改动的未决项。

子 Agent：只做父 Agent 指定的工作包和路径。读适用目录规则及必要来源段落，不重复全项目开工、不另建 TASK、不关闭父任务或更新共享状态。

完成最小连贯结果，保留已接受规则和他人改动。执行相关检查；修改 Markdown 同步中英并运行 AuditDocs。原生代码变化需要适用 Editor 构建；结构／配置改变需要 ValidateProject -Strict。

独立审查只按 Workflow.md 或用户明确要求触发。教学可选，旧学习字段不能单独成为中断开发的理由。

TASK 中一次记录结果、路径、检查与剩余工作。串行 Primary 可兼任共享状态负责人，其他 worker 简短交接。Git 集成／远端操作保留明确授权要求。
