# Strategy presentation authoring

- Chinese companion: StrategyPresentation.zh-CN.md
- Scope: TASK-050–052 / ADR-0008–0009; native UE playable map.

## Painted tutorial

Fresh runs in `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld` select `tutorial-painted-v1`. The fixed 2:1 source painting, navy/gold HUD and proportional 1672×941 design canvas follow the user's supplied reference image. The generation request is recorded in `Artifacts/TASK-051/art-prompt.txt`; the delivered image is a reference-guided reconstruction, with no pixel-perfect claim. Source art is `Content/Padma/UI/SourceArt/T_TutorialMap.png` (1774×887 RGB).

`SPadmaTutorialMap` draws the painting and rule overlays. Fresh generator-v3 tutorials retain 55 cells, seven fixed landmarks and seeded wilderness; `fire` (薪火台), directly linked to `home`, initially belongs to the ruler with one guard for Encounter testing. Saved v1 (162 cells) and v2 (55 cells) layouts retain their topology, ownership and garrison definitions without adding the new guard. Core data determines habitability, ownership and open edges; minimap and sidebar use the same stable IDs. See [ADR-0009](../Decisions/ADR-0009-Painted-Tutorial-Hex-Map.md).

Single-click a cell to pin its right-side details and action target. Crossing other cells changes only the map highlight. Clicking a different target updates details while retaining header, objectives, hand, minimap and phase controls; a queued full view retains priority. Inspection itself does not move, pay or start battle. Selected A/B cards still enter the confirmation flow below. Modal ownership, hold, capture and drag continue to block background input.

TASK-051 records its native 1920×1080 rendering, hover/sidebar retention, plus-button zoom, Fit after release, Escape menu gating and time-0/time-4 atmosphere checks. Those earlier checks do not establish acceptance of TASK-052's interaction and motion changes; current build, test, capture and user-playtest evidence belongs in [TASK-052](../Production/Tasks/TASK-052-Tutorial-Interaction-and-Motion-UI.md). The authored reference used for save compatibility remains separate from the active painted layout, so loading an authored map and returning to a tutorial save does not replace the trusted anchor configuration.

Use `-PadmaLegacyMap` for the previous world presentation. The `encounter`, `act` and `roundtrip` capture scenarios retain the legacy map setup; continuing a run uses its saved layout. The landscape/PCG sample below remains available on that path. Native runtime inspection and visual acceptance for the painted tutorial are tracked separately in [TASK-051](../Production/Tasks/TASK-051-Painted-Tutorial-and-Hover.md).

## Tutorial interaction

Select an A hand card, then click a map cell to open a deployment confirmation with the captured card and node IDs. Selecting B and clicking a cell opens an attachment choice listing every colocated world A instance. Each choice shows its own Core eligibility result; ineligible entries are disabled. A cell with no A cards, or no eligible A, shows a clear explanation and offers cancellation. The sidebar deployment action and valid A/B drop use the same placement flow. Preview calls the existing Core deployment command on a value copy; only explicit confirmation executes it against the live run. Confirmation revalidates phase, card, node, attachment target and costs. Cancel pays nothing; subsequent hover cannot replace the captured destination. C deployment retains its existing command behavior.

The tutorial hand uses five columns with additional rows in a vertical scroll box. Existing scroll offset is retained when the hand needs rebuilding, and detail-only map selection updates leave the hand intact. Warm ivory paper carries the title and footer, with thin framing and stable atlas illustrations.

WASD or arrow keys pan the tutorial painting; middle-button drag also pans it. The initial fit is zoom 1 with zero pan. The first nonzero pan raises zoom to at least 1.12, supplying an overscan margin while clamping the painting to the viewport. Map Fit returns to zoom 1 and zero pan after the button releases capture. Modal ownership, held/captured pointers and editable-text/spin-box focus block keyboard panning.

Painted navigation has one input owner: the CommonUI page forwards navigation key down/up to `SPadmaTutorialMap`, which owns the held-key set and Home reset. Its native Slate pointer handlers consume map clicks, middle drag and wheel zoom. Page/overlay changes cancel interaction, clear navigation keys and release the map's own capture; window deactivation or a conflicting pointer capture also clears held navigation. The controller does not poll WASD again for the painted map, avoiding a stale PlayerInput key after Slate consumes its release. The legacy 3D camera remains controller-owned.

The war strip presents two shares of the existing rule score: player share is `clamp(0.5 + 0.5 * War / warGoal, 0, 1)`, and ruler share is its complement. A neutral score therefore displays 50:50. The authored score and victory boundary remain unchanged. `SPadmaWarBalance` eases the displayed share and draws the central balance glyph; the shader supplies teal/red liquid beneath a gently moving surface, dark navy headspace, a thin foam edge and restrained bubbles. Surface motion spans both halves even when the center glyph covers the dividing seam. This presentation adds no separate war resource or simulation.

