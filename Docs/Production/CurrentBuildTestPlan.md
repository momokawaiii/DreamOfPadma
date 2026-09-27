# Current UE Validation Plan

- Chinese companion: [CurrentBuildTestPlan.zh-CN.md](CurrentBuildTestPlan.zh-CN.md)
- Document ID: TESTPLAN-CURRENT-001
- Version: 1.0
- Updated: 2026-09-27
- Status: Focused on the retained Chen ACT scene, turn-based maps and PadmaNPR plugin

This is the small validation plan for the retained repository scope. The old HTML prototype, generic model-preview map, independent map-authoring map and generic ACT template assets are retired. Their historical records are not current test entry points.

## Retained entry points

| Area | Entry point | Boundary |
|---|---|---|
| Turn-based world | `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld` | Main map, strategy UI, cards, preparation, divination and authored world data |
| Turn-based battle | `/Game/Padma/MVP/Playable/Maps/L_PadmaBattle` | Battle dependency entered from the world map; do not delete independently |
| Chen ACT | `/Game/Sandbox/ACT/Training/Maps/L_ChenACT` | Chen, three wooden targets, actions, camera, FX, analytics and render study |
| PadmaNPR | `Plugins/PadmaNPR` | Runtime/editor modules, profiles, materials, shaders and authoring tools used by the retained character path |

Shared `Source`, `Config`, `Scripts`, `Content/ThirdParty` and documentation remain only when they support these entry points or repository governance. The `MVP` directory name is historical; the retained `MVP/Playable` packages are the current turn-based implementation.

## Required checks

1. Confirm the three map packages exist and no additional project `.umap` is introduced without an explicit scope decision.
2. Open `L_PadmaWorld` in PIE and verify map entry, inspection, resource/card interaction and travel to `L_PadmaBattle` without a stale preview route.
3. Open `L_ChenACT` in the Editor and PIE. Verify the scene-owned Chen and three targets, movement/action reset, weapon lifecycle, camera and training HUD. Rendering and hand-feel remain separate manual evidence.
4. Build the project with `PadmaNPR` enabled and inspect module/plugin load errors. A successful build does not certify visual parity or packaging.
5. Run the repository checks after Markdown or structure changes:

```powershell
pwsh -NoProfile -File .\Scripts\AuditDocs.ps1
pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict
pwsh -NoProfile -File .\Scripts\AuditAssets.ps1
node .\Scripts\Editor\ExportPlayableData.cjs --check
```

6. When a UE commandlet is run, record the branch, `HEAD`, dirty state, map, engine version, report path and any project-specific errors. Use one Unreal process at a time and close the Editor before asset/resource writes.

## Evidence limits

Repository checks establish structure and references only. They do not establish PIE behavior, GPU output, visual fidelity, Cook/package success, clean-install behavior or release readiness. Existing Chen action failures and incomplete source-parity checks remain visible in [ProjectState](../ProjectState.md) and [TASK-056](Tasks/TASK-056-Chen-ACT-Actions.md).

## Source references

- [Current project state](../ProjectState.md)
- [Retained scope and cleanup record](ProjectCleanup.md)
- [Native turn-based guide](../Content/NativePlayableDemo.md)
- [Chen ACT/render lab](../Content/ChenACTRenderLab.md)
- [PadmaNPR architecture](../../Plugins/PadmaNPR/Architecture.md)
- [Build matrix](BuildMatrix.md)
