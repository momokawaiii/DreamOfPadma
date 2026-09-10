# Archived Project State: 2026-09-09

- Chinese companion: ProjectState-2026-09-09.zh-CN.md
- Full pre-TASK-045 snapshot; historical evidence, not current instructions/state. Relative links are rebased. Current facts: [ProjectState](../../ProjectState.md).

# Project State

- Chinese companion for user reading: `Docs/ProjectState.zh-CN.md`

## Baseline

- Project: DreamOfPadma
- Engine association: see `DreamOfPadma.uproject`; do not change it without an ADR.
- Current milestone: MVP high-level baseline frozen / Agent workflow ready / bilingual change-log protocol established / PIE foundation verified / future ACT architecture boundary accepted / M1 Core contract complete / ADR-0003 accepted / TASK-006 cluster complete / TASK-007 integrated locally / TASK-011 v0.3 design amendment Verified / TASK-008 Encounter foundation and TASK-036 ACT authoring and TASK-040 native model preview Verified; local integration pending
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
- `ADR-0003 Fixed Playable Prototype Vertical Slice` was accepted and `TASK-006 Playable Prototype Vertical Slice Cluster` was completed on 2026-09-07; `TASK-007` was then implemented, independently reviewed, and locally completed as `Done`; `TASK-008` subsequently passed implementation review as `Verified`; TASK-009 and TASK-010 remain separately gated.
- A bilingual, release-style implementation Changelog is now established at `Docs/Changelog.md` and `Docs/Changelog.zh-CN.md`; Primary Agents provide drafts and the Integration Coordinator records the final shared entry before `Verified` or `Done`.
- `TASK-007 SLG Demo World Selection and Transition` was locally integrated on 2026-09-08 after independent Review `Pass`; completion-report closeout commit `9ea99c7` and the shared integration update are local only. The user-owned `Config/DefaultEngine.ini` and default-map change is preserved in separate local commit `d8f107f` for TASK-010 and was not mixed into TASK-007; `TASK-008` has since reached `Verified`, with local integration pending.

## Next tasks

1. Current priority is [TASK-042 HTML rule prototype](../../Production/Tasks/TASK-042-HTML-Playable-Prototype.md): align playable behavior in the browser before resuming UE production. Start at the [HTML entry](../../../Prototypes/HTML/index.html) and [play/UE continuation guide](../../../Prototypes/HTML/README.md).
2. UE TASK-007 remains locally Done; TASK-008/036/040 retain Verified candidates and historical evidence. No C++, UE assets, configuration, dependencies or integration change in this turn. Resume TASK-009/010 and downstream dependency order only after HTML alignment.
3. Do not implicitly start TASK-005 or other unresolved work. HTML temporary numbers do not become production UE configuration; formal rules, assets and structural decisions retain their gates.

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
- ADR-0002's Encounter-first order remains. [ADR-0004](../../Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md) plans native GAS in both modes through TASK-013, separate mode systems/tables and ACT-only action sequences. External gameplay plugins still need an ADR; exact schemas and Open/Deferred gameplay remain gated.
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

## Full-MVP expansion (2026-09-08)

- The user requested the complete rules MVP and bilingual [TASK-011–035 cluster](../../Production/Tasks/TASK-011-Full-MVP-Task-Cluster.md), then explicitly requested implementation grounded in mycard.pdf/mygame.pdf. TASK-011 v0.2 documentation was Verified after four independent read-only reviews; child runtime tasks remain Backlog until their own prerequisites and decisions are met.
- Separate turn-based/ACT character collections and ACT battle-settings characters/weapons are accepted. Basic non-A skills retain shared identity/slots with mode-specific effects. Terrain may restrict an ACT trait without automatically excluding the whole character.
- The user imports placeholders after work starts. On 2026-09-09, D03 accepted clear mycard.pdf values that do not conflict with later decisions; missing, ambiguous or conflicting values still need individual confirmation.
- SLG uses a Civilization VI-like oblique view, nodes and preset connections over a hex-shaped map. The contract-card sandbox summon effects from mycard 6–7 are excluded from the first version pending redesign. The user accepted layered storage: static chapter map definition asset, .umap presentation and SaveGame run changes; see ADR-0005.
- [Source extraction](../../Production/MVPSourceExtraction.md) records all 49 PDF pages, card categories, screen interpretation and conflicts; [MVP decisions](../../Production/MVPDecisionRegister.md) records gates. No independent ACT character/weapon catalog is present in the PDFs.
- TASK-007 remains locally Done. TASK-008 Encounter foundation and TASK-036 static ACT authoring are now Verified; calendar/resource/synthesis/Encounter GAS/ACT combat/save runtime are still pending. TASK-011 itself is documentation-only; no merge, push, release or engine dependency change has occurred.


## Current implementation candidate