## Landscape sample

On the legacy presentation path, the run projects stable graph IDs through `APadmaWorldMapActor`. Its continuous shared-vertex procedural surface carries graded roads and node foundations; a separate water surface occupies the outer margin. This is a procedural mesh, not an `ALandscape` sculpting asset. Fixed node positions and permitted graph edges remain authoritative. Decorative terrain does not create traversable tiles.

`DA_FirstRegion_Theme` under `/Game/Padma/World/Maps/Presentation` controls surface grid spacing, hill/mountain relief, margin, water, biome colors, decoration count/scales/slope limit and model bindings. Cosmetic seeds drive the height/noise and controlled PCG independently of gameplay randomness. PCG excludes node foundations, road corridors, water and steep slopes, then places clustered tree/rock instances at sampled surface heights. Selection changes reuse the terrain and decoration.

`/Game/Padma/World/ArtSample` contains editable sample meshes, materials and landmark definitions: two broadleaf variants, a cedar, boulders, citadel, shrine and village. These establish shape, palette and scale; they are not final production character/building art. See Epic's [PCG overview](https://dev.epicgames.com/documentation/en-us/unreal-engine/procedural-content-generation-overview) for the point/filter/spawner workflow.

## Interface ownership and activation

| Layer | Implementation / input | Time and settlement |
|---|---|---|
| Persistent root | `UPadmaGameScreen`; owns page and overlay stacks | No gameplay settlement |
| Gameplay / preparation page | `UPadmaActivatableLayer` in page stack; HUD, map, hand and contextual panels | Calendar advances only through its command; combat retains its own clock |
| Menu / codex / journal | Modal activatable entry in overlay stack | Browsing submits no gameplay settlement |
| Card / unit details | Read-only top overlay; 500 ms hold, move cancellation and consumed release | Encounter enemy autoplay waits; committed actions can finish. ACT clock remains unchanged |
| Dialogue / confirmation | Only top choices or explicit confirm submit commands | Cancel/back does not pay, synthesize, load or retreat |

Only the top overlay accepts input. Underlying gameplay remains visible; Back returns through retained parents and restores focus. Gameplay pages request All input; overlays request Menu input. The focused native activation node handles Escape / gamepad face-button-right because this project has no CommonInput default-back action table configured. Showing a widget, activating it, enabling input and pausing simulation are separate concerns. ACT Tab retains its independent 1/10-speed skill-library policy. Preparation timing/edit availability during battle remains D27; its battle entry stays disabled.

Held hand/garrison cards resolve their **instance ID**; battle units resolve their **battle ID**; codex and ACT preparation use their distinct **definition IDs**. Battle inspection uses live HP/max HP/attack/defense/shield/block and action data. World inspection uses stored instance values and immutable definitions. A configured model portrait takes priority; otherwise the card illustration is clearly labelled. Character level/XP is unconfigured. Shared player skills are separate from character abilities. Unheld C definitions remain restricted until successful synthesis establishes discovery.

The primary runtime skin uses UI-domain material instances for flat navy ink, aged pale gold and warm ivory paper. The earlier nine-slice `/Game/Padma/UI/Textures/T_HUDPanel` and `T_HUDButton` assets are preserved; they are no longer the primary runtime panel/button skin. Primary action buttons own separate material instances and bind hover enter/leave to `HoverAmount`, so one button's scan does not animate another. Card art uses stable-ID UV regions of `T_CardIllustrations`. Native CommonUI classes build their visual content in Slate; there is no claim that each button is a CommonButtonBase Blueprint or that a Widget Blueprint designer skin was generated.

At PIE end the controller calls idempotent `ShutdownPresentation`: cancel gestures, detach command callbacks, explicitly release both manually constructed CommonUI stacks and their Slate caches, then clear screen references. Those stacks are not the WidgetTree root, so superclass cleanup alone is insufficient. Button-closure strong MIDs use the transient package as Outer, preventing a temporarily retained button from keeping Screen/GameInstance alive through the material. Screen-owned UPROPERTY materials are cleared with the screen.

## Native overlay motion

CommonUI activation and focus routing take effect independently of entrance animation. `SPadmaMotionElement` supplies native Slate opacity and translation with a cubic ease-out; material masks reveal portraits. These are not authored Widget Blueprint UMG animation tracks.

