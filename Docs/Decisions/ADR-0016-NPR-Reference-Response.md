# ADR-0016: Preserve reference response as an explicit material variant

Status: Accepted for implementation; visual acceptance is tracked in TASK-057.

## Decision

Keep the existing generic NPR parameter contract. Add an explicit per-slot reference response using `M_NPR_<Type>_Reference` parents. It preserves audited source parameter semantics instead of forcing different algorithms into approximate generic controls. Append Cloth, HairLine, Brow and EyeShadow surface identities without changing existing enum values.

For reference slots, the profile's named scalar/vector maps are authoritative; the source MI owns textures and material topology. Generated editor MICs are cleared and rebuilt from the profile, while runtime MIDs receive the same values. Removing a profile override must return to the source MI's inherited value, not retain stale generated parameters. Generic controls are not injected into reference slots.

The component continues to reserve CPD 0–19. Existing spare components carry the signed reference face-SDF value at 7 and its validity at 11. The profile optionally owns a UCurveFloat evaluated using UE's native interpolation, including weighted tangents. The angle is acos(dot(surface-to-light, calibrated head forward))/pi; the sign uses the world-Z component of cross(light travel, head forward). Frame validity lets standalone material previews fall back to authored values.

Reference graph lowering uses stable sorted argument signatures and explicit native tangent-basis inputs. It rejects unsupported live nodes. The Chen Cloth comparison specialization fixes audited static switches; it is not a general material-graph compiler. Engine-dependent sampling, exposure, blend modes and vertex-stage expressions remain explicit integration boundaries.

## Consequences

- Reference maps expose more controls than the generic authoring contract. They are a faithful baseline for later typed UX consolidation, not a claim that every character uses identical algorithms.
- Compiled shaders, persisted assets and fixed-view comparisons are separate checks. Moving-light, head-pose, full-body and runtime acceptance remain mandatory.
- Source helper geometry and section layout may differ from the target's nine sections. Additional source eye/mouth/neck sections cannot be reproduced merely by assigning an existing target slot a different material.
- No engine modifications, custom engine Toon BSDF, Profile Atlas or new render pass is introduced by this decision.
