# Project State

- Chinese companion for user reading: `Docs/ProjectState.zh-CN.md`

## Baseline

- Project: DreamOfPadma
- Engine association: see `DreamOfPadma.uproject`; do not change it without an ADR.
- Current milestone: MVP high-level baseline frozen / Agent workflow ready / PIE foundation verified / future ACT architecture boundary accepted / M1 Core contract complete / ADR-0003 accepted / TASK-006 cluster complete / TASK-007 ready for implementation
- Runtime modules: the generated `DreamOfPadma` module only
- Git repository: `main` tracks the private GitHub `origin/main`
- Git LFS: initialized locally

## Completed

- Blank C++ UE project created.
- Initial repository layout added.
- Agent contract and documentation index added.
- Git ignore and Git LFS attributes added.
- Git repository and local Git LFS hooks initialized.
- Validation, asset-audit, test, and packaging script entry points added.
- Five MVP design baseline documents created in English for Agents and Chinese for user review.
- The baseline documents were checked against `mygame.pdf`, `mycard.pdf`, and the latest project clarifications.
- UE5.8 Target settings updated and the DreamOfPadmaEditor target compiled successfully.
- Program architecture, data-driven architecture, architecture Agent rules, and module Agent rules added.
- Logical module documentation directories and bilingual module README skeletons added for PadmaCore, PadmaGame, PadmaGameplay, PadmaWorld, PadmaUI, PadmaEditor, and PadmaTests.
- Battle-mode clarification recorded: Encounter is a condition-driven turn-based action timeline with initiative, priority, reactions, interrupts, extra actions, and non-turn windows; RealTimeAction is the ACT/FPS/other real-time route with ACT as the first MVP mode; basic non-A cards are battle-only; ABC-card active abilities are Sandbox-available by default with per-ability restrictions.
- Interaction clarification recorded: Encounter hand and RealTimeAction slots are one-to-one through the shared `Tab` card repository; ACT MVP `Tab` opens a blurred five-card panel, sets world time to 1/10, and rejects movement/attack input while accepting only basic non-A card input; slots are 1-5 per page and the mouse wheel requests the next page. Total page count is deferred.
- Encounter trigger clarification recorded: when an ABC-card-controlled character or unit moves onto a point where an enemy is staying, the point triggers an Onmyoji-like turn-based Encounter; the resolved battle route can select/trigger Encounter, ACT, or a future RealTimeAction presentation and story consequence.
- Battle transaction clarification recorded: local battles may be entered at any calendar phase, pause sandbox time, record a complete versioned snapshot of all battle-mutated run state, commit on success, and restore that snapshot exactly on defeat or exit.
- MVP outcome clarification recorded: the two MVP victories are ruler-core life reaching zero and war balance reaching the player victory boundary; the outcome contract remains extensible for the other documented victory and defeat methods.
- Synthesis clarification recorded: the documented layered formula remains the source; synthesis failure grants no reward, and resource loops require authored cost/risk while allowing luck to compensate in some cases.
- User accepted the high-level MVP design and architecture baseline on 2026-09-02. Explicitly open, proposed, deferred, and playtest-only parameters remain unresolved by design.
- The private GitHub origin and SSH-over-443 authentication were verified from the user's normal terminal.
- Baseline commit `cbb4da4` and annotated tag `mvp-baseline-v0.1.0` were published to the private origin.
- Project-scoped Codex multi-Agent settings and nine bounded Role profiles were added under `.codex/`; planner, architect, reviewer, and tutor profiles are read-only by default, while `module_worker` is the scoped writer.
- Three project Skills now define repeatable task execution, independent review, and evidence-based learning under `.agents/skills/`.
- `TASK-001 Foundation Verification` is complete; the user-provided manual evidence confirms that the default map launches in PIE on 2026-09-03.
- The approved hierarchy is documented: one independently mergeable writable Goal normally has one task contract, one worktree, and one Primary Agent; subagents receive bounded work packages; Skills store procedure rather than mutable project truth.
- Bilingual operating documents now cover Codex setup, conservative parallelism, integration order, Git/GitHub backup, a complete M1 example, and the novice learning loop.
- `TASK-002 Agent Workflow Bootstrap` is complete, and the task template now records decision state, exact write set, delegation, integration order, verification, recovery, and separate Agent/user learning evidence.
- `TASK-003 Core Rule Contract Slice` is complete after independent read-only review and user approval on 2026-09-07; it freezes the design-only Core contract and downstream test matrix but authorizes no runtime implementation.
- `TASK-004 ACT Architecture Decision Integration` records ADR-0002 and the Future ACT Development Agent Contract as an accepted documentation-only boundary: Encounter remains first, ACT requires a separate approved task, built-in GAS requires an approved dependency change, and external gameplay plugins require another ADR.
- `ADR-0003 Fixed Playable Prototype Vertical Slice` was accepted and `TASK-006 Playable Prototype Vertical Slice Cluster` was completed on 2026-09-07; `TASK-007` is now `Ready` for a new implementation session, while TASK-008 through TASK-010 remain separately gated and no implementation Agent has started.

## Next tasks

