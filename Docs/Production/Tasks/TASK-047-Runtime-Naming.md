# TASK-047 Runtime Responsibility Naming

- Chinese companion: TASK-047-Runtime-Naming.zh-CN.md
- Status: Verified (working tree; user playtest remains TASK-046)
- Primary: root; current Local codex/mvp-demo-foundations
- Authorization: user requests professional runtime filenames/types instead of Demo naming; user saved and closed the Editor for migration/build.

## Scope and acceptance

Rename reusable runtime source under Source/DreamOfPadma/{Public,Private}/MVP into responsibility-based Core/Game/Gameplay/Presentation/World/UI folders. Replace Demo/Playable runtime symbols with domain names; isolate GameMode and SaveGame declarations in their own headers. Update includes, generated-header names, native tests, editor authoring callers and current documentation references. Keep legacy Source/.../Demo fixtures, content package paths, stable gameplay IDs, serialized field names, save slot/app/version and game behavior intact.

Root owns these source moves and dependent source include edits, Config/DefaultEngine.ini exact CoreRedirects, Scripts/Editor export/author/migration scripts, a versioned migration manifest, this task pair, a naming policy pair, current native guide/index/state/changelog and affected architecture references. Binary assets under Content/Padma/MVP/Playable may be loaded/re-saved using UE only to migrate reflected references; no path moves or third-party changes. Source paths are verified inside the workspace before moving; existing destination files must never be overwritten.

Acceptance: no Demo/Playable delivery-stage words in the migrated runtime type/file names; new names describe responsibility. Exact redirects preserve old DataTable/DA/GameMode/SaveGame types. Validate old save bytes, existing authored rows and both maps after migration; Editor build, existing 11 focused tests, a real map/battle roundtrip, AuditDocs and strict project validation. One bounded read-only compatibility review; no broad gameplay review. No commit/merge/push.

## Evidence

The [naming policy and complete file/type map](../../Architecture/RuntimeNaming.md) and `Scripts/Migrations/RuntimeNaming047.json` record 39 moved files, four new split/compatibility files, 48 renamed symbols and 39 exact redirects. `FPadmaACTSkillRow` remains the ACT authoring binding row; `FPadmaACTSkillEffectRow` names the separate effect schema. The additional focused regression is `Private/Tests/Compatibility/PadmaRuntimeNamingTest.cpp`.

Current native runtime roots are Core, Game, Gameplay, Presentation, World and UI. `Game/Framework` separates GameMode/controller; `Game/Save` separates the envelope and compatibility reader. Dependent legacy/test includes and editor export/authoring scripts use the new contracts. Paired current architecture/module/model guides, index, project state and changelog are synchronized. Historical TASK evidence and fixture content names remain unchanged.

- Editor target: UE 5.8.2 `DreamOfPadmaEditor Win64 Development`, succeeded; local `Artifacts/TASK-047/editor-build.log`.
- Automation: `Automation RunTests TASK046+TASK047`, with `-PadmaNamingLegacySave`; 12 succeeded, zero warnings/failures, report created `2026.09.09-08.55.26` UTC at `Artifacts/TASK-047/Automation/index.json`.
- Original pre-rename binary produced the isolated `PadmaNamingCompatibility047` slot with default values. The new typed reader loaded those old bytes. A separate synthetic legacy payload uses its old header and legacy struct names while UE writes accurate property sizes; non-default RNG/calendar/resources/preparation, populated cards/nodes/profile/mobility/discard and the entire nested run compare equal after load. Re-saving/reloading the current envelope also matches. The user's normal save slot was not overwritten.
- Initial direct `StaticLoadObject` checks exposed that plain string lookup does not apply CoreRedirects; UE's generic SaveGame reader has the same class-string issue. Tests now check the redirect registry, and production loads bytes through the exact current/legacy envelope adapter before existing app/version/state validation. No serialized fields, format version or gameplay values changed.
- `MigrateRuntimeTypes.py`: all 18 table contents unchanged; 17 renamed-row tables plus catalog re-saved (18 assets), both maps re-saved with `PadmaGameMode`; row-type picker tags current. Evidence: `asset-migration.json` / `.log`; original package backups are in ignored `content-before`.
- Updated authoring scripts reran successfully: zero existing assets overwritten, both maps preserved, catalog validation returned no error (`authoring.log`).
- Native rendered world → battle → exit/rollback → world passed; new GameMode/screen loaded, pending battle reset to zero, same 21 run cards after return (`roundtrip.log`, `roundtrip.png`).
- Focused independent compatibility inspection found no source mapping/field/default drift. Its non-default old-state evidence gap was closed by the passing synthetic payload checks above. No broad gameplay re-review.
- Documentation audit, strict project validation and scoped whitespace checks are recorded in `Artifacts/TASK-047/validation.log`.

Limits: this establishes Editor/runtime compatibility on the current UE version. Packaged/cooked save migration was not exercised; UE's struct redirect behavior differs there, so do not infer packaged compatibility from the Editor test. TASK-046 user interaction acceptance and production art remain pending. No new lesson requested, no Git staging/commit/merge/push, no third-party or legacy fixture asset migration.
