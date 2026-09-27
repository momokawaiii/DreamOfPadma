# TASK-055 Chen Attack01 source FX

- Status: Review; native playback is verified, source visual fidelity remains under review.

## Current outcome and decisions

Attack01 uses native GAS Montage playback and AN/ANS timing, including changed playback rates, repeated combo and cancellation. Source emitter/texture bindings and attachment transforms guide the UE effects; imported data alone is not proof of original rendering parity.

Retain the existing Attack01 assets/tuning when extending Chen actions. Later character-wide work belongs to [TASK-056](TASK-056-Chen-ACT-Actions.md). Do not restore superseded effect layers or bypass Montage lifecycle using an independent animation clock.

## Acceptance and evidence

Source mapping, material/particle changes, native tracing, Montage/Notify verification, hit rectangle, subtle glow and prefab-position corrections are retained with their exact artifact paths in [historical evidence](../ProjectCleanup.md).

## Remaining work

Source-fidelity review and actual affected-action visual/runtime acceptance. Build/import success is not complete motion/FX parity or package acceptance. Preserve existing source/asset edits; no Git integration implied.
