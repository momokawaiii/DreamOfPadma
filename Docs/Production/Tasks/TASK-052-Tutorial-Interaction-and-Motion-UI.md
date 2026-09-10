# TASK-052 Tutorial interaction and motion UI

- Chinese companion: TASK-052-Tutorial-Interaction-and-Motion-UI.zh-CN.md
- Status: Review (implementation verified; user visual/hold-feel acceptance pending)
- Primary / Local checkout: Codex Primary / codex/mvp-demo-foundations
- User authorization: Fix five playtest issues: regional hover refresh; select A/B then click a map target with confirmation/attachment choice; vertical parchment hand; WASD and fewer tutorial cells; animated 50:50 war balance and refined runtime motion UI.

## Outcome and scope

Retain the fixed tutorial painting and actual rules. The latest follow-up pins details on click; hover only highlights the map without replacing header, objectives or hand. Explicit target clicks snapshot the selected card and node; A requires confirmation, B requires selecting a colocated A or a clear no-target modal. Confirmation revalidates current state and never bypasses cost/phase rules. Preserve the authored war score and victory threshold; display its neutral value as 50:50 and map signed progress to opposing shares.

Allowed source: Source/DreamOfPadma/Public and Private/UI/{Screens,Input,Layers}; Public/Game/Framework/PadmaPlayerController.h and Private/Game/Framework/PadmaPlayerController{,UI,Map,Inspection}.cpp plus a bounded new placement helper; Public/Private/Core/{Content/PadmaContentTypes,WorldMap/PadmaTutorialMap,WorldMap/PadmaWorldMapDefinition,Run/PadmaRunRules}; Public/Private/Game/Run/PadmaRunSubsystem if required for trusted compatibility; Private/Tests/{UI,World}. Assets/scripts: Content/Padma/UI/SourceArt and project UI Textures/Materials, Scripts/Editor/AuthorMotionUI.py and ExportPlayableData.cjs only if reflected fixture fields change. No third-party assets, unrelated config, maps or gameplay combat settlement changes.

Documentation: this TASK pair; affected StrategyPresentation, WorldState, ADR-0008/0009, save/authoring contract only when changed, ProjectState, index and Changelog pairs. Artifacts/TASK-052 holds baselines, prompts, captures and checks. One serial UE lane. Independent workers own disjoint controller, Core-grid and art-authoring packages; Primary owns widget integration.

## Acceptance

- [x] Hover preserves non-detail widget identity and scroll/selection; no full HUD reconstruction.
- [x] Select A/B then click target: A confirmation, B eligible-A list/no-A message; cancel and stale state are safe; real deployment/attachment succeeds.
- [x] Hand wraps into rows and scrolls vertically, with parchment texture and legible counts.
- [x] WASD pans tutorial at initial zoom; fresh tutorial has substantially fewer cells and seven key nodes; old saved grids remain coherent.
- [x] War UI starts at 50:50, uses a central balance icon and opposing animated liquid, and reflects actual war progress.
- [x] Matte navy/aged gold/paper palette, restrained scan/reveal/stagger motion and retained CommonUI modal routing.
- [x] Editor build, focused tests, native interaction/render evidence and scoped independent review.
- [x] Final bilingual documentation and scoped whitespace checks.

## 2026-09-10 follow-up: PIE exit, click pinning and nearby guard

The user explicitly requested the PlayLevel.cpp:553 PIE-exit repair, click-pinned cell details, and a nearby enemy garrison. Primary implemented this follow-up serially; existing visual-acceptance limits remain. Earlier hover/v2 evidence below describes the pre-follow-up version. Added documentation authority covers the affected Design/EN and Design/ZH/06_MapAndPreparation.md pair.

- Controller changes SelectedNode/PreviewNode and queues regional details only on click; hover cannot change the sidebar. select-node no longer unconditionally refreshes the full HUD; A/B modals still request full views when needed.
- EndPlay calls idempotent ShutdownPresentation to detach callbacks, cancel input and explicitly release manual CommonUI stacks/Slate caches. Button-closure strong MIDs use the transient package as Outer. The original crash log lacks a complete root path; these two source retention paths were repaired, then checked with one-GC and actual PIE-exit evidence.
- Fresh generator v3 shares v2's 55-cell projection; fire initially belongs to the ruler with one guard and a direct home edge. v1/v2/new runs restore in both directions using trusted authored configuration; old runs gain no guard. No maps, config, assets or combat settlement changed.
- Exact follow-up source baselines/delta are under Artifacts/TASK-052/ClickExit/Before and changed-files.json: PlayerController exit/Map/UI/Placement, GameScreen, ActivatableLayer, TutorialMap and their UI/World tests; new PadmaUISessionReleaseTest.cpp.
- ClickExit/build-final.log: Editor build succeeded. ClickExit/TestsFinal/index.json: 32 passed (31 clean, one expected invalid-generation warning), zero failed/not run. Includes one-GC GameInstance/screen/stack release, repeated shutdown with a retained button, click/hover isolation, A/B, direct Encounter, v1/v2/v3 restores and invalid-anchor rejection. An initial new test imposed a guard-count restriction beyond the existing fixed-anchor contract; it was corrected to test actual anchor-position rejection without broadening save validation.
- ClickExit/pie-result.json and pie.log: three actual Editor/D3D12 PIE Start/Stop cycles, each with map→menu→codex→character-definition details. All stopped cleanly without the PIE/GC retention assertion. Startup contains separate unattributed engine automation-test messages, so this is not a claim of a globally error-free log.
- Independent read-only review: Pass, no unresolved P0–P3, based on current delta, final build/32 tests and three PIE cycles. A native play window was opened; automated mouse operation could not proceed because desktop-app approval timed out. Click behavior is supported by automation tests, not claimed as a fresh mouse playtest.
- Follow-up documentation checks: AuditDocs passed (284 Markdown files / 142 pairs); scoped whitespace check passed for 30 changed files. Logs: ClickExit/audit-docs.log and ClickExit/whitespace-check.log. No Git integration performed.

