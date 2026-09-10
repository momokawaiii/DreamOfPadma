# ADR-0008 Activatable presentation layers

- Chinese companion: ADR-0008-Activatable-Presentation-Layers.zh-CN.md
- Status: Accepted
- Date: 2026-09-09
- Scope: TASK-050–052 native presentation; follows the user's requested CommonUI hierarchy and its Slate/material motion extension.

## Decision

The persistent screen owns separate CommonUI page and overlay stacks. A gameplay or preparation page is an actual activatable widget. Passive HUD belongs to that page. Details, menus, dialogue and confirmations are modal activatable widgets on the overlay stack; only its top entry accepts input. The underlying page remains visible. Back restores the previous overlay, then the page. Activation controls routing and focus; it does not settle gameplay or imply world pause.

The controller projects stable IDs and read-only rule/combat data into view models and submits commands to existing services. Overlay navigation state is transient. A press gesture owns the 500 ms inspection threshold, movement cancellation and suppression of the release click. World input and keyboard shortcuts respect overlay ownership.

A map click pins the right-side details and action target; hover controls only the map highlight. A new clicked target uses detail-only projection, retaining header, objectives, hand, minimap and phase controls without replacing a queued full view. A/B placement captures card and clicked node before the modal: A confirms, B lists colocated A eligibility or a no-target explanation. Preview uses a Core command value copy; confirmation revalidates the live command and cancel pays nothing.

The painted map has one navigation owner. Its CommonUI page routes key down/up to the native Slate map; that map owns navigation state, Home, click, middle-drag and wheel handling. Cancellation clears held navigation and releases its own capture. Controller polling does not duplicate painted-map WASD; the separate 3D camera remains controller-owned. This keeps input release and cancellation in the same ownership chain as input start.

Visual timing is separate from activation. Native Slate opacity/translation and UI material masks provide entrance, hover scan and portrait reveal; no Widget Blueprint UMG animation tracks are required or claimed. The current entrance sequence starts title/subtitle/portrait/information/actions at 0.05/0.09/0.12/0.18/0.25 seconds, with 35–40 ms action staggering. Input ownership and Back do not wait for that sequence. A portrait MID owns its texture and crop; shader time never displaces the portrait image. Per-button hover MIDs prevent shared scan state. Material shader-time controls do not pause Slate activation or gameplay.

Inspection uses card-instance data outside battle and live battle snapshots inside battle. An optional model portrait takes precedence over the existing card illustration. Missing level/progression data remains explicitly unconfigured. Character abilities and the player's shared skill repository are separate sections. Encounter inspection gates enemy autoplay, as required by Combat.md/TASK-044; already committed actions may finish. ACT retains its existing timing, including the separate Tab skill-library policy. D27 preparation timing is not decided by this UI change.

Exit lifecycle explicitly releases the manually constructed CommonUI stacks and their widget-pool/Slate caches; the WidgetTree root does not include those stacks. Controller EndPlay invokes idempotent `ShutdownPresentation` to detach callbacks, cancel input and release resources. Button-closure strong MIDs must use the transient package as Outer, so they cannot retain GameInstance through Screen. A one-GC release test and actual PIE Stop checks verify this boundary.

## Consequences

CommonUI nodes provide lifecycle and input ownership independently of drawing style. UI-domain material brushes now supply the primary paper/navy/gold skin; older nine-slice assets remain preserved. The opposing liquid war strip displays complementary shares of the existing rule score, starting at 50:50 for neutral progress; its smoothing and central glyph add no gameplay state. Asset paths, material parameters and authoring commands are maintained in [StrategyPresentation](../Content/StrategyPresentation.md).

Continuous terrain and PCG remain World presentation under ADR-0007. The painted tutorial's versioned Core grid is governed separately by ADR-0009; UI geometry and animation do not become save authority. Parent overlay contexts, retained detail regions, captured placement targets and input restoration require focused regression coverage. TASK-052's runtime capture and interaction acceptance remain in its task evidence; implementing this motion sequence alone is not an acceptance result.
