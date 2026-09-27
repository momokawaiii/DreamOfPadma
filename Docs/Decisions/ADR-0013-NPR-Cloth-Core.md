# ADR-0013: Cloth Core and Explicit Profile Application

- Date: 2026-09-22
- Status: Accepted scope; character visual acceptance pending
- Chinese summary: [ADR-0013-NPR-Cloth-Core.zh-CN.md](ADR-0013-NPR-Cloth-Core.zh-CN.md)

## Decision

Extend the independent PadmaNPR plugin with a Runtime module for shader path registration, typed Cloth data and an explicit MID factory. Editor depends on Runtime, never the reverse; neither depends on DreamOfPadma gameplay. Load Runtime at PostConfigInit so packaged material compilation can resolve plugin shader includes. No Renderer module or custom Pass is introduced.

Use Toon and ZMDRender as reference conventions rather than one universal source graph. Keep .ush math independent of engine-private Toon APIs. The initial integration is a stock DefaultLit artistic hybrid, not a custom BSDF or per-light cel implementation. UE 5.8 Toon BSDF/Profile/Atlas remain excluded.

Cloth defaults resolve into enabled per-group material-slot overrides. The Profile owns artistic values. Texture-binding MIs own resources, encodings and channel selection. The MID factory snapshots resolved values into a new instance and never changes a mesh; the caller owns assignment, lifetime and restoration. General Style/Character/Binding assets may be introduced later without pretending they already exist here.

## Consequences

The first shader module can be learned and tested independently. No automatic migration or scene mutation is required. Explicit initialization and light-direction updates are necessary. The current editor-only project plugin restriction is removed for Runtime availability; Editor remains excluded from non-editor targets by its module type. Cook/package and a second-project install still require validation.

Contracts and acceptance boundaries: [Cloth guide](../../Plugins/PadmaNPR/Cloth.md).
