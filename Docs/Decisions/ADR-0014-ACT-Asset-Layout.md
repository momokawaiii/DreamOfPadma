# ADR-0014: Role-based ACT asset ownership

Status: Accepted, 2026-09-22. Chinese summary: [ADR-0014-ACT-Asset-Layout.zh-CN.md](ADR-0014-ACT-Asset-Layout.zh-CN.md).

The user requested consolidation of the Chen ACT sandbox and retirement of historical
Attack01/Centimeter organization. Adopt the role-based [asset layout](../Content/ACTAssetLayout.md):
character rules/presentation/animation/art, independent weapon art/definition, and
training map/environment/targets. Keep the scene-owned participant contract of ADR-0012.

Preserve stable gameplay IDs, first-attack behavior, user-authored presentation,
animation variants and distinct FX implementations. Retire the duplicate saved
single-attack fixture chain; tests use transient objects and canonical shared content.
No additional GAS implementation or duplicate production catalog is introduced.

Use Unreal AssetTools with a full hash-verified backup, per-package dispositions,
fresh reference validation and runtime regression. Interrupted rename residue is
removed only after confirming no retained/external inbound references. AssetTools
and AssetRegistry are private editor-only dependencies of the existing editor-gated
ACT authoring adapter, never Core or packaged gameplay dependencies.

Sandbox remains experimental. File migration, Editor build and automation are not
cook/package or visual fidelity proof. Raw extraction evidence and backups stay
outside the mounted runtime content and are not used for daily gameplay initialization.
