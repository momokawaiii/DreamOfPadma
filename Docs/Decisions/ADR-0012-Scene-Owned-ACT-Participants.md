# ADR-0012: Scene-owned ACT participants

- Status: Accepted
- Date: 2026-09-22
- Chinese summary: [ADR-0012-Scene-Owned-ACT-Participants.zh-CN.md](ADR-0012-Scene-Owned-ACT-Participants.zh-CN.md)

## Decision

Chen's `L_ChenACT` owns its placed character and training targets. Blueprint/level
components hold meshes, materials, animation classes and transforms. They are visible
before PIE. `APadmaACTMeleeLab` references these actors explicitly; it must not silently
spawn replacements when required references are missing. A separate render-only clone
is unnecessary: the same placed actor is the authoring object and runtime participant.

`SceneSpec` is persistent authoring data; `Spec`, ASC abilities/effects, movement,
hit windows and analytics are runtime state. C++ validates and initializes the latter
without replacing the authored character presentation. `Apply Scene Presentation`
is an explicit initial-assembly button, not a per-play overwrite. Empty instances can
bootstrap from their Blueprint's `SceneSpec` during construction.

`UPadmaCombatComponent` distinguishes borrowed scene participants from owned spawned
participants. Cleanup releases both sets' GAS state but destroys only owned actors.
F8 restores the captured initial actor transforms and reinitializes the same objects.
Weapon mounting/dissolve still runs as presentation behavior, preserving authored
mesh/material choices and mount corrections. Generic encounters and legacy test
fixtures retain explicit spawn-based operation; the hidden card/environment source
remains a transient implementation detail, not a visible replacement character.

## Asset boundary

Runtime consumes cooked UE assets and native code. Daily authoring consumes existing
`Content` assets and versioned authoring configuration under `Scripts/Editor`.
`Artifacts` contains optional evidence/backup/extraction output, never required input
for opening, editing or playing the authored map. Explicit offline extraction and
reimport utilities may require source files; they are not startup/configuration steps.
Historical FBX import metadata is provenance, not a runtime file dependency.

## Verification and limits

Require an Editor build, repeated registration/reset/exit tests, fresh-process map
reload and reference checks. Visual material quality and packaged behavior are separate
acceptance. See [authoring guide](../Content/ChenACTRenderLab.md) for editable fields
and the current task for evidence.
