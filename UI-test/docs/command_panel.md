# Solin Command Panel

The **Solin Command Panel** is a fast, always-focused interface for all major IDE actions.  
It opens with **Shift + :** and accepts short, mnemonic commands — no need to type `:` inside the panel.

---

## Overview

- Commands are **1–2 letters long**  
- Panel is **always focused and top-level** when open  
- Smooth fade-in/out transitions  
- **All results update instantly** as you type — this applies globally  
- Live filtering and autocompletion are built in  
- Invalid or empty actions print clear console messages (no popups)  
- Panel closes automatically after execution or **Esc**

---

## Core Commands

| Command | Action | Notes |
|----------|---------|-------|
| `b` | Build | Runs the active build command; if multiple build targets exist (1–5), a numbered popup appears and filters live as you type. |
| `r` | Run | Runs the active run command; if multiple run targets exist (1–5), a numbered popup appears and filters live as you type. |

---

## Search Commands

| Command | Action | Notes |
|----------|---------|-------|
| `sf` | Search Files | Searches all project files recursively using the in-memory file list. |
| `sv` | Search Vars | Searches cached symbols (classes, functions, variables, namespaces). |

---

## File Operations

| Command | Action |
|----------|--------|
| `cf` | Create File |
| `df` | Delete File |
| `rf` | Rename File |
| `mf` | Move File |
| `of` | Open File |

- **All operations are strictly relative to the project root.**  
- Never modifies or touches anything outside the project directory.  

---

## System / Meta Commands

| Command | Action | Notes |
|----------|---------|-------|
| `s` | Settings | Opens the settings view with live filtering. |
| `h` | Help | Lists all available commands. |
| `h <command>` | Help (specific) | Shows info about a single command as you type. |
| `e` | Exit | Prompts to save unsaved files before quitting. |
| `qe` | Quick Exit | Instantly quits with no confirmation (even if files are unsaved). |