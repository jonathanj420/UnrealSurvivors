# DarkEdenSurvivors — Project Context & Development Principles

> **Purpose:** Give Claude Code the project's background, the developer's original learning goals, and guidance for continuing development. This is high-level context, not a substitute for inspecting the repository. The actual source code is the source of truth for implementation status and current architecture.

## 1. Project Overview

- **Project:** `DarkEdenSurvivors`
- **Engine:** Unreal Engine 5.7
- **Language:** C++
- **Genre / core format:** A *Vampire Survivors*-like action-survival prototype themed around *DarkEden*.
- **Role in the developer's portfolio:** A gameplay-programming project intended to demonstrate both implementation ability and understanding of gameplay architecture.

## 2. The Developer's Original Goal

The original goal was not merely to make a working game or to copy AI-generated code.

The developer deliberately asked Gemini to propose implementations inspired by professional game-development practices, including AAA-style architecture, because they wanted to learn how production code is structured and how systems and data flow through a real project.

The earlier workflow was roughly:
1. Describe a gameplay feature or technical problem to Gemini in a browser.
2. Request a professional, production-oriented design and implementation.
3. Review the generated code, copy it into the project, and test it.
4. Build, inspect logs, and debug errors.
5. Study the resulting implementation to understand its design, responsibilities, and execution flow.

This workflow was slower than today's integrated coding-agent workflow, but it was also a deliberate learning process. The developer reports that, even after leaving the project untouched for more than six months, they can still explain its major flows and underlying principles.

**Important:** Preserve this learning objective. Do not reduce the project to a quick prototype assembled without understanding. The goal is to build the game while continuing to develop the ability to reason about professional C++ gameplay systems.

## 3. Technical Direction

- **Engine:** Unreal Engine 5.7.
- **Language:** C++.
- **No Gameplay Ability System (GAS).** The project was designed without GAS. Do not introduce GAS unless explicitly requested.
- **C++-first gameplay.** Do not assume core gameplay is Blueprint-based. Avoid adding Blueprint dependencies to core gameplay unless the developer explicitly asks for them or the existing project demonstrates a deliberate reason.
- The developer is interested in production-quality, professional game architecture, including patterns commonly found in larger game projects.
- Do not equate “AAA-style” with maximum abstraction or complexity. Choose an architecture because it solves a real problem, and explain meaningful trade-offs.
- Avoid both extremes: do not oversimplify a system that benefits from clear separation of responsibilities, and do not introduce elaborate frameworks merely to make code look enterprise-grade.

## 4. Architectural Themes

The following are known architectural interests and portfolio themes. Inspect the repository to establish which are implemented, partially implemented, or only intended.

### 4.1 Data-driven design
Separate variable gameplay data and tuning from the code that consumes it where appropriate. Make it possible to extend or tune content without adding unnecessary hard-coded branches.

### 4.2 Component-based behavior
Give components focused responsibilities where that improves reuse, ownership, or clarity. Keep actor and component responsibilities understandable. Do not split everything into components automatically.

### 4.3 Object pooling
Reuse frequently created and destroyed gameplay objects where it is useful, especially in high-volume gameplay. Pay attention to object lifecycle, activation/deactivation, reset behavior, stale state, and safe reuse.

### 4.4 Clear data flow and responsibility boundaries
Make it easy to trace where data originates, how it is transformed, which system owns it, and where it is consumed. Prefer explicit, understandable communication between gameplay systems over hidden dependencies on unrelated actors' state.

These themes are not permission to redesign the project blindly. Read the existing implementation before deciding whether a change is needed.

## 5. A Relevant Lesson Applied to Another Project: Damage-to-Kill Flow

The developer later applied a design idea learned while working on `DarkEdenSurvivors` to another Unreal Engine project, `Kuaqe`.

In Kuaqe, an earlier damage-to-kill implementation stored kill-related information on the victim/corpse and had the server later refer back to that actor to retrieve and validate the information.

The developer considered an alternative inspired by the DarkEdenSurvivors approach: collect the information needed for kill processing during damage handling, package it into an explicit data structure/report, and pass that information along to the subsequent death/kill-processing flow. The developer proposed this alternative to Claude, which agreed it was a reasonable design direction.

