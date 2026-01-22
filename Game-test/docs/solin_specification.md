# Solin IDE Specification

---

## 1. Overview

Solin is a native IDE built for efficiency and clarity.  
It uses a minimal number of permanent threads, binary in-memory caching, and direct system execution for build and run commands.  
All operations are deterministic, asynchronous, and designed to never block the main thread.

---

## 2. Thread Model

| Thread | Purpose | Notes |
|---------|----------|-------|
| **Main** | UI and input | Handles rendering, layout, event dispatch, and high-level control flow. |
| **Command/Search** | Command panel processing | Executes all user commands, searches, and lookup operations using in-memory data. |
| **Compile** | Build management | Runs user-defined build and run commands through external processes. |
| **Save/Load** | Cache management | Loads project data at startup and writes updates to disk asynchronously when saving. |

Each thread runs independently and remains active for Solin’s lifetime.  
All use a shutdown flag for safe termination; no blocking joins occur during exit.

---

## 3. Cache System

Project state and metadata are fully loaded into memory at startup for instant access.  
Caches are written back to disk on save using the Save/Load thread.

| File | Purpose |
|------|----------|
| `.sol` | Persistent project data (settings, layout, breakpoints, targets). |
| `.solcache` | In-memory working snapshot containing the hierarchy, symbols, and search data. |

Deleting either cache is safe; Solin regenerates them as needed.  
All cache data is stored in a compact binary format for fast loading and minimal disk I/O.

---

## 4. Build and Run System

Solin does not include a built-in compiler.  
Instead, it executes **any** user-specified system command directly through standard system calls.

- Supports any valid executable or script (e.g., `cl`, `clang++`, `gcc`, `make`, `cmake --build`, `python build.py`, etc.).  
- Invalid commands print a non-blocking console message such as  
  `Couldn't run build command 2 because it was empty`.  
- Output from subprocesses is streamed directly to the Solin console in real time.  
- Supports up to **5 build** and **5 run** targets.  
  - Empty targets are ignored.  
  - If multiple exist, a small popup (1–5) appears for selection.  
- Shortcut keys:  
  - **F5** — Run (default target)
  - **Ctrl+B** — Build

The Compile thread handles all build and run execution and listens for shutdown or cancellation events.

---

## 5. UI Structure

The Solin interface is built around **dynamic visibility** and **maximum code space**.  
All elements fade smoothly and remain docked within the workspace — no floating windows.

### Layout Components

| Element | Description |
|----------|--------------|
| **Main Area** | Contains editors, console, error list, debugger, and call stack. Always visible. |
| **Hover Panels** | Panels for actions like project hierarchy and top toolbar. They fade in when the mouse hovers the corresponding edge and fade out when inactive. |
| **Top Bar** | Appears when hovering the top edge; contains quick-access buttons (build, run, debug, settings). Hidden otherwise. |
| **Right Sidebar** | Appears when hovering the right edge; displays the project hierarchy and file tree. Hidden otherwise. |
| **Command Panel** | Central overlay for text commands. Always top-level and focused while open. Opens with **Shift + :**, closes with **Esc** or after execution. |
| **Transitions** | All UI transitions occur over ~100–150 ms, optimized for smoothness without delay. |

This structure allows the majority of the screen to remain dedicated to code editing while keeping all controls accessible without clutter.

---

## 6. Command Panel

The Command/Search thread is fully responsible for the command panel’s logic and responsiveness.  
It processes all user commands and performs lookups directly on the in-memory cache.

### Behavior

- Opens with **Shift + :**  
- Always focused and top-level while active  
- Updates results instantly with every keystroke  
- Closes automatically after execution or **Esc**  
- All command logic runs on the Command/Search thread  
- File and variable searches use cached data only — no live disk access

### Functionality

- Build, run, and search operations are triggered and handled here.  
- File creation, renaming, deletion, and movement are also initiated here.  
- File operations are **strictly limited** to the project root and cannot affect the system outside it.  
- Settings and help queries are accessible through the same interface with live filtering.  
- Invalid or empty input results in a printed console message without UI interruption.

---

## 7. Runtime Behavior

- Recommended minimum: **2 cores / 4 threads**.  
- All project data is cached in memory at startup for instant availability.  
- UI remains interactive during compilation, saving, and searching.  
- All background tasks are asynchronous and atomic.  
- Console output, errors, and logs stream live during execution.  
- Shutdown occurs instantly via flag signaling; all threads exit cleanly.