# Dream of Padma HTML Rule Prototype

- Chinese companion: README.zh-CN.md
- Document ID: HTML-PROTOTYPE-042
- Version: 0.4
- Status: Playtest candidate
- Tasks: [TASK-042](../../Docs/Production/Tasks/TASK-042-HTML-Playable-Prototype.md), [TASK-043 feedback](../../Docs/Production/Tasks/TASK-043-Encounter-Visual-Feedback.md), [TASK-044 targeting and card casting](../../Docs/Production/Tasks/TASK-044-Encounter-Targeting-and-Card-Casting.md)

## Open and play

Open [index.html](index.html) in a desktop browser. The HTML, CSS and JavaScript run offline without a server or package installation. Refresh an already-open page after updating these files. The header should say **HTML 0.4**. Keep the files together. Save/Continue uses this browser's local storage at the same launch location; another browser/location may not share that save. This is the new playable prototype, not the older visualization-cache sketch.

The user confirmed that the initial page opens and enters the map. Automated browser access to its local file URL was rejected by the browser security policy; no alternate access was attempted. New interactions and visual layout still need manual browser acceptance. State tests are separate evidence.

## Check the current amendment

1. Start a trial. **Single-click home** to inspect it. **Double-click home** to open the full preparation screen; the right-side “Enter Home Preparation” button is an accessible alternative. Neither action moves cards or costs resources.
2. Switch the top tabs: **Node Details / Protagonist Display / Other Modes**. Node Details lists the actual home core, resident protagonist and deployed ABC instances. “View Node Scene” still opens the separate placement preview.
3. In Protagonist Display, preview either placeholder protagonist, three outfits, sword/spear and compatible poses. Save the resident/display configuration. Leaving an unsaved preview discards only that preview. These choices grant no statistics, ABC cards or ACT equipment.
4. In Other Modes, select ACT or FPS and save that mode's recruit/weapon. ACT uses its saved recruit on the next battle. FPS supports selection preview only. Changing either mode must not change the resident protagonist or the other mode. The separate global settings page reads the same ACT state.
5. Return to the map and enter Morning. Deploy **Florence's Dawn**, then attach **Cherry Blossom Dream**. Select the group and move it from home to **Fire**. Advance Dusk → Dawn → Morning so its field accumulates once.
6. On Fire, preview and execute synthesis. Participating A/B go to ABC discard. On success, C appears **directly on Fire**, with one arrival notification and no hand entry/second deployment fee. The new C is selected for further movement. Failure creates nothing.
7. Reach **Watchman** or **Forge**, then click the operation. Read/advance the dialogue and explore its branch. Opening, advancing or cancelling must not grant anything. Only the final acceptance grants the existing one-time reward; a completed event cannot be claimed again. Escape and “Leave for now” cancel without consuming the event.
8. Save on the sandbox, reload and continue. Home appearance, ACT/FPS selections, world C and completed event flags survive. Dialogue and battle must end or be cancelled before saving/loading.

## First combat loop

Default route: Home → Fire → Plain → Forge → Ruler Core. A synthesized C can move directly; the initially held Dancer may still be deployed from hand. Choose ACT or Encounter at the separate Move Here command when the destination contains enemies.

- ACT: WASD/arrows or click the field to move; Space or the attack button attacks in range. Tab opens the five-slot skill repository at one-tenth world speed; 1–5 plays cards. Movement/normal attack input is rejected while open, but existing velocity continues. Tab closes it.
- Encounter: units act in next-ready timeline order. A player action permits one optional basic card, followed by an ordinary attack or guard. For an ordinary attack, first press the attack button, then click a living enemy to execute. The Dancer's attack controls optionally select two targets; the attack executes when the second distinct valid enemy is clicked.
- Basic cards enter their own discard pile and recycle on exhaustion. ABC material cards use a different discard pile.
- Local defeat/exit restores the complete pre-entry run, including move costs, inventory, random state, roster and log. Success commits. Core HP zero or the player war-balance boundary wins this fixture.

## Targeting, hand cards and unit details (0.4)

