# PadmaUI

- Document ID: ARCH-MODULE-UI-001
- Version: 0.3
- Status: Native CommonUI/Slate implementation; production UI redesign pending; no standalone UE module
- Chinese companion: [README.zh-CN.md](README.zh-CN.md)
- Owner: UI and Input
- Parent: [ProgramArchitecture](../../ProgramArchitecture.md)

## Current implementation

UI/Screens has a persistent root and separate CommonUI page/overlay stacks. Gameplay pages request All input; modal details, menus and confirmations request Menu input. Native Back restores navigation. Most content is native Slate with texture brushes, not a finished designer-editable WBP collection. See [StrategyPresentation](../../../Content/StrategyPresentation.md) and [ADR-0008](../../../Decisions/ADR-0008-Activatable-Presentation-Layers.md).

Current map clicks pin details; hover is feedback. A/B hand selection plus cell click opens deployment/attachment flows. World inspection, global preparation, shared skill repository and ACT character/weapon roster remain distinct contexts backed by their own authoritative services.

## Redesign contract

Q72/Q78/Q84 authorize rebuilding the complete page hierarchy, layout, styling and gestures. Preserve **domain state, commands and result semantics**, not the old widget arrangement or hotkeys. New behavior requires an explicit command/result extension; visuals and animation do not authorize direct RunState mutation.

The user's full UI specification should provide:

| Contract | Required information |
|---|---|
| Pages and transitions | Entry/exit conditions; persistent HUD versus active page versus modal overlay |
| Read model | ViewModel fields, stable identities, update events and loading/empty/error states |
| Player intents | Target identity, request, success/failure result and disabled explanation |
| Input lifecycle | Focus target, mouse/keyboard routing, Back/cancel, nested modal return and input restoration |
| Run integration | Save/continue, tutorial skip/replay, mode travel and cleanup behavior |
| Presentation | Layout/type/color/texture, layer order, enter/exit timeline and sound/Cue bindings |

Refresh only views affected by a changed value/event. Inspecting a node must not rebuild unrelated HUD/hand views or clear a pending command selection. Rendering a disabled button does not replace authoritative validation.

## Ownership and presentation

UI owns temporary selection, focus and display state. Core/Game/Gameplay own resources, cards, maps, battle results and saves. Restore views from those values after loading; do not persist gameplay truth in widgets. Rules own permissions, costs, slow-time and slot semantics; the accepted redesign may change their presentation/gesture.

CommonUI handles activation/input/Back; UMG/Slate handles composition; shared UI materials and widget animations handle masks, scan highlights, gradients, motion and parallax. Niagara, camera and audio are presentation consumers. Start from one representative page/card before scaling. Do not require a unique material per button or AE/video playback for ordinary interactive HUD content.

Keyboard/mouse is P0; hand-controller polish is deferred. The multiplayer entry is hidden or clearly unavailable. Chinese production text and stable TextIds are required; full English and voice acting are deferred. Audio placeholders/absence are allowed during functional development; RC audio is governed by [BuildMatrix](../../../Production/BuildMatrix.md).

## Verification and learning

Check command mapping, error feedback, partial refresh, selection persistence, nested activation/Back, cancel/failed action, input restoration, mode travel and PIE teardown. Apply the final interaction specification when available; do not claim a new layout accepted from a screenshot alone.

[DemoDeliveryPlan](../../../Production/DemoDeliveryPlan.md) owns the UI/material/motion learning units and asset checklist. Existing TASK-014/031/035/037/038/040 preserve implementation history; they do not freeze the next Demo's screen design.
