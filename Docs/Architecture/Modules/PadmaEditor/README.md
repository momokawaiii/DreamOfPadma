# PadmaEditor Module Program Document

- Document ID: ARCH-MODULE-EDITOR-001
- Version: 0.1
- Status: Planned editor-only boundary; no standalone UE module yet
- Canonical language: English for Agent consumption
- Chinese companion: README.zh-CN.md
- Owner: Editor and Content Tools Agent
- Current implementation: no project-owned editor code yet
- Parent architecture: ../../ProgramArchitecture.md and ../../DataDrivenArchitecture.md

## 1. Purpose

PadmaEditor contains editor-only tools that make data-driven content safe to author, inspect, validate, and preview. It must improve the content pipeline without becoming a runtime dependency.

## 2. Responsibilities

- Data asset and DataTable validation.
- Stable ID, reference, tag, curve, and probability checks.
- Card activation-context, Encounter action-timeline/initiative/priority/window rules, ACT RealTimeAction input-profile, five-visible-slot page, and numeric-slot checks. The exact Encounter rules, lifecycle probabilities, and total page count remain deferred.
- Card, synthesis, outcome, world, encounter, AI, and story preview tools.
- Import/export adapters when the project chooses CSV, JSON, or a mixed pipeline.
- Content audit reports and duplicate/opaque-name warnings.
- Editor-only GM helpers and calculation trace views.
- Asset creation templates and naming enforcement.
- Documentation/schema links surfaced to content authors where useful.

## 3. Non-responsibilities

PadmaEditor must not own:

- Packaged runtime rules.
- Runtime save or network behavior.
- A second implementation of synthesis or combat formulas.
- Content that bypasses the canonical data contracts.

Editor previews must call the same pure calculation services as runtime.

## 4. Public contracts

- Validation report with asset ID, field, severity, and reason.
- Import/export result and source version.
- Preview request/result for card, synthesis, outcome, world, and encounter data.
- Preview requests for basic non-A card rejection, Encounter action-timeline windows, ACT RealTimeAction `Tab`/blur/1-10 card-only bullet-time, and ABC-card active abilities outside battle.
- Content audit report.
- Editor command wrappers that call normal command paths where possible.

## 5. Data and runtime ownership

PadmaEditor owns authoring-time tooling state and reports. It reads and validates definitions owned by Core, Gameplay, World, and UI content pipelines. It does not own the final runtime state.

Generated output, intermediate files, and local editor caches remain outside the source-controlled content contract.

## 6. Dependencies and integration

PadmaEditor may depend on runtime data contracts and editor APIs. Runtime modules must not depend on PadmaEditor. All validators must use the same schema version and stable ID rules as runtime.

## 7. Tests and debug evidence

Required tests:

- Duplicate and missing ID detection.
- Broken reference and invalid tag detection.
- Probability, curve, threshold, and cost range checks.
- Basic non-A battle-only contexts, player action-turn/one-card gates, Encounter action-timeline windows, ABC outside-battle contexts, and ACT RealTimeAction `Tab`/blur/1-10 five-visible-slot/page mappings. Validators must not assume a total page count or lifecycle probabilities.
- World graph and era/chapter hierarchy checks.
- Preview/runtime parity.
- Import/export round trip when an import pipeline exists.

Reports should be saved as review evidence without embedding absolute local paths in content.

## 8. Implementation stages

1. Keep the PowerShell audit scripts as the initial static layer.
2. Add a small editor validator for the first data schema, including card activation contexts.
3. Add preview tools after the pure calculation service exists.
4. Add an ACT RealTimeAction `Tab`/blur/1-10 input-profile/page/slot validator when that mode profile is defined; leave total page count and lifecycle probabilities configurable.
5. Add import pipeline only after the source format is decided.

## 9. Learning targets and risks

Learning targets: Unreal editor extensions, asset tooling, reflection, data validation, import pipelines, and content-authoring UX.

Main risk: letting editor convenience create data that runtime cannot validate or load.