1. Hold either a friendly or enemy unit for about **500 ms** to inspect it. The detail panel shows a portrait placeholder, all currently available statistics and abilities. Level is explicitly **not configured**; it is not inferred from another value. Opening details does not attack or spend resources.
2. Press **Ordinary Attack** to enter target selection. **Cancel** leaves it without spending anything. With the default one-target setting, clicking a valid enemy immediately attacks that enemy. The Dancer can select a target count of two in the attack controls: the first valid click selects one enemy, the second distinct valid click executes against both. Its authored maximum is two; no additional target is silently selected.
3. Drag a basic skill upward from the hand. Drop a single-target card on the appropriate living noncore unit; drop **Fishbone Rain** into the arena. A valid drop opens a target/cost preview. Click **Confirm Release** to pay and resolve the card. Dropping, selecting or previewing alone does not spend resources. **Cancel**, **Escape** and invalid drops are free.
4. Alternatively, click a card or press its number to select it, then click its valid unit or the arena and confirm. Changing the card/target updates the preview. The current action still permits only one basic card; playing it does not replace the unit's ordinary attack or guard.
5. Basic skills originate from the **card or environment**, without a C-unit caster or a trail from that character. Fishbone Rain falls from the sky in three waves and affects **both allies and enemies**. Ordinary attacks keep their character-to-target animation.

The user will verify these page interactions during play. This update implements no UE feature. The proposed UE card animation, Sequencer and material/shader presentation remain future work.

## 0.3.1 action-freeze repair

Fixed controls remaining disabled at action windup: default browser timers now call through their global host. Scheduling, painting or completion callback failures cancel remaining presentation and rebuild the view from the already committed game state, without retrying attacks or charging twice. Errors remain in the console and a visible recovery notice explains what happened.

To retain a run in an older open page, first Exit/Rollback at the top, save on the map, then Ctrl+F5 to the current **HTML 0.4** and Continue. Check basic skills, ordinary attacks and guard to confirm playback ends and the next action becomes available. The two-character Vitruvian/Dancer fixture was added to the 0.3.1 regression tests.

The earlier 85 tests bypassed the real default timer path with injected clocks and did not prove browser success. Updated app tests load the shipped browser-global module and default Playback with receiver-sensitive timer doubles. These Node doubles still do not constitute actual browser acceptance.

## Encounter feedback play route

Quick route: Start → advance to Morning → deploy the held Dancer at Home → select it for movement → Fire → Plain → select Gate, choose **Encounter** on the right, then Move Here. Reselect the card after each move.

1. Read the top “Your action / Enemy about to act” banner and matching text/border on the unit. The timeline predicts six upcoming actions; repeated enemy entries are valid, not fixed side alternation.
2. With the Dancer, choose two targets in the attack controls and press Ordinary Attack, then click each guard: the second valid click executes. Separate trails run from the Dancer to each chosen target, followed by impact, numbers and health-bar changes. Commands lock during playback; 1×/2× affects presentation timing only. On a later action, keep the default one-target setting and check that the first valid enemy click attacks immediately.
3. Drag Wind or Mantra onto a friendly unit during a player action, then confirm to observe shield/block grants; follow with an ordinary attack or guard. Hits distinguish absorption/block, One Cut labels true damage, Light displays actual restored HP, and Rain falls onto both sides in three waves. Check a cancelled preview as well: it must consume nothing.
4. Observe the last hit: fallen units and the outcome remain visible before returning to the map. Exit/Rollback works during ordinary playback; final settlement temporarily locks exit and global menus.
5. Press a numeric skill during enemy windup: rejection must not strand the enemy. Navigate away during playback and return using browser Back; restore an operable view of authoritative state instead of stale disabled controls.
6. Resize the window and inspect unit/trail placement. System reduced-motion preference keeps text, static connections and HP feedback while removing moving particles and shaking.

Implementation boundary: local SVG/CSS requires neither Three.js nor a network download. The engine emits transient typed receipts; independent playback reads detached before/after snapshots without changing numbers, timeline or saves. Existing ACT controls remain. When Rain defeats the unit owning the current action but allies survive, the existing alive-unit timeline chooses the next actor. The 0.3.1 baseline recorded 92 passing Node cases (47 original + 20 receipts + 25 playback/input/paint doubles); that historical result does not certify the new 0.4 interactions or real browser visuals. The steps above remain manual acceptance checks.

## Data and limits

`data.js` contains source-labelled card definitions, editable HTML numeric defaults, map, independent appearance/recruit catalogs and short original placeholder dialogues. `engine.js` owns state and validated commands. `app.js` routes inputs; `card-input.js` maintains basic-card targeting and confirmation previews. `home-ui.js` renders independent appearance layers; `style.css` / `home.css` provide presentation; `encounter-fx.js` / `encounter-fx.css` play Encounter actions and impacts. Illustrations are SVG placeholders, not imported models or animation assets.

