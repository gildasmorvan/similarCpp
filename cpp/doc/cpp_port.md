## SIMILAR C++ Port Overview

### Scope

The C++ port provides a complete translation of the **SIMILAR** (SImulation Multi-Agent of Interacting Levels ARchitecture) framework from Java, with:

- A **microkernel** containing core simulation abstractions (agents, environments, levels, influences, time model, dynamic state).
- An **extended kernel** providing high-level, concrete agent and model classes.
- Additional C++-based engines used by higher-level projects such as **JamFree** and **Similar2Logo**.

Conceptual and Java-oriented documentation remains in `src/doc`. This document focuses on the C++ translation itself.

### Project Structure (C++)

At a high level:

```text
cpp/
├── microkernel/            # Core simulation framework (interfaces + minimal implementations)
│   ├── include/            # Header-only interfaces and simple inline implementations
│   └── src/                # Concrete implementations and utilities
│
├── extendedkernel/         # Higher-level concrete framework
│   ├── include/            # Extended agent and model classes
│   ├── src/                # Implementations
│   └── examples/           # C++ examples (e.g. simple_extendedkernel example)
│
├── jamfree/                # Traffic simulation engine built on SIMILAR concepts
└── similar2logo/           # C++ multithreaded engine for Similar2Logo
```

For a detailed description of files and classes, see `cpp/README.md`.

### Design Principles

- **Faithful translation** of the original Java microkernel and extended kernel.
- **Header/implementation separation**: interfaces and small inline utilities are header-only; non-trivial logic lives in `.cpp` files.
- **Modern C++17**: smart pointers, RAII, exceptions, standard containers.
- **Same concepts as Java version**: agents, levels, influences, time model, dynamic state.

Key patterns used:

- **Strategy** for pluggable behavior models (perception, decision, revision).
- **Template Method** in base agent classes.
- **Interface segregation** via focused pure-virtual interfaces.

### Building the C++ Core

Build the main C++ libraries (microkernel + extendedkernel):

```bash
cd cpp
mkdir -p build
cd build
cmake ..
make
```

This typically builds:

- `libsimilar_microkernel.a` – core microkernel library.
- `libsimilar_extendedkernel.a` – extended kernel library.
- Example executables (such as an extendedkernel demo).

### Using the C++ Microkernel / Extended Kernel

A typical usage pattern is:

1. Define your **global state** and local dynamic state types.
2. Implement **perception**, **decision**, and **state revision** models.
3. Instantiate extended agents and inject your models.
4. Configure levels and the simulation engine.
5. Run the simulation loop.

For a concrete example, see:

- `extendedkernel/examples/simple_example.cpp`
- The examples and explanations in `cpp/README.md`.

### C++ Engines Built on SIMILAR

On top of the C++ core, several engines make use of the same architecture:

- **JamFree (cpp/jamfree)** – high-performance traffic simulation (microscopic and hybrid micro/macro), with optional GPU acceleration and a Python/web front-end.
- **Similar2Logo (cpp/similar2logo + top-level Python)** – C++ multithreaded engine for Logo-style simulations, exposed to Python via bindings.

These projects share the same conceptual foundations (agents, levels, influences, reactions) and can be understood using the original SIMILAR documentation in `src/doc`, while this file describes how those ideas have been realized in C++.


