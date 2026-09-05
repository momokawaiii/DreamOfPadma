# Dream of Padma 文档索引

- 英文原文（Agent 阅读）：`Docs/00_INDEX.md`

这是 Agent 和协作者进入文档体系的第一入口。Agent 根据当前任务只读取相关英文文档；中文文件用于用户阅读。

## 当前状态

- [项目状态](ProjectState.zh-CN.md)
- [Agent 工作流](Agent/Workflow.zh-CN.md)
- [Codex 项目配置与角色](Agent/CodexSetup.zh-CN.md)
- [Git 工作流](Agent/GitWorkflow.zh-CN.md)
- [多 Agent 端到端示例](Agent/WorkflowExample.zh-CN.md)
- [任务模板](Production/TaskTemplate.zh-CN.md)

## 设计规则

- [时间与日历](Rules/Time.zh-CN.md)
- [卡牌合成](Rules/Synthesis.zh-CN.md)
- [战斗](Rules/Combat.zh-CN.md)
- [世界状态](Rules/WorldState.zh-CN.md)

## MVP 策划基线（已验收）

这些文档中的高层 MVP 范围和规则理解已于 2026-09-02 通过用户验收，作为实现基线。明确标记为“开放”“提案”“待确认”、延期或仅供试玩验证的内容仍未决定，程序实现时不得自行补全。

- [主策划案](Design/ZH/01_MainPlanner.md)
- [系统策划案](Design/ZH/02_SystemPlanner.md)
- [战斗/AI 策划案](Design/ZH/03_CombatAIPlanner.md)
- [关卡/内容策划案](Design/ZH/04_LevelContentPlanner.md)
- [数值策划案](Design/ZH/05_NumericalPlanner.md)

## 架构

- [模块图](Architecture/ModuleMap.zh-CN.md)
- [程序架构基线](Architecture/ProgramArchitecture.zh-CN.md)
- [数据驱动架构](Architecture/DataDrivenArchitecture.zh-CN.md)
- [运行时流程](Architecture/RuntimeFlow.zh-CN.md)
- [存档模式](Architecture/SaveSchema.zh-CN.md)
- [设计模式指导](Architecture/DesignPatterns.zh-CN.md)
- [架构 Agent 规则](Architecture/AGENTS.zh-CN.md)
- [模块 Agent 规则](Architecture/Modules/AGENTS.zh-CN.md)

### 逻辑模块程序文档

- [PadmaCore](Architecture/Modules/PadmaCore/README.zh-CN.md)
- [PadmaGame](Architecture/Modules/PadmaGame/README.zh-CN.md)
- [PadmaGameplay](Architecture/Modules/PadmaGameplay/README.zh-CN.md)
- [PadmaWorld](Architecture/Modules/PadmaWorld/README.zh-CN.md)
- [PadmaUI](Architecture/Modules/PadmaUI/README.zh-CN.md)
- [PadmaEditor](Architecture/Modules/PadmaEditor/README.zh-CN.md)
- [PadmaTests](Architecture/Modules/PadmaTests/README.zh-CN.md)

## 生产与学习

- [路线图](Production/Roadmap.zh-CN.md)
- [构建矩阵](Production/BuildMatrix.zh-CN.md)
- [第一个基础任务](Production/Tasks/TASK-001-Foundation.zh-CN.md)
- [Agent 工作流初始化任务](Production/Tasks/TASK-002-Agent-Workflow-Bootstrap.zh-CN.md)
- [第一个核心规则契约切片](Production/Tasks/TASK-003-Core-Contracts.zh-CN.md)
- [学习课程](Learning/Curriculum.zh-CN.md)
- [开发学习闭环](Learning/Workflow.zh-CN.md)
- [GM 命令](Tools/GMCommands.zh-CN.md)

## Agent 自动化

- 项目自定义 Agent 配置：`.codex/config.toml` 和 `.codex/agents/*.toml`
- 任务执行流程：`.agents/skills/padma-task-runner/SKILL.md`
- 独立 Review 流程：`.agents/skills/padma-review/SKILL.md`
- 基于证据的学习流程：`.agents/skills/padma-learning-loop/SKILL.md`

## 资源

- [资源命名](Content/Naming.zh-CN.md)

## 决策记录

- [ADR-0001 项目结构](Decisions/ADR-0001-ProjectStructure.zh-CN.md)

策划基线的双语同步规则见 [Design/AGENTS.zh-CN.md](Design/AGENTS.zh-CN.md)。
