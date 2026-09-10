# TASK-045 Lean Agent Workflow

- Chinese companion: TASK-045-Lean-Agent-Workflow.zh-CN.md
- Document ID: TASK-045
- Version: 0.1
- Status: Verified
- Primary: root, current Local codex/mvp-demo-foundations
- Authorization: user requests workflow simplification before UE continuation to reduce token use, with an explanation of the tradeoffs.

## Outcome and scope

Use one primary, scoped context, compact task evidence, conditional review and optional teaching. Preserve game meaning, exact write authority, meaningful verification, bilingual docs and Git permissions.

Allowed paths (English/Chinese pairs where Markdown): root AGENTS; Docs/Agent/Workflow, CodexSetup, WorkflowExample; Docs/Learning/Workflow; Docs/Production/TaskTemplate; Docs/ProjectState; Docs/Production/History/ProjectState-2026-09-09; Docs/00_INDEX; Docs/Changelog; this TASK; the three existing .agents/skills/padma-*/SKILL pairs; the nine existing .codex/agents/*.toml profiles. Keep .codex/config.toml and all role sandbox/model settings unchanged. No runtime, UE assets, build configuration, new framework, dependency installation, commit, merge or push.

A single read-only explorer examined role duplication while root prepared docs. No further role relay or broad review. Root is the only writer. Preserve pre-existing ProjectState content in a dated snapshot before replacing the startup summary.

## Acceptance

- [x] Startup does not require full historical/task/design reading or both languages.
- [x] Child package does not take over the entire TASK or repeat parent onboarding.
- [x] Routine UI/docs work requires no automatic reviewer or teaching exercise.
- [x] Save/rollback/determinism/GAS ownership/public-contract changes retain targeted tests and the risk review policy.
- [x] Current state records HTML acceptance and user-reported learning; archived facts are preserved.
- [x] Markdown pairs, TOML/skill syntax and scoped diff checks pass. Existing runtime code/configuration are unchanged.

## Cost evidence

Measured normalized text characters in eight English entry/procedure files, before and after this change:

| Path | Before | After |
|---|---:|---:|
| AGENTS.md | 5144 | 3852 |
| Docs/ProjectState.md | 20655 | 4231 |
| Docs/Agent/Workflow.md | 11090 | 6053 |
| Docs/Agent/CodexSetup.md | 8119 | 3370 |
| Docs/Production/TaskTemplate.md | 3408 | 970 |
| .agents/skills/padma-task-runner/SKILL.md | 2848 | 1457 |
| .agents/skills/padma-review/SKILL.md | 2276 | 1106 |
| .agents/skills/padma-learning-loop/SKILL.md | 2037 | 871 |

Total 55577 -> 21910, 60.6% fewer characters. This is a reproducible text-size comparison, not measured token billing or a guaranteed per-task saving. History was archived, not deleted. Actual use depends on loaded files, cache, reasoning, tools, retries and number of children.

## Evidence and remaining work

Applied 37 scoped workflow files. AuditDocs passed (214 Markdown files / 107 pairs); ValidateProject -Strict passed. Parsed all 9 role TOMLs and verified their non-instruction settings were preserved. Checked both archived state bodies against the complete pre-change originals with rebased links. Scoped git diff --check passed after removing trailing spaces. The standard Skill validator could not start because PyYAML is absent; a strict dependency-free check validated the exact two-field plain-scalar headers and name/description constraints in all 3 Skill pairs. No dependency was installed. Walked through UI-only, save/GAS lifecycle and child-package routing against the new instructions; no runtime behavior is claimed. Existing sessions may retain older injected prompts; a fresh user-opened task is needed to observe newly loaded profiles. No live agent reload or future token saving is claimed.

Learning explanation: progressive disclosure and delegation overhead. The user reports prior Blueprint/data/UI/GAS study; no retest or compulsory exercise. Preserve resource/permission constraints independently of shorter prose.

Sources: [official customization](https://learn.chatgpt.com/docs/customization/overview), [subagents and costs](https://learn.chatgpt.com/docs/agent-configuration/subagents), current repository instructions.
