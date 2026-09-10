# Card Synthesis Rules

- Chinese companion for user reading: `Docs/Rules/Synthesis.zh-CN.md`

This document is the rule contract. Production numerical coefficients remain configurable data and must not be invented in code. On 2026-09-09 the user authorized editable temporary numbers for the HTML rule prototype only; they do not become UE production defaults.

## Material lifecycle (accepted 2026-09-09)

After a synthesis attempt resolves, all participating A and B cards enter the discard pile, on both success and failure. Failure grants no reward and paid Flow is not refunded. Validation rejection is not an attempt and consumes neither cards nor resources. A tendencies accumulate into each attached B field once when the calendar enters Morning each day; previewing or reopening a screen does not accumulate them.

On success, C appears directly at the participating A/B pair's current node, including a destination they previously moved to. Preserve that stable NodeId before discarding the inputs. C does not enter hand and requires no second deployment action or fee. This is the user's correction to the first HTML implementation; imported or initially held C cards retain their separate hand-deployment path.

## Resolution order

1. Validate both input card IDs and their categories.
2. Combine the input label/vector representation.
3. Score candidate C cards.
4. Apply Softmax to obtain candidate selection probabilities.
5. Select one candidate C using a seeded random stream.
6. Calculate success from resonance distance, receptor affinity, global entropy, and faith.
7. Resolve success or failure.
8. On failure, grant no reward. If Sigma dice are retained, resolve only a separately documented failure-side consequence.

Sigma dice must not turn a failure into a reward, partial success, or a hidden success path. Any remaining non-reward use must be explicitly defined in data and this contract.

## API contract

Preview and execution must call the same service and return the same calculation fields. The result should include:

- selected candidate
- candidate probabilities
- success probability
- random rolls or roll identifiers for debug builds
- success/failure
- failure consequence, if a non-reward consequence is configured
- seed or stream position

## Test cases

- Invalid input
- No candidate
- One candidate
- Multiple candidates
- Zero and maximum entropy
- Faith boundaries
- Each receptor type
- Success
- Failure with no reward
- Configured failure consequence, if Sigma dice remain in the rule set
- Same seed produces the same result