1. `ADR-0003` is `Accepted` and the bilingual [TASK-006 Playable Prototype Vertical Slice Cluster](Production/Tasks/TASK-006-Playable-Prototype-Cluster.md) is `Done`; `TASK-007` is `Ready` and may start a new implementation session. TASK-008 through TASK-010 remain `Review` and serially gated.
2. Start the `TASK-007` implementation on Local branch `feature/TASK-007-world-selection-transition`; after its independent Review and integration, proceed serially to TASK-008 Encounter character/skill runtime -> TASK-009 card/UI -> TASK-010 integration and PIE acceptance. Do not create Worktrees by default.
3. Keep the first visual Goal fixed-fixture and presentation-first: one Demo tile, one local scene, one card, one character, and read-only skills. Do not let it decide any Open/Proposed/Deferred rule.
4. Keep [TASK-005 Deterministic Random Foundation](Production/Tasks/TASK-005-Deterministic-Random-Foundation.md) at `Review`; it is not a prerequisite for this non-random Demo cluster and must not be started implicitly.
5. Re-evaluate broad automation, Debug panel, Calendar, Resource Ledger, and production asset migration after the playable slice; Calendar still requires its index-origin decision, and Resource Ledger still requires authored value/cap/debt/loop policies.

## Known decisions

- The old prototype is reference-only.
- Content is not ignored as a whole; generated UE folders are ignored.
- Core rules must be independent from presentation and concrete world Actors.
- Save data is versioned and ID-based.
- The high-level MVP design and architecture baseline is frozen. Explicitly open, proposed, deferred, and playtest-only items remain unresolved; permanent module ownership has not been assigned.
- English design baseline documents are the Agent-facing versions; Chinese files are synchronized user-facing companions.
- Architecture module directories are logical boundaries and documentation homes; they are not standalone UE modules yet.
- The English architecture documents are the technical source for Agents; Chinese companions are user-facing mirrors.
- Encounter battles are turn-based action-timeline battles rather than fixed player/ruler alternation; terrain and card abilities can affect initiative, priority, reactions, interrupts, extra actions, and non-turn windows. The high-level behavior is fixed, but the exact action-bar formula, precedence, and window card allowance are intentionally deferred until implementation needs them.
- The first RealTimeAction mode is ACT, and the MVP requires one complete playable ACT battle loop.
- ADR-0002 establishes Encounter-first delivery and a future C++-first, data-driven ACT boundary. It does not authorize ACT/GAS implementation; built-in GAS needs an approved dependency task, external gameplay plugins need a separate ADR, and exact schemas plus all Open/Deferred gameplay semantics remain unresolved.
- Basic non-A hand skill cards are legal only in local battle, with at most one during each eligible player action turn in Encounter and ACT MVP `Tab`/bullet-time/numeric-slot input in RealTimeAction; the visible slots are 1-5 per page. Lifecycle probabilities and total page count are intentionally deferred.
- ABC-card active abilities are Sandbox-available by default, with exceptional restrictions configured per ability; their additional in-battle legality is not yet decided.
- Local battles can be entered at any calendar phase, pause sandbox time, capture a complete versioned pre-battle snapshot, commit on success, and restore the complete snapshot exactly on defeat or exit.
- The two MVP victories are ruler-core life reaching zero and war balance reaching the player victory boundary; additional documented victory and defeat methods remain future-extensible outcome definitions.
- Synthesis failure grants no reward. Resource loops require explicit cost, timing, and risk; luck may compensate a cost in an authored loop.
- Windows Development packaging, cooking, staging, and packaging-focused learning are deferred until after MVP completion; they are not TASK-001 or M0 gates.
- Repository coordination uses these distinct units: `AGENTS.md` for mandatory rules, `Docs/` for mutable truth, Role profiles for specialist behavior, one Goal/TASK pair for one delivery result, a worktree for independent write isolation, a Primary Agent for accountability, subagents for bounded delegation, and Skills for repeatable procedure.
- A worktree is allocated to an independently mergeable writable Goal, not permanently to a Role or logical module. Shared contracts, maps, binary UE assets, central configuration, Editor sessions, and final integration stay serial.
- Temporary operating choice (2026-09-03): near-term development may use a Local feature branch from `main` (proposed branch: `feature/TASK-001-gameplay-flow`); Worktree is optional and is not created by default until independent write isolation is needed. Existing same-file ownership, review, and integration rules still apply.
- Near-term playable-slice priority (2026-09-07): `TASK-006` through `TASK-010` target a fixed, functional flow comparable to `E:\\2026ue\\padma`—select one SLG tile, enter one mapped local scene, play one card, summon one character, and read its skills. The reference project remains read-only reference material; this priority change does not authorize bulk asset/code migration or any Open/Deferred rule.
- Initial concurrency is capped at four spawned subagents per parent session, no more than two writable worktrees, and one UE build/Editor/PIE lane. These limits may change only after integration evidence supports it.
- Remote push, tags, releases, destructive Git operations, and meaningful scope expansion require explicit user authorization.

## Open questions

- Final numerical coefficients/clamping for the documented synthesis formula, and whether Sigma dice remain for any non-reward failure consequence.
- Whether the overworld and encounter scene share one World Partition world.
- Exact Encounter action-bar precedence, initiative modifiers, response/interrupt priority, extra-action rules, non-turn windows, end-turn effects, and the precise card allowance of those windows; this is intentionally left blank until the relevant implementation task.
- ACT MVP trigger, background blur, movement/attack input rejection, and 1/10 world-time scale are decided. Remaining duration/toggle details, page-boundary behavior, and total page count are deferred.
- Whether ABC active abilities are also legal during Encounter or RealTimeAction.
- Basic non-A hand lifecycle and its card-specific probabilities for discard, return, or cooldown; five visible slots per page and mouse-wheel next-page input are decided, but total page count and boundary behavior are deferred until the relevant card UI implementation.
- The final source format for data import: CSV, JSON, or a mixed pipeline.
- The exact joint battle-route resolution data, including how both sides and terrain/card effects constrain distance/route and map to mode/story triggers.
- The exact semantics of Shape/Returning Ruin.
