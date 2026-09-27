# ADR-0017: Manual NPR authoring backend

Status: Accepted for the initial manual authoring slice, 2026-09-27.

The manual mesh/semantic texture interface must not silently interpret arbitrary PBR resources as Chen-specific reference encodings. Add a separate opaque Manual backend, sharing named HLSL math and the existing runtime Profile/component map transport. Reference remains compatible and unchanged by default.

Editor-only UPadmaNPRCharacterRecipe records mesh, per-slot inputs and output destination. Generated runtime profiles do not reference this editor object. Each converted slot has an adapter material and source MI; artistic overrides are stored in runtime Profile maps. No runtime dependency on Python, Slate or the recipe is introduced. bManualResponse defaults false and requires the map-based response plus a Manual parent. Manual currently uses explicit-key-light Unlit evaluation and does not claim scene-shadow visibility or parity with the reference materials.

Studio separates transient preview from persistent generation. Generate validates and compiles before saving; output collisions are rejected and originals are preserved. In-place regeneration is deferred. Apply remains an explicit operation on one matching actor mesh; editor-applied MICs and level changes are saved by the user. Changes to generation inputs invalidate the cached generated result. Existing DA/Reference Apply/Restore remains accessible from the new panel.

The preview uses SEditorViewport/FPreviewScene and skeletal animation evaluation, not a static screenshot. Weather, automatic matching and outline are not part of this slice. See [manual authoring](../../Plugins/PadmaNPR/ManualAuthoring.md) for capability and verification limits.
