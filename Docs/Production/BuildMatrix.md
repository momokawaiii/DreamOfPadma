# Build Matrix

- Chinese companion: [BuildMatrix.zh-CN.md](BuildMatrix.zh-CN.md)
- Status: Offline Demo acceptance target; packaged acceptance remains pending
- Scope: [ChapterZero](../Rules/ChapterZero.md)

| Configuration | Required evidence |
|---|---|
| Editor Development / PIE | Compilation, targeted automation, real interaction and clean teardown; cannot substitute for packaged checks. |
| Win64 Development | Clean-location offline installation, complete golden path and failure/continue checks. |
| Win64 Shipping | The same content and behavior in an offline release-quality candidate. |
| DebugGame / Test | Optional diagnostics; no separate delivery requirement. |

## Package purpose and asset gate

Declare **PrivateLearning** or **PublicRC / Steam-target** separately from build configuration. A Shipping executable alone neither proves release readiness nor makes a private learning package a public delivery.

For PublicRC / Steam-target, reject `civili` and known prohibited-source content in the reachable/cooked dependency graph. Replacement must be real, not a filename/path rename. Preserve source package/provenance and audit from shipped maps/catalogs/root assets through hard, soft and transitive references: Blueprint defaults, DA/DT, AnimBP/Montage, Niagara, materials and LevelSequence, plus the final cook/package manifest. Referencer chains identify failures. A filename grep alone cannot pass this gate.

Manually replace references through ordinary Content Browser/Details; no dedicated replacement tool. [ACTMigrationMatrix](../Content/ACTMigrationMatrix.md) records migration/replacement evidence. Required priority and verified playable promotion are different: independent behavior verification and the applicable asset audit must pass before promotion. No such complete audit is implemented or passed by this documentation change. Actual Steam submission/integration is deferred.

## Required in both final packages

- Logs identify revision, configuration, package purpose and content/map versions.
- Install/extract to a clean location; launch without Editor, project source paths, uncooked assets or Steam. Installer technology remains open.
- Actually play menu/opening → Chapter Zero's map/calendar/ABC/synthesis/event/dialogue → Encounter → ACT → occupy court → ending. ACT exercises the full one-character P0, including representative projectile/summon/execution; see its [contract](../Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md).
- Verify save/continue, failure/exit/startup rollback, tutorial completion/skip/replay and same-run reward idempotency.
- Load the fixed baked 3D map with matching MapKey; do not regenerate PCG during packaged play.
- Verify keyboard/mouse, nested activation/Back/cancel, readable final Chinese, scene travel and clean shutdown. No hand-controller certification or multiplayer testing is required.
- Final values come from approved card/rule data. RC presentation includes production art, animation, VFX and **production audio**. Voice acting/full English are optional. No audio or placeholder audio is allowed only during functional development.
- Record user visual/feel acceptance and actual failures. Build or automation success alone cannot establish packaged end-to-end acceptance.

## Early baseline and diagnostics

Start packaging and measurement early. Record hardware, resolution/settings, minimum acceptable frame rate, load time, memory and PCG instance counts on the development machine, then use generous agreed budgets. These fields are currently unset; no 1080p/60 FPS threshold is approved. Once these baselines are agreed, crossing one pauses further content additions until the cause is diagnosed; generous limits do not remove this gate. Broader optimization is later work. Crashes, softlocks, severe leaks, loading failures or runaway spawning require diagnosis before more content is added.

Use current Scripts/Build profiles where supported; inspect their capability first. Evidence belongs in Artifacts. TASK-053 runs documentation checks only: no new UE build, PIE, asset import, cook or package acceptance is claimed.