| Element | Start delay | Motion |
|---|---:|---|
| Title | 0.05 s | Left entrance; default duration 0.28 s |
| Subtitle | 0.09 s | Shorter left entrance; default duration 0.28 s |
| Portrait | 0.12 s | Left entrance and slanted material reveal over 0.42 s |
| Information | 0.18 s | Right entrance; default duration 0.28 s |
| Actions | 0.25 s | 35 ms stagger in a vertical list, capped after the sixth delay slot; 40 ms stagger in a horizontal row |

The enclosing overlay fades in over 0.16 s. The portrait samples a fixed texture region, with a narrow pale-gold leading edge and no time-driven image flow. A separate portrait MID carries `PortraitTexture` and `UVMinX/Y`, `UVMaxX/Y`. For atlas cell `(column, row)`, use `((column+0.01)/3, (row+0.01)/3)` through `((column+0.99)/3, (row+0.99)/3)`; standalone portraits use 0..1. The material brush itself uses full UVs to avoid applying the crop twice. Back can restore the retained parent immediately; visual delays do not postpone input ownership or change Encounter/ACT timing.

## Author motion UI assets

With the Editor closed and the serial UE lane free, run the new authoring script with an RHI:

```powershell
$EngineRoot = 'E:/Epic Games/UE_5.8'
& "$EngineRoot/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "$PWD/DreamOfPadma.uproject" -run=pythonscript -EnablePlugins=PythonScriptPlugin "-script=$PWD/Scripts/Editor/AuthorMotionUI.py" -unattended -nop4 -DDC-ForceMemoryCache -ddc=NoZenLocalFallback
```

`Scripts/Editor/AuthorMotionUI.py` imports `Content/Padma/UI/SourceArt/T_HandParchment.png` (1448×1086) as `/Game/Padma/UI/Textures/T_HandParchment`. The built-in image-generation prompt is recorded in `Artifacts/TASK052/paper-prompt.txt`. The source is clean, finely textured ivory paper without a baked frame or bevel.

| Runtime material instance | Purpose / texture binding |
|---|---|
| `/Game/Padma/UI/Materials/MI_UIPaper` | Paper title/footer surfaces; `PaperTexture` → `T_HandParchment` |
| `/Game/Padma/UI/Materials/MI_UIInkPanel` | Flat navy panel, fine grain and restrained gold edge |
| `/Game/Padma/UI/Materials/MI_UIGoldAction` | Matte pale-gold action surface; `PaperTexture` → `T_HandParchment`; hover scan |
| `/Game/Padma/UI/Materials/MI_UIWarLiquid` | Opposing teal/red liquid strip and fine seam |
| `/Game/Padma/UI/Materials/MI_UIPortraitReveal` | Fixed portrait sampling and slanted reveal; `PortraitTexture` defaults to `T_CardIllustrations` |

Each instance has a same-folder parent with the `M_` prefix in place of `MI_`. All five parents use the UI material domain and translucent opacity.

| Scalar | Default | Purpose |
|---|---:|---|
| `RevealAmount` | 1 | 0 hides, 1 fully reveals; the active portrait animates this value |
| `HoverAmount` | 0 | Hover response; primary buttons set 1 on enter and 0 on leave |
| `AnimationStrength` | 1 | Strength of time-driven effects; 0 removes that motion |
| `FreezeAnimation` | 0 | 1 substitutes `PreviewTime` for GPU Time |
| `PreviewTime` | 0 | Fixed shader time in seconds |
| `ScanAmount` | 0.35 | Gold scan intensity, multiplied by hover and animation strength |
| `Balance` | 0.5 | War material: 0 all red, 1 all teal |
| `AspectRatio` | 12 | War strip width/height, supplied from its arranged geometry |
| `WaveStrength` | 1 | War surface/seam-wave strength |
| `UVMinX`, `UVMinY` | 0 | Portrait region minimum |
| `UVMaxX`, `UVMaxY` | 1 | Portrait region maximum |

The first five scalars are shared; the others belong to the gold, war or portrait material as described. GPU Time drives the scan and fluid phase; native presentation updates hover, eased balance, geometry ratio and portrait reveal. `FreezeAnimation=1` with `PreviewTime=0` or another fixed second supports reproducible shader captures. This does not freeze native Slate entrances or gameplay. `-PadmaMapArtTime=<seconds>` applies the runtime shader-time override to the map and applicable skin MIDs without changing saved assets.

Reruns validate and preserve existing graphs, source texture and MI overrides. The one explicit migration upgrades only the known prior `M_UIWarLiquid` Custom-code hash in the script's own tagged, expected node set; it changes that code after validation and successful compilation, preserving instance parameters. Unknown/custom graphs are rejected, and a failed upgrade restores the prior in-memory code without saving it. Wrong asset classes, missing bindings or a changed source hash also fail clearly. UE 5.8 setter results are checked by readback. `Artifacts/TASK-052/motion-ui.json` contains source dimensions/hash, created/preserved/migrated assets, shader hashes, compile errors and effective scalar values. A ready report confirms asset authoring only; runtime capture and interaction acceptance remain in TASK-052.

