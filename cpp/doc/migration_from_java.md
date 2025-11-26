## Migration Guide: Java Documentation → C++ / Python Stack

The original SIMILAR project was written in **Java**, and its main conceptual documentation (agents, levels, influences, reactions, probes, time models) lives in `src/doc`. The current project adds a full **C++ port** and **Python bindings** on top of these ideas.

This guide explains how to read the Java‑oriented docs when working with the C++/Python implementation.

### Where to Start

- **Concepts and theory (Java era)**:
  - `src/doc/README.html` and the HTML documentation under `src/doc/extendedKernel`, `src/doc/microKernel`, etc.
  - PDF and article references in `src/doc/files/`.
- **C++ implementation and engine reports**:
  - `CPP_ENGINE.md`, `CPP_ENGINE_PLAN.md`, `CPP_ENGINE_COMPLETE.md` under `docs/`.
  - `CPP_ENGINE_README.md` at the root.
  - C++‑specific docs under `cpp/doc/` (this directory).
- **Python bindings and web interfaces**:
  - `cpp/jamfree/PYTHON_BINDINGS_SUMMARY.md`, `cpp/jamfree/QUICK_START.md`.
  - `MULTITHREADED_ENGINE_PYTHON.md` and related Similar2Logo docs.

### Mapping Java Concepts to C++ / Python

In the Java documentation (`src/doc`), you will find:

- **Interfaces** like `IAgent`, `IEnvironment`, `ILevel`, `IInfluence`, `ISimulationEngine`.
- **Levels and influences**: levels represent places where agents evolve; influences describe intended actions; reactions apply them.
- **Time models and dynamic state**: time stamps, scheduling, and state containers.

In the C++ port, these map to:

- **C++ interfaces** (`class` with pure virtual methods) in `cpp/microkernel/include/` (see `cpp_port.md` for a high‑level view).
- **Extended kernel classes** in `cpp/extendedkernel/include/` implementing concrete behaviors.
- **C++ engines** (JamFree, Similar2Logo) that embed these abstractions and add domain‑specific logic.

In Python, these abstractions appear as:

- High‑level DSLs (`similar2logo.dsl`, JamFree Python API).
- Python classes that mirror C++ types (exposed via pybind11), e.g. `jamfree.Vehicle`, `jamfree.Road`, `similar2logo` Turtle classes.

### Reading Java Docs with C++ in Mind

When browsing `src/doc`:

- Treat any **Java interface** description as applying directly to the corresponding **C++ header**:
  - Java `interface` → C++ pure virtual class.
  - Java package → C++ namespace and directory.
- When the Java docs discuss:
  - **Multi‑level modeling, influences, reactions** → implemented in C++ by the microkernel and extended kernel, and then specialized in JamFree and Similar2Logo.
  - **Execution flow and scheduling** → mirrored by the C++ engines; see `docs/MULTITHREADING.md` and `CPP_ENGINE.md` for how this is parallelized.

The Java documentation remains the best **theoretical reference**, while the C++ and Python docs describe concrete realizations and performance details.

### Where to Look for Concrete Examples

- **Java examples and diagrams**:
  - See the HTML and images under `src/doc/extendedKernel` and `src/doc/microKernel`.
- **C++ examples**:
  - `cpp/extendedkernel/examples/` (basic SIMILAR usage in C++).
  - `cpp/jamfree/examples/` (traffic simulations in C++).
  - `cpp/similar2logo/src/` and `similar2logo` docs for Logo‑style C++ examples.
- **Python examples**:
  - `examples/python/` at the root (SIMILAR2Logo DSL examples).
  - `cpp/jamfree/python/examples/` (JamFree traffic simulations).

Use the Java docs to understand **what** the architecture does, then use the C++/Python docs and examples to see **how** it is implemented and how to run it in this project.