## Evidence and remaining work

Evidence root: `Artifacts/TASK-052/`. The existing dirty Local checkout is retained; task-start source/document baselines are under `Before/`.

- Hover uses a detail-only projection and persistent right host. Header, objective, phase, hand widgets and scroll offset survive hover; actual resource/hand changes update their region. A queued full refresh cannot be replaced by a later partial hover. The same regional contract covers the legacy 3D-map sidebar.
- Placement stores the selected card and clicked node IDs. A confirmation and B per-instance target choices run the existing Core validation again at payment; disabled choices carry reasons. More than three modal actions use a bounded vertical scroll region, including Close. No-A and stale/cancel paths spend nothing. Click, drag/drop and sidebar deployment route through the same placement contract.
- Fresh tutorial generator v2 uses 55 cells, radius 10 in normalized map coordinates, with seven fixed key positions. Shared `FPadmaHexGridConfig` drives generation, exact picking and outlines. Trusted v1 configuration preserves 162-cell saves, including saves without the new optional grid field; forged layouts are rejected atomically. First pan supplies 1.12x overscan without blank edges, and Home restores the exact fit.
- The painted map owns mouse selection, wheel and middle-button panning in Slate. Its CommonUI page forwards navigation keys; capture consumes and clears those keys, and page/modal changes cancel navigation and map-owned capture. PlayerInput no longer polls a second WASD state for the painting; the 3D camera retains its controller path.
- `T_HandParchment.png` and five project UI materials provide paper, matte ink, gold scan, liquid balance and portrait mask reveal. `AuthorMotionUI.py` is repeatable and preserves existing MI overrides; the final known-hash migration changes only the war material's shader. `motion-ui.json` reports ready with no warnings. No third-party assets changed.
- War score remains the authoritative existing rule. The display is `clamp(0.5 + 0.5 * War / warGoal, 0, 1)` versus its complement: initial War=0 displays 50:50. Liquid uses animated surface/foam/bubbles; the middle scale remains vector geometry. Native Slate elements provide eased, staggered entrance; per-widget dynamic UI materials provide reveal/scan. CommonUI still owns activation and input. This delivery adds no AE video, Niagara system or UWidgetAnimation asset.
- `build-final.log`: final Editor build succeeded in **9.43 s** after a disabled-button text-contrast correction. `Tests/index.json`: **35 passed** (34 clean plus one expected invalid-generation warning), zero failed/not run. `Tests-InputFinal/index.json`: **5/5 UI tests passed**, zero warnings/failures, after final keyboard/capture fixes and before the final color-only change. Coverage includes actual widget identity/scroll/queued state, many-target accessibility, placement cancellation/stale state, generation versions, old saves, topology and relevant prior rules.
- `native-input-final.log`: Primary used native Windows input to select Earth, click home, confirm A deployment (**Flow 60 to 56**), select B yarrow, click a no-A tile, observe the no-A modal, verify W was blocked and cancel without spending. Selecting home then choosing the Earth instance attached B (**Flow 56 to 55**, garrison count 2). W and Home worked; changing liquid surfaces were observed under real RHI. Logs record the session/layers; screenshots shown during the input tool calls and these observations establish the UI result.
- `native-final.log`: after eliminating duplicate keyboard ownership, Primary scrolled seven cards to the second row, hovered another node without losing scroll, panned with W, opened the menu, verified W did not pan behind it, closed with Esc and restored map fit with Home. Header/hand positions remained stable.
- `tutorial-final.png` / `capture-final.log`: actual 1920x1080 final HUD inspected, including readable disabled action text. `details-final.png` / `capture-details.log`: actual card-detail portrait and current attributes/ability text inspected after mask reveal. Configured data is shown; missing independent portrait/level/skills remain explicitly labelled. These renders do not prove pixel identity with the reference.
- Independent read-only review: **Pass**, no unresolved P0-P3 findings. Resolved findings included legacy regional refresh, long attachment lists, material instance parenting, queued refresh preservation and mixed keyboard ownership.
- Final checks: `AuditDocs.ps1` and scoped baseline-to-working-file whitespace checks passed, including new source files. Logs: `audit-docs.log`, `whitespace-check.log`; `changed-files.json` records the bounded task delta. No structural/shared-configuration change required a broader project audit.

User art/physical long-press feel, sustained mixed mouse-and-key gestures and packaged-build behavior remain unverified. No commit, merge or push was authorized or performed.
