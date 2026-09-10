# Codex Setup and Roles

- Chinese companion: CodexSetup.zh-CN.md
- Read this for role/tool setup. Daily execution lives in Workflow.md.

## Responsibility split

AGENTS = stable constraints; ProjectState = current facts; TASK = one delivery; Role = a specialist viewpoint; Skill = a reusable procedure. Put each instruction in its owning layer and link rather than copy. English is canonical; Chinese companions are for user reading.

The current primary handles both coordination and serial implementation. A separate Integration Coordinator is useful only when multiple independent deliveries actually require integration. Do not open a chat or worktree simply to activate a Role.

## Available roles

| Role | Use |
|---|---|
| module_worker | A bounded patch with exact write authority |
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

Changed files are on disk; existing sessions may already contain older injected instructions. Follow the current user amendment in this task and use a fresh user-opened task to verify loading of changed profiles/Skills. Do not claim a hot reload without observing it.

## Skills

- padma-task-runner: execute/update a TASK; child packages do not repeat parent setup/closure.
- padma-review: requested or risk-triggered review; direct diff evidence, scoped recheck.
- padma-learning-loop: requested explanation/exercise; no automatic quiz from legacy learning fields.

Only load a Skill when its purpose matches the task. A selected Skill reads supporting material only when needed. Existing YAML UI metadata stays intact.

## References

These document mechanisms, not additional project approval gates:
- [Customization and progressive disclosure](https://learn.chatgpt.com/docs/customization/overview)
- [Subagents, costs and custom roles](https://learn.chatgpt.com/docs/agent-configuration/subagents)

Validated against official documentation on 2026-09-09. Schema validation does not prove that an already-running session reloaded a profile.
