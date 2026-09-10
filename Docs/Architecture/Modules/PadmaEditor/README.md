# PadmaEditor

- Document ID: ARCH-MODULE-EDITOR-001
- Version: 0.2
- Status: Existing authoring scripts/adapters; custom story editor deferred; no standalone Editor module
- Chinese companion: [README.zh-CN.md](README.zh-CN.md)
- Owner: Editor and Content Tools Agent
- Allowed ownership: Scripts/Editor and editor-gated authoring/preview adapters in the current runtime source; exact write paths per TASK

## Current tools

The project already has Python authoring scripts, ACT/model definitions and map preview/import/validation adapters. “No project-owned editor code” is obsolete. See [WorldMapAuthoring](../../../Content/WorldMapAuthoring.md), [ModelAuthoring](../../../Content/ModelAuthoring.md) and [ACTAuthoring](../../../Content/ACTAuthoring.md) for actual entry points.

Tools create/edit validated definitions and present reports. They do not own current run state or a second copy of combat/synthesis formulas. Reuse runtime evaluators for previews. Keep source configuration and generated output distinct; respect existing authored edits on reruns.

## Near-term responsibility

- Validate IDs, references, row/mode compatibility and configured values before consumption.
- Improve the map generation/bake/metadata/cook path under [ADR-0010](../../../Decisions/ADR-0010-Offline-Demo-Content-and-Map.md).
- Make errors actionable by asset/field/reason; preserve prior valid content on failed import/generation.
- Feed required chapter runtime through ordinary typed assets/tables while the specialized editor is postponed.

Editor APIs may depend on runtime contracts; packaged runtime must not depend on editor-only classes. Existing assets are not a reason to introduce a new standalone module. Add one only when real editor dependencies require that boundary.

## Deferred story editor

The accepted future design is a separate ChapterSourceAsset with UEdGraph, compiled to runtime ChapterDefinition. A restricted DAG uses Start, Dialogue, Choice, Condition, TutorialAction, Effect and End, with typed execution edges and parameters in Details. No arbitrary Blueprint calls, general data pins or parallel scripting language.

Potential UI components are FAssetEditorToolkit, SGraphEditor, Details, FScopedTransaction and FMessageLog. This is preserved design intent, not a current dependency or task to implement. Story graph editing/compilation, text/localization entry tooling and PresentationCue authoring UI are all deferred by the latest user decision. Do not let them block the Demo runtime.

## Contracts, checks and risks

Public tool results identify source version, asset/field, severity and failure reason; previews return the same typed calculation result as runtime. Import/export round-trip checks apply when that pipeline exists.

Test duplicate/missing IDs, invalid references/modes, preview parity, failed-edit preservation and bake/cook consistency. Save only stable compiled content IDs at runtime; never UEdGraph or temporary preview Actors. Tools introduce no network authority.

Learning topics when requested: Unreal reflection, editor extensions, validation, transactions, graph schemas and compilation. Main risk: building a second large editor product before the playable Demo path is complete.
