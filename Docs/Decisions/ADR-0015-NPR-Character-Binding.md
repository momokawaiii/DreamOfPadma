# ADR-0015: Plugin-only NPR character binding

Status: Accepted for the minimal character rendering workflow, 2026-09-27.

## Decision

Keep the plugin independent of gameplay and engine forks. Source MIs own texture bindings. A typed character Data Asset owns artistic parameters per named material slot. Studio applies the Profile through a native editor library into persistent derived MICs, with transactions and restoration. Runtime uses independent MIDs. The mesh owns dynamic light/head data in reserved Custom Primitive Data indices 0..19, avoiding global MPC interference and transient editor material serialization.

One component binds one skeletal mesh and an explicit directional light. Profile identity is metadata; it does not imply an Atlas/GBuffer implementation. Cloth keeps its existing profile contract. The minimal character backend uses stock Unlit materials; no UE 5.8 Toon BSDF or built-in Toon profile features are used. Future native Substrate work requires a separate proven extension seam.

## Consequences

Daily authoring needs no Python import. A Profile must be reapplied after editing its static parameters. Generated MIs are caches, not authoring authority. The CPD range must be reserved by integration. Hair shadow is initially helper geometry and bangs use masked temporal dithering; they are not physical projection/refraction. See [Character contract](../../Plugins/PadmaNPR/Character.md) for controls, lifecycle and validation evidence. Runtime build/GPU tests do not prove packaging or final visual parity.
