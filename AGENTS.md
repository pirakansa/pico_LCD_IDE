# AGENTS.md

This document is the README for AI coding agents. It complements the human-facing README.md so that agents can develop safely and efficiently.

---

## Scope

* Treat this file as the primary agent policy for the repository root.
* Prefer explicit user instructions when they conflict with general guidance.
* Keep changes minimal and aligned with the current codebase; do not perform broad cleanup unless requested.

---

## Build & Validate

* Build: `vorbere run build`
* Test: `vorbere run test`
* Check: `vorbere run check`
* Cleanup: `vorbere run clean` or `cargo clean`.
* For CLI usage and command examples, see the Usage section in README.md.

---

## Project Structure

This repository is a Raspberry Pi Pico firmware project written in C and built with CMake.

Current notable structure:

```
.
├── CMakeLists.txt
├── vorbere.yaml
├── src/
│   ├── main.c
│   ├── events/
│   ├── lcd/
│   ├── pico/
│   └── usb/
├── Pico_code/
├── pico-sdk/
└── pico-examples/
```

Rules:

* Keep application code under `src/`.
* Keep module-local build definitions in the existing `src/*/CMakeLists.txt` files.
* Do not introduce new top-level directories unless the user asks for that explicitly.
* Treat `pico-sdk/`, `pico-examples/`, and `Pico_code/` as vendored or upstream-derived content unless the task clearly requires editing them.

---

## Setup And Build

* Primary task runner: `vorbere run <task>`.
* Build command: `vorbere run build`.
* Direct build flow currently used by the repository:

```sh
mkdir -p build
cmake -DPICO_BOARD=pico -S . -B ./build
make -C ./build
```

* Cleanup command: `vorbere run clean`.
* If a task requires additional tooling, prefer updating `vorbere.yaml` instead of inventing ad-hoc undocumented commands.

---

## Validation Policy

Agents must check the real task definitions before claiming validation coverage.

Current repository state:

* `vorbere run build` performs a real firmware build.
* `vorbere run check` is currently a placeholder command.
* `vorbere run test` is currently a placeholder command.

Rules:

* Run `vorbere run build` before and after substantive code changes when feasible.
* Run `vorbere run check` and `vorbere run test` as well, but do not describe them as meaningful static analysis or test coverage until their task bodies are implemented.
* If validation is incomplete because a task is a placeholder, say that explicitly in the final report.
* When modifying behavior, add validation that matches the current stack when practical, such as build verification or targeted host-side checks.

---

## Coding Standards

* Use C for firmware changes unless the repository already uses C++ in the touched area.
* Follow the existing naming and file layout in each module instead of imposing a new style.
* Keep `src/main.c` focused on initialization and high-level orchestration when possible.
* Put module logic in the existing module directories under `src/`.
* Extract repeated literals into named constants when doing so improves clarity without expanding scope.
* Avoid unrelated refactors.

---

## CMake And Dependencies

* Prefer editing existing `CMakeLists.txt` files in place rather than introducing parallel build systems.
* Keep library boundaries consistent with the current structure (`libpico`, `liblcd`, `libusb`, `libevent`).
* Do not replace submodule-based dependencies with fetched network dependencies unless the user asks for that change.
* Obtain user approval before adding new external dependencies or updating submodules.

---

## Testing And Hardware Assumptions

* This repository does not currently contain a real automated test suite under `tests/`.
* Do not claim hardware behavior was verified unless you actually ran it on hardware.
* If a change affects GPIO, LCD, USB HID, interrupts, or board-specific behavior, call out that hardware validation is still required unless it was performed.
* Prefer host-independent validation where possible, but do not fabricate coverage that the repository does not have.

---

## Documentation Policy

- **Language**: All documentation (README.md, docs/, inline doc-comments) MUST be written in **English**.
- **README.md (top level)** is onboarding-first: overview, install, and one quick-start. Keep it short and link to details in `docs/`.
- **docs/** holds detailed documentation and is organized as:
  - **User guides** (practical usage / workflows)
  - **Specification references** (contracts: schema, flags, processing rules)
  - If content mixes both, split it into the appropriate documents.
- **Source of truth**
  - For post-implementation updates, treat **code + passing tests** as SoT and use `docs-maintenance-implementation-sync`.
  - For design-first work where the **spec is SoT**, use the spec-driven skills (`spec-driven-doc-authoring` / `spec-to-code-implementation`).
- **PR hygiene**: Update docs with behavior changes. If no doc updates are needed, explicitly note **"No documentation changes"** in the PR description.
---

## Dependency Management Policy

* Add dependencies with `cargo add <crate>`; do not edit Cargo.toml by hand for adds.
* Use SemVer pins; avoid wildcards unless necessary.
* Update dependencies per-PR with `cargo update -p <crate>`, explaining the target and reason.
* Run `cargo audit` for PRs to ensure no known vulnerabilities.
* Limit **dev-dependencies** to tests/tooling; remove when unused. Keep **build-dependencies** minimal and justify large additions.

---

## Release Process

* Follow **SemVer** for versioning.
* Tag new releases with `git tag vX.Y.Z` and verify `vorbere run release` outputs.
* Update CHANGELOG.md and reflect the changes in the release notes (include generators in the PR if they were used).

### CHANGELOG.md Policy

* **Sections**: Follow `[Keep a Changelog]` categories - `Added / Changed / Fixed / Deprecated / Removed / Security`.
* **Language**: English.
* **Writing Principles**:
  * Describe "what changes for the user" in one sentence; include implementation details only when needed.
  * Emphasize **breaking changes** in bold and provide migration steps.
  * Include PR/Issue numbers when possible (e.g., `(#123)`).
* **Workflow**:
  1. Add entries to the `Unreleased` section in feature PRs.
  2. Update the version number and date in release PRs.
  3. After tagging, copy the relevant section into the release notes.
* **Links (recommended)**:
  * Add comparison links at the end of the file.
* **Supporting Tools** (optional):
  * Use tools like `git-cliff` or `conventional-changelog` to draft entries, then edit manually.

---

## PR Template

PR descriptions MUST be written in **English** and MUST include:
- Motivation
- Design
- Tests (only what was actually run)
- Risks

For structured authoring (template, checklist), use the skill: `pr-description-authoring`.

---

## Checklist

* [ ] `vorbere run check`
* [ ] `vorbere run test`
* [ ] `vorbere run build`
