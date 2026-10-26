# Contributing

## Propose a change

Open a branch and pull request describing the concrete behavior change and how it was checked. Include the relevant model, generated-code, and toolchain versions for controller changes.

## Verify changes

From the repository root:

```sh
python3 tests/test_can_callback.py
python3 tests/check_docs.py
```

For firmware changes, also rebuild with the matching target toolchain and record bench checks when hardware is available. Host tests use stubs and do not replace target tests.

## Preserve model traceability

Make controller behavior changes in the matching Stateflow/model source before regenerating C. Clearly label any temporary manual edits to generated code. Recheck initialization, fault thresholds, recovery, sample timing, and relay actuation.

Keep original models, weights, configuration files, and measurement evidence. Avoid committing new IDE build output, machine-specific paths, caches, or credentials. The ignore rules do not remove previously tracked artifacts.

## Report results precisely

Distinguish historical screenshots from results obtained on the current commit. State inputs, tool versions, board configuration, and acceptance criteria. Record SOC errors in percentage points when comparing normalized SOC, and separate observed timing, assumed budgets, and measured physical switching times.
