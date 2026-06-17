# AGENTS.md

This document is the README for AI coding agents. It complements the human-facing README.md so that agents can develop safely and efficiently.

---

## Documentation of Process vs Policy

This repository separates **policy** from **how-to guidance**:

- **AGENTS.md = Policy (MUST/MUST NOT)**  
  Contains the mandatory rules agents must follow (e.g., language requirements, required sections, validation expectations, boundaries).
  Keep it short and stable.

- **SKILLS = Procedure / Templates / Checklists**  
  Contains step-by-step workflows, templates, and checklists used to comply with policy.
  Prefer updating skills when improving writing structure or workflow details.

Rule of thumb:
- If it is a non-negotiable rule for reviews/CI: put it in **AGENTS.md**.
- If it is an example, template, or writing process: put it in a **skill**.

---

## Scope

* Treat this file as the primary agent policy for the repository root.
* Prefer explicit user instructions when they conflict with general guidance.
* Keep changes minimal and aligned with the current codebase; do not perform broad cleanup unless requested.
* Read relevant source, tests, docs, and task definitions before changing behavior or validation claims.
* Avoid introducing new top-level directories unless the user asks for that explicitly or the existing project layout clearly requires it.

---

## Build & Validate

Default validation commands:

* Check: `vorbere run check`
* Test: `vorbere run test`
* Build: `vorbere run build`
* Cleanup: `vorbere run clean`

Rules:

* Inspect `vorbere.yaml` before claiming what `check`, `test`, or `build` actually validates.
* Run `vorbere run check`, `vorbere run test`, and `vorbere run build` before completing substantive code changes when feasible.
* If a relevant task is missing, placeholder-only, skipped, or fails, say so explicitly in the final report.
* Use underlying tool commands only to investigate or isolate failures; keep `vorbere run <task>` as the reported validation entry point.
* When behavior changes, update or add validation that fits the existing stack.

---

## Project Structure

* Follow the existing repository layout and naming conventions.
* Keep application code, tests, documentation, and build definitions in their established locations.
* Keep module-local build definitions near the module when the repository already follows that pattern.
* Treat vendored, generated, upstream, or submodule content as external unless the task clearly requires editing it.
* Place temporary files and generated scratch data under ignored build/temp locations, not beside source files.

---

## Coding Standards

* Always run `vorbere run check` and ensure all included static checks pass with no warnings (CI requirement).
* Keep entry-point files focused on startup, dependency wiring, and high-level orchestration where practical.
* Prefer existing module boundaries and helper APIs over new abstractions.
* Extract magic numbers/URLs into constants with meaningful names.
* Avoid unrelated large refactors; keep changes minimal in scope.

---

## Testing & Verification

* Unit tests: `vorbere run test`
* For additional file or network operations, use temp directories or `httptest` to avoid external dependencies.
* When command behavior changes, keep usage examples in `README.md` and fixtures under `test` consistent.

### Static Analysis / Lint / Vulnerability Scanning

* Run `vorbere run check` as the default entry point for static analysis, linting, vulnerability scanning, and related verification.
* If needed, use underlying component commands only to investigate or isolate specific failures (for example, `vorbere run vulnerability`).

---

### Static Analysis / Lint / Vulnerability Scanning

* Run `vorbere run check` as the default entry point for static analysis, linting, vulnerability scanning, and related verification.
* If needed, use underlying component commands only to investigate or isolate specific failures (for example, `vorbere run vulnerability`).

---

## CI Requirements

CI expectations are defined by repository workflow files and `vorbere.yaml`.

Before opening or updating a PR, confirm the relevant local equivalents pass, normally:

* `vorbere run check`
* `vorbere run test`
* `vorbere run build`

If a command cannot be run locally, explain why and identify the remaining risk.

---

## Security & Data Handling

* Do not commit secrets or confidential information.
* Do not log personal or authentication data in logs or error messages.
* Use fictitious URLs and passwords in test data; avoid hitting real services.
* Obtain user approval before accessing external networks.

---

## Agent Notes

* When instructions conflict, prioritize explicit user prompts and clarify any uncertainties.
* Before and after your work, ensure `vorbere run check`, `vorbere run test`, and `vorbere run build` all succeed; report the cause and fix if any of them fail.

---

## Branch Workflow (GitHub Flow)

This project follows **GitHub Flow** based on `main`.

* **main branch**: Always releasable. Direct commits are forbidden; use pull requests.
* **Feature branches (`feature/<topic>`)**: Branch from `main` for new features or enhancements, then open a PR when done.
* **Hotfix branches (`hotfix/<issue>`)**: Branch from `main` for urgent fixes, merge promptly after CI passes.

### Rules

* Always branch from `main`.

---

## Commit Message Policy

Commit messages MUST follow **Conventional Commits** and MUST be written in **English**.

For structured authoring, use the skill: `conventional-commits-authoring`.

---

## Documentation Policy

- **Language**: All documentation (README.md, docs/, inline doc-comments) MUST be written in **English**.
- **README.md (top level)** is onboarding-first: overview, install, and one quick-start. Keep it short and link to details in `docs/`.
- **docs/** holds detailed documentation and is organized as:
  - **User guides** (practical usage / workflows)
  - **Specification references** (contracts: schema, flags, processing rules)
  - If content mixes both, split it into the appropriate documents.
- **Source of truth**
  - For post-implementation updates, treat **code + passing validation** as SoT and use `docs-maintenance-implementation-sync`.
  - For design-first work where the **spec is SoT**, use the spec-driven skills (`spec-driven-doc-authoring` / `spec-to-code-implementation`).
- **PR hygiene**: Update docs with behavior changes. If no doc updates are needed, explicitly note **"No documentation changes"** in the PR description.

---

## Release Process

* Follow **SemVer** for versioning.

---

## PR Template

PR descriptions MUST be written in **English** and MUST include:

- Motivation
- Design
- Tests (only what was actually run)
- Risks

For structured authoring, use the skill: `pr-description-authoring`.

---

## Checklist

* [ ] `vorbere run check`
* [ ] `vorbere run test`
* [ ] `vorbere run build`
