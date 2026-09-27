# Codex Setup and Roles

- Chinese companion: CodexSetup.zh-CN.md
- Read this for role/tool setup. Daily execution lives in Workflow.md.

## Responsibility split

AGENTS = stable constraints; ProjectState = current facts; TASK = an optional durable handoff; Role = a specialist viewpoint. Put each instruction in its owning layer and link rather than copy. English is canonical; Chinese companions are short plain-language summaries; internal tasks need no new companion. Workflow.md overrides legacy role wording that suggests mandatory document updates.

The current Primary owns coordination, implementation methods and exploration under Workflow.md. It may make technical decisions and maintain affected central/module documents within the authorized outcome; specialist role names are not exclusive editing or approval offices. A separate Integration Coordinator is useful only when independent deliveries need coordination. Do not open a chat or worktree simply to activate a Role.

## Available roles

| Role | Use |
|---|---|
| module_worker | Primary delivery autonomy, or an exact delegated child write set |
| explorer | A specific repository question |
| reviewer | A scoped correctness/risk review under Workflow.md |
| architect | A real dependency, contract, lifecycle or persistence question |
| chief_planner | Unresolved product scope/acceptance |
| system_planner | Unresolved game-rule semantics |
| combat_ai_planner | Encounter/ACT timeline or AI semantics |
| level_content_planner | Node, encounter, story or pacing content |
| numerical_planner | Authored numbers, curves and reproducible balance evidence |
| learning_tutor | A user-requested lesson or exercise |

Project custom profiles keep their existing names and sandbox defaults: module_worker is workspace-write; planners, architect, reviewer and tutor are read-only. Runtime permissions still govern actions. A Role is available expertise, not a standing team to invoke on every task.

## Configuration

.codex/config.toml currently enables agents, sets a capacity of four spawned threads excluding the Primary, and keeps interruption messages. TASK-045 does not change this capacity, model defaults, reasoning effort or permissions. Limit routine delegation by usefulness rather than configuring a fixed role relay.

Avoid copying full conversation history to a bounded child. Use the compact work-package prompt in Workflow.md and explicit source references. More agents can save elapsed time while increasing total token use; measure before adding concurrency.

Changed files are on disk; existing sessions may already contain older injected instructions or removed Skill entries. Follow the user's current decision rather than restoring retired entries. Do not claim a hot reload without observing it.

## Direct execution and learning

Routine delivery, review and teaching need no project Skill wrapper. Use [Workflow](Workflow.md) for autonomy and review, the retained role profiles when useful, and [Learning Workflow](../Learning/Workflow.md) for a requested lesson. Historical TASK references to the three retired project Skills are evidence, not current invocation instructions. Other installed Skills remain available; choose them for concrete task value, not merely because a request involves editing files.

## References

These document mechanisms, not additional project approval gates:
- [Customization and progressive disclosure](https://learn.chatgpt.com/docs/customization/overview)
- [Subagents, costs and custom roles](https://learn.chatgpt.com/docs/agent-configuration/subagents)

Validated against official documentation on 2026-09-09. Schema validation does not prove that an already-running session reloaded a profile.