## Author the painted tutorial assets

Run `Scripts/Editor/AuthorTutorialPresentation.py` with the Editor closed and the UE lane free. Use an RHI so the Custom HLSL can compile; omit `-NullRHI` for this script:

```powershell
$EngineRoot = 'E:/Epic Games/UE_5.8'
& "$EngineRoot/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "$PWD/DreamOfPadma.uproject" -run=pythonscript -EnablePlugins=PythonScriptPlugin "-script=$PWD/Scripts/Editor/AuthorTutorialPresentation.py" -unattended -nop4 -DDC-ForceMemoryCache -ddc=NoZenLocalFallback
```

The script imports `/Game/Padma/UI/Textures/T_TutorialMap` as full-resolution sRGB/UserInterface, creates UI-domain `/Game/Padma/UI/Materials/M_TutorialMap`, and binds `/Game/Padma/UI/Materials/MI_TutorialMap`. The map texture uses MaxTextureSize 0 and LODBias 0; the small nine-slice texture limits below do not apply. The material samples the painting at unchanged UV and adds localized water ripples/flow, drifting mountain/valley haze and a warm forge pulse near UV (0.765, 0.705). GPU Time drives those effects; there are no per-frame CPU parameter updates.

| Parameter | Default | Authoring use |
|---|---:|---|
| `MapTexture` | `T_TutorialMap` | Source painting texture object |
| `AnimationStrength` | 0.7 | Overall effect strength; 0 returns the original painting |
| `CloudStrength` | 0.55 | Cloud and valley haze strength |
| `FreezeAnimation` | 0 | Set to 1 to use a fixed shader time |
| `PreviewTime` | 0 | Fixed time in seconds when animation is frozen |

For deterministic atmosphere screenshots, set `FreezeAnimation=1` and `PreviewTime=0` on the instance; restore `FreezeAnimation=0` for animation. Set `AnimationStrength=0` for a reduced-motion/base-art check. These controls change presentation only and do not pause gameplay.

Development builds also accept `-PadmaMapArtTime=0` (or another time in seconds) to freeze only the runtime material instance for reproducible captures, without changing the saved material asset.

Reruns validate and preserve existing assets and instance overrides. Missing source or wrong asset classes fail clearly. The script recognizes its `Padma.Presentation.Generator` metadata and can repair missing parent/texture bindings on its own generated instances; conflicting custom bindings are preserved and reported as errors. It does not replace an existing painting or rebuild a custom material graph. Intentionally changed source art must be reimported through Unreal. The creation/validation report is `Artifacts/TASK-051/tutorial-presentation.json`; task evidence belongs in TASK-051.

## Rebuild the legacy landscape and preserved nine-slice assets

Keep the Editor closed and run one commandlet at a time after compiling DreamOfPadmaEditor. From the repository root, choose your engine path:

```powershell
$EngineRoot = 'E:/Epic Games/UE_5.8'
& "$EngineRoot/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "$PWD/DreamOfPadma.uproject" -run=pythonscript -EnablePlugins=PythonScriptPlugin "-script=$PWD/Scripts/Editor/AuthorLandscapeSample.py" -unattended -nop4 -NullRHI -DDC-ForceMemoryCache -ddc=NoZenLocalFallback
& "$EngineRoot/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "$PWD/DreamOfPadma.uproject" -run=pythonscript -EnablePlugins=PythonScriptPlugin "-script=$PWD/Scripts/Editor/AuthorStrategyUI.py" -unattended -nop4 -NullRHI -DDC-ForceMemoryCache -ddc=NoZenLocalFallback
```

The landscape script creates/updates only its sample assets and the existing visual theme, validates mesh orientation/material slots, and writes `Artifacts/TASK-050/landscape-assets.json`. Preserve custom art in separate wrappers before deliberately regenerating sample assets. The UI import script preserves existing pixel content and normalizes the panel/button MaxTextureSize to 256/512 for compact nine-slice corners; source PNGs retain their original resolution. Both fail clearly on missing/wrong-class inputs; neither changes the graph, accepted rules or save schema.

Runtime capture options: `-PadmaDemoScenario=details` opens held dancer inspection; `layers` opens menu → codex → dancer definition; existing `home`, `encounter`, `act` and `roundtrip` scenarios remain available. `-PadmaPlayableCapture=<absolute.png>` captures the native UI after startup and exits. Check `[PadmaLayers]` activation/deactivation records and `[PadmaMapPCG]` completion logs. Acceptance evidence belongs in TASK-050.
