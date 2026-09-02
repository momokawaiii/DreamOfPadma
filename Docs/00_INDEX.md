# Dream of Padma Documentation Index

- Chinese companion for user reading: `Docs/00_INDEX.zh-CN.md`

This file is the first documentation entry point for agents and collaborators. Read only the linked documents relevant to the current task.

## Current state

- [Project State](ProjectState.md)
- [Agent Workflow](Agent/Workflow.md)
- [Codex Project Setup and Roles](Agent/CodexSetup.md)
- [Git Workflow](Agent/GitWorkflow.md)
- [End-to-End Multi-Agent Example](Agent/WorkflowExample.md)
- [Task Template](Production/TaskTemplate.md)

## Design rules

- [Time and Calendar](Rules/Time.md)
- [Card Synthesis](Rules/Synthesis.md)
- [Combat](Rules/Combat.md)
- [World State](Rules/WorldState.md)

## MVP design baseline - accepted

The high-level MVP scope and rule interpretation in these documents were accepted on 2026-09-02 and form the implementation baseline. Items explicitly marked `Open`, `Proposed`, `To confirm`, deferred, or playtest-only remain unresolved and must not be invented during implementation.

### English - Agent-facing

- [Main Planner](Design/EN/01_MainPlanner.md)
- [System Planner](Design/EN/02_SystemPlanner.md)
- [Combat and AI Planner](Design/EN/03_CombatAIPlanner.md)
- [Level and Content Planner](Design/EN/04_LevelContentPlanner.md)
- [Numerical Planner](Design/EN/05_NumericalPlanner.md)

### Chinese - user-facing companions

- [主策划案](Design/ZH/01_MainPlanner.md)
- [系统策划案](Design/ZH/02_SystemPlanner.md)
- [战斗/AI策划案](Design/ZH/03_CombatAIPlanner.md)
- [关卡/内容策划案](Design/ZH/04_LevelContentPlanner.md)
- [数值策划案](Design/ZH/05_NumericalPlanner.md)

## Architecture

- [Module Map](Architecture/ModuleMap.md)
- [Program Architecture Baseline](Architecture/ProgramArchitecture.md)
- [Data-Driven Architecture](Architecture/DataDrivenArchitecture.md)
- [Runtime Flow](Architecture/RuntimeFlow.md)
- [Save Schema](Architecture/SaveSchema.md)
- [Design Pattern Guidance](Architecture/DesignPatterns.md)
- [Architecture Agent Rules](Architecture/AGENTS.md)
- [Module Agent Rules](Architecture/Modules/AGENTS.md)

### Logical module program documents

- [PadmaCore](Architecture/Modules/PadmaCore/README.md)
- [PadmaGame](Architecture/Modules/PadmaGame/README.md)
- [PadmaGameplay](Architecture/Modules/PadmaGameplay/README.md)
- [PadmaWorld](Architecture/Modules/PadmaWorld/README.md)
- [PadmaUI](Architecture/Modules/PadmaUI/README.md)
- [PadmaEditor](Architecture/Modules/PadmaEditor/README.md)
- [PadmaTests](Architecture/Modules/PadmaTests/README.md)

## Production and learning

- [Roadmap](Production/Roadmap.md)
- [Build Matrix](Production/BuildMatrix.md)
- [First Foundation Task](Production/Tasks/TASK-001-Foundation.md)
- [Agent Workflow Bootstrap Task](Production/Tasks/TASK-002-Agent-Workflow-Bootstrap.md)
- [Learning Curriculum](Learning/Curriculum.md)
- [Development Learning Loop](Learning/Workflow.md)
- [GM Commands](Tools/GMCommands.md)

## Agent automation

- Project custom-Agent configuration: `.codex/config.toml` and `.codex/agents/*.toml`
- Task execution procedure: `.agents/skills/padma-task-runner/SKILL.md`
- Independent review procedure: `.agents/skills/padma-review/SKILL.md`
- Evidence-based learning procedure: `.agents/skills/padma-learning-loop/SKILL.md`

## Content

- [Asset Naming](Content/Naming.md)

## Decision records

- [ADR-0001 Project Structure](Decisions/ADR-0001-ProjectStructure.md)

The design-baseline rules are documented in [Design Agent Rules](Design/AGENTS.md).

The Chinese documentation index is available at [00_INDEX.zh-CN.md](00_INDEX.zh-CN.md).