This is a useful example of the developer's goals:
- Learn an architectural idea in one project.
- Understand the problem it addresses.
- Recognize where it may apply in another project.
- Compare alternatives rather than accepting the current implementation automatically.

**Do not assume that the two projects use identical code or that one approach is universally superior.** The best design depends on lifecycle, authority, validation, event ordering, and ownership requirements. When discussing similar patterns, explain the trade-offs and inspect the actual code.

## 6. How Claude Code Should Work on This Project

### 6.1 Inspect before changing
Before editing a system:
- Read the relevant classes and call sites.
- Trace the execution and data flow.
- Check ownership, lifecycle, and server/client responsibilities where applicable.
- Identify existing conventions and dependencies.
- Establish whether the issue is real and whether the proposed change fits the current architecture.

### 6.2 Preserve professional learning value
When making a meaningful architectural decision, briefly explain:
- What problem the design solves.
- Why the chosen responsibility boundary is appropriate.
- How data moves through the system.
- Why this approach is preferable to plausible alternatives in this context.
- Important downsides, complexity, and edge cases.

Do not write lengthy explanations for every trivial line. Focus on decisions that affect architecture, lifecycle, correctness, extensibility, networking, or debugging.

### 6.3 Collaborate rather than blindly execute
The developer may propose an alternative design or ask whether an existing pattern is appropriate. Evaluate the proposal on its technical merits:
- Agree when it is sound and explain why.
- Point out risks or missing requirements when necessary.
- Compare viable alternatives fairly.
- Do not agree merely because the developer suggested it.
- Do not replace the developer's design without explaining the reason.

### 6.4 Prefer production-quality design, not complexity for its own sake
Use sound Unreal Engine C++ practices and architecture appropriate to the system's actual requirements. Avoid unnecessary abstraction, speculative generalization, premature optimization, and AAA terminology without substance.

A simpler implementation is appropriate when it is genuinely better. A more structured implementation is appropriate when it improves ownership, extensibility, testability, lifecycle safety, or clarity.

### 6.5 Verify changes
- Build the project when the environment permits.
- Run relevant tests or targeted validation when available.
- Inspect compiler output and logs.
- Do not claim a change works unless it has been verified to the extent possible.
- Distinguish confirmed behavior from assumptions or untested reasoning.

### 6.6 Preserve developer understanding
The developer wants to improve as a gameplay programmer, not merely obtain code. For substantial changes, leave them with a clear understanding of the important execution flow and design choices. Avoid forcing them to read every generated line in equal detail; prioritize the parts that determine system behavior and correctness.

## 7. Portfolio Positioning

Potential portfolio themes, subject to verification in the source:
- Unreal Engine 5.7 and C++ gameplay programming.
- A *Vampire Survivors*-like action-survival prototype themed around *DarkEden*.
- Gameplay architecture without GAS and with a C++-first approach.
- Data-driven gameplay design.
- Component-based behavior and separation of responsibilities.
- Object pooling for frequently reused gameplay objects, where implemented.
- Understanding and explaining gameplay data flow, object lifecycle, and architectural trade-offs.

Be accurate. Do not claim that a system is finished, optimized, tested, or implemented in a particular way unless the repository or measurements confirm it.

## 8. Known Information and Unknowns

### Known from project context
- Unreal Engine 5.7.
- C++-based *Vampire Survivors*-like action-survival prototype themed around *DarkEden*.
- No GAS.
- C++-first core gameplay direction.
- The developer deliberately used AI to study professional/AAA-inspired code structures and system flows.
- Data-driven design, component-based behavior, object pooling, and explicit data flow are relevant architectural themes.
- The developer can still explain the project's major flows and principles after a long break.
- The project is part of the developer's gameplay-programming learning and portfolio journey.

### Must be verified from the repository
- Current build status and required environment/plugins.
- Exact gameplay loop and current feature list.
- Class names and implementation details for characters, enemies, attacks, projectiles, damage, death, kill processing, spawning, and pooling.
- Exact data assets, tables, schemas, and configuration formats.
- Which architectural themes are implemented, partially implemented, or merely planned.
- Known bugs, technical debt, performance measurements, and test coverage.

---

## Working Principle

**Build the game, learn the architecture, and verify the result.**

Treat the repository as the source of truth. Use this document to understand the developer's intent, then inspect the code before making implementation claims or changes.