The explicit source-card values remain separate from HTML temporary numbers. Numeric controls apply when starting a new run. The selected nine ABC cards implement only the effects listed in each card's coverage text. Unsupported abilities are not silently treated as complete. The printed faith formula's jump at 80 is retained rather than silently smoothed.

Still outside this version: all remaining PDF cards/abilities, direct/multiple-B synthesis, complete ABC depletion/debt rules, full era/status precedence, ruler patrol/threat/reinforcements, affection buffs, additional campaign-loss conditions, formal route selection, actual GAS/UE runtime and finished art. Some extreme editable profiles can make a run impractical; default values are the reproducible fixture. Home/recruit selections are currently run-scoped; permanent progression remains a later design decision.

Old version-1 saves receive only absent new home/FPS fields. Existing invalid identities are rejected. Previously synthesized cards in an old save are not guessed or moved retroactively; start a fresh 0.4 run to verify the corrected synthesis flow.

## Proposed UE continuation — no implementation in this task

Use **CommonUI for the screen and input stack plus one reusable 3D presentation stage**. These solve different problems. Home's three tabs should switch content inside the page instead of calling OpenLevel for every tab. CommonUI supplies activatable UI/input routing. See [Epic CommonUI overview](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-advanced-multiplatform-user-interfaces-with-common-ui-for-unreal-engine).

The display stage may be a small dedicated .umap, loaded/streamed when needed. Start with a direct presentation camera in the main viewport, covered by UI, and restore the sandbox camera/selection on return. Isolate stage lighting, collision and input. Use SceneCapture2D and a Render Target when an embedded viewport or multiple simultaneous views justify the extra rendering/memory cost. Stop capture when hidden; do not manually recapture in addition to every-frame capture. References: [Level Streaming](https://dev.epicgames.com/documentation/unreal-engine/level-streaming-overview-in-unreal-engine?lang=en-US), [CaptureScene](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/USceneCaptureComponent2D/CaptureScene).

If home later becomes a freely navigable base with its own gameplay and many NPCs, or the sandbox must be unloaded for memory, an independent full level becomes appropriate. Its preparation menus can still use CommonUI.

Maintain separate main-protagonist, ACT-recruit, FPS-recruit and ABC-instance IDs. Resolve each selection to its own definition and model/outfit/weapon/compatible-pose soft references. The display Actor is transient; saved state stores IDs. Do not duplicate ACT skill-table ownership into a general appearance asset. Ignore stale async model-load completions after changing the selected character. Changing a display weapon is not automatically changing a recruited character's combat equipment.

For dialogue, use static StoryId/line/speaker/text/choice/condition/next/effect definitions, with optional voice, portrait and sequence soft references. A story service validates choice commands and invokes the world/resource services; completion/effect IDs prevent duplicate rewards. A UI page displays state only. Save versioned progress/choices/completion facts, not Widgets or Actors; first support safe saves before/after dialogue. Use LevelSequence for camera/action performances, not as the sole owner of branching or rewards: skip/replay/backward scrubbing must not regrant effects. See [Epic Sequencer Event Track](https://dev.epicgames.com/documentation/unreal-engine/cinematic-event-track-in-unreal-engine).

For future Encounter presentation, the user requests Sequencer-driven hand-card motion with material/shader effects. Basic cards bind their visuals to the card/environment and chosen targets; Rain uses the sky/arena rather than a C character. Gameplay commands continue to own validation, cost and one-time resolution. Animation playback, skipping or replaying must not apply a card again.

These are requirements and recommendations for a later architecture review/ADR. This task enables no UE plugin, changes no map or engine dependency, and writes no C++.

## Validation

With Node.js available, from the repository root:

```powershell
node --test Prototypes/HTML/engine.test.cjs Prototypes/HTML/encounter-events.test.cjs Prototypes/HTML/encounter-fx.test.cjs Prototypes/HTML/card-input.test.cjs
node --check Prototypes/HTML/app.js
node --check Prototypes/HTML/home-ui.js
node --check Prototypes/HTML/encounter-fx.js
node --check Prototypes/HTML/card-input.js
```

The engine suite includes daily timing, seeded synthesis, group movement, dialogue completion/cancellation/rollback, independent identity domains, save migration and both battle loops. Automated state checks do not certify browser double-click dispatch, long-press, dragging, layout, keyboard feel or actual UE behavior. Record manual results for the amendment steps and the 0.4 interactions above when playing.