Root is the sole writer on Local `codex/mvp-demo-foundations`, based on main `c946591`. TASK-008 v0.2 and TASK-036 v0.1 are `Verified`, each with independent Review `Pass` and no remaining P0–P2 findings. UE5.8.2 Editor compile succeeded; all four project automation tests passed with zero test warnings/failures. UE-authored Encounter/ACT assets and both authoring-script reruns succeeded. The actual Sandbox → Encounter transition, receipt consumption, one placeholder summon and three read-only skill definitions passed runtime smoke; the frame-11 image was visually checked. Protected configuration, Build.cs and uproject are unchanged. Evidence and reproduction steps are in the two completion reports. This is a working-tree candidate: no commit, merge or push. User PIE acceptance/learning remain pending, and TASK-009/010 retain their serial integration gates.

## Map/preparation design amendment

TASK-011 v0.3 is Verified for the documentation amendment covering the user's hover/local-model/global-configuration clarification; independent architecture review returned Pass and bilingual/link/dependency/strict project checks passed. The screen design remains Draft and the interactive sketch passed browser checks at three widths in light/dark themes. TASK-037/038/039 are new Backlog contracts. D24–D26 are now answered: inspection-only click, separate right-side card movement, full enemy node disclosure, FPS future and ACT playable. D27 configuration timing/ownership and D28 relationship bonuses remain open. The Desktop/cd source paths/hashes are reverified. The interactive sketch is design preview, not UE evidence. TASK-008/036 and their prior build/test/review evidence remain Verified; local integration remains unauthorized.

## Model authoring continuation (2026-09-09)

[TASK-040](../../Production/Tasks/TASK-040-Model-Assets-and-Scene-Bindings.md) is Verified on the same Local branch. UE model Data Assets, a typed source catalog, static scene slots and a native model preview are implemented. The Editor build and six automation tests passed; asset creation, valid reruns and two rejected invalid existing-binding reruns passed. The real UE graybox image was inspected. Bilingual/strict/link/dependency checks passed; independent read-only review returned Pass with no remaining P0–P3. User PIE/learning acceptance and local integration remain pending. D29 records three-state mobility and atomic A/attached-B movement with each member checked; metadata is implemented, world movement is not. No full SLG UI, GAS battle, gameplay garrison selection or SaveGame runtime is claimed. The earlier HTML design remains separate evidence. See [ModelAuthoring](../../Content/ModelAuthoring.md) for editable assets.

## Current HTML candidate (v0.4; TASK-042 / TASK-043 / TASK-044)

[TASK-044](../../Production/Tasks/TASK-044-Encounter-Targeting-and-Card-Casting.md) implements attack-first target selection with free cancellation, long-press details for both sides, and drag/preview/confirm basic-card casting with card/environment provenance. Dancer retains optional two-target release; character details use placeholder art and explicitly unconfigured levels/abilities. The latest app/FX smoke file passed 36 cases once; earlier engine/receipt/input evidence is in the task. The user will test browser interaction and requested no further extensive review. TASK-044 is Review for user playtesting, not a new code-review gate. UE production remains paused; HTML temporary values do not change UE assets or rules without a separate task.

The offline HTML connects calendar, economy, synthesis, node movement, Encounter/ACT, rollback and victory. Its new three-tab home screen separates protagonist appearance/poses from ACT/FPS recruitment; forge/watchman dialogues precede one-time settlement; synthesized C appears directly at the current A/B node. Each of nine ABC definitions labels implemented effects; the complete PDF system is not claimed.

47 engine cases and independent code review passed, with review verdict Conditional pass; 42 pure template-render checks are not browser visual acceptance. The user confirmed that the initial version opens the map. Browser automation was denied by the local file URL policy; new double-click/layout/keyboard behavior still needs manual acceptance. TASK-042 remains Review. See its [completion report](../../Production/Tasks/TASK-042-HTML-Playable-Prototype.md); no commit, merge or push.


[TASK-043](../../Production/Tasks/TASK-043-Encounter-Visual-Feedback.md) adds Encounter current-actor markers, future-action predictions, source-to-target trails, actual HP/shield/block/heal/recoil feedback, multi-wave playback and final-hit retention with offline SVG/CSS. Numbers and saves remain unchanged; Rain now correctly skips a fallen caster when allies survive, using the existing alive-unit timeline. 92 Node cases pass (47 original, 20 receipts, 25 playback and input/paint doubles); independent code review found no remaining P0–P2. Real browser visual/pacing acceptance remains pending and the TASK stays Review. UE production remains paused.


0.3.1 repairs the action-windup freeze reported in the user screenshot: native timers call through the global host; scheduler/effect exceptions release presentation and rebuild committed state without duplicate costs. The earlier 85 cases bypassed the default timer path. App tests now load that shipped adapter and add receiver checks, fault recovery and the Vitruvian/Dancer regression. Post-refresh user browser confirmation remains pending.
