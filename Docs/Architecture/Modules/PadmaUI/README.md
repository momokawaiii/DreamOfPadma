# PadmaUI Module Program Document

- Document ID: ARCH-MODULE-UI-001
- Version: 0.1
- Status: Planned logical boundary; no standalone UE module yet
- Canonical language: English for Agent consumption
- Chinese companion: README.zh-CN.md
- Owner: UI and Input Module Agent
- Current implementation: Source/DreamOfPadma/ is the temporary shared runtime module
- Parent architecture: ../../ProgramArchitecture.md and ../../DataDrivenArchitecture.md

## 1. Purpose

PadmaUI presents authoritative results and converts local input into shared commands. It supports the strategic/RTS-like sandbox, turn-based Encounter controls, and the RealTimeAction input route without owning rule state.

## 2. Responsibilities

- Input adapters for sandbox strategy, Encounter turns, RealTimeAction, mouse, keyboard, controller, ACT, and FPS presentation.
- Encounter turn controls and end-turn feedback.
- ACT RealTimeAction MVP `Tab` interaction: clicking `Tab` opens the repository panel, blurs the background, reveals five cards, and slows world time to 1/10.
- While the ACT MVP repository/bullet-time state is active, movement and attack input are rejected; only basic non-A card-related input is accepted, while the battle scene preserves inertia.
- Shared card repository interaction: `Tab` opens the repository, slots 1-5 are visible per page, and the mouse wheel requests the next page. Total page count and boundary behavior are deferred.
- Command construction and local input validation that does not replace rules.
- HUD, card hand, synthesis preview, map, resource, threat, and outcome views.
- Outside-battle activation controls for ABC-card active abilities.
- View models or read-only presentation state.
- Display of calculation traces, command failures, combat logs, and outcome reasons in debug builds.
- Presentation references for card art, icons, animation, VFX, audio, camera, and localization.
- Accessibility and input feedback requirements when they become relevant.

## 3. Non-responsibilities

PadmaUI must not own:

- Authoritative resource, card, world, combat, or outcome state.
- Synthesis, damage, AI, or victory formulas.
- Direct mutation of GameState, WorldState, or save data.
- Persistence of gameplay truth in widgets.

The UI sends commands and displays results/events. A disabled button is not a substitute for rule validation.

## 4. Public contracts

- Input adapter to command mapping.
- Read-only state view models.
- Preview request/result views.
- Command success/failure presentation.
- Battle-mode, action-timeline, turn-state, bullet-time, page, and numeric-slot input feedback.
- ABC-card active-ability activation requests from the sandbox.
- Combat log and AI trace presentation.
- Outcome and save/load notifications.
- Control-mode transition presentation requests.

UI contracts should not expose concrete actor ownership as the only way to render a state.

## 5. Data and runtime ownership

UI consumes presentation profiles, localization IDs, icon/mesh references, and read-only Core/World/Gameplay views. It owns transient view state, selection, focus, and input-adapter state.

Presentation data can change without changing rule definitions. Rule assets must not depend on a widget path.

## 6. Dependencies and integration

PadmaUI depends on public command, result, event, and read-only view contracts. PadmaGame composes the UI with runtime services. UI must not become a dependency of PadmaCore.

## 7. Tests and debug evidence

Required tests:

- Input maps to the correct command.
- Invalid commands display an explanation from the rule layer.
- Preview displays the same calculation fields as execution.
- UI reacts to world, combat, resource, and outcome events.
- Loading a save reconstructs views from state rather than widget history.
- Controller and keyboard adapters preserve command semantics.
- A basic non-A card cannot be submitted from the sandbox or outside the player's eligible Encounter action turn, and the one-card-per-turn limit is visible.
- Clicking `Tab` opens the shared card repository, blurs the background, reveals five cards, and enters 1/10-speed ACT bullet time; slots 1-5 map one-to-one between the Encounter hand and the ACT RealTimeAction page, and the mouse wheel requests the next page.
- While that state is active, movement and attack input are rejected, input is filtered to basic non-A card actions, scene inertia is preserved, and a quick numeric-key press maps to the intended ACT card-slot request. Total page count and boundary behavior are not assumed.
- An ABC-card active ability can be submitted from the sandbox without entering a battle and reports its configured restrictions.

Manual evidence should include screenshots or a short capture of synthesis preview, failure explanation, node mutation, combat log, and outcome reason.

## 8. Implementation stages

1. Implement a minimal command panel and state text view.
2. Add card hand, ABC active-ability controls, and synthesis preview.
3. Add map/node and resource views.
4. Add turn-based Encounter controls and combat log.
5. Add the complete ACT RealTimeAction bullet-time, five-slot paging, and numeric-slot battle loop after the Encounter path is stable.
6. Add alternate mode presentation and polish.

## 9. Learning targets and risks

Learning targets: UMG, MVVM or view-model design, input abstraction, UI feedback, localization, accessibility, and presentation performance.

Main risk: placing gameplay rules in Blueprint widgets because it is convenient to prototype.
