# Architecture Documentation Rules

- Follow ../../AGENTS.md and ../Agent/Workflow.md; no separate role or approval process here.
- Architecture describes representation, ownership and dependencies; Rules owns gameplay meaning. Separate static definitions from mutable state. Presentation consumes state and sends commands rather than settling gameplay.
- Read and update only affected contracts. Keep Core independent of world/UI presentation and cross-system access explicit. Record consequential structural changes in an ADR; a new UE module needs a real build, dependency or test reason.
- Module labels do not reserve files for specialist Agents. Preserve actual concurrent write assignments. Chinese summaries follow ../AGENTS.md.
