# Lecture Builder Skill — Implementation Plan

> **For agentic workers:** REQUIRED: Use superpowers:subagent-driven-development (if subagents available) or superpowers:executing-plans to implement this plan. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a 4-skill lecture framework (`/lecture`, `/tikz`, `/exercise`, `/meme`) with knowledge registry and course state, bootstrapped against SPR-2026.

**Architecture:** A local Claude Code skill plugin at `~/.claude/skills/lecture-builder/` containing an orchestrator SKILL.md + 3 sub-skill SKILL.md files + reference docs + agent prompts. Course data (course.toml, course-state.md, knowledge/) lives in the course repo.

**Tech Stack:** Claude Code skills (SKILL.md markdown), TOML config, Obsidian markdown with YAML frontmatter, TikZ/lualatex, Make.

**Spec:** `docs/superpowers/specs/2026-03-17-lecture-builder-skill-design.md`

---

## File Map

### Course repo (`~/teaching/linux-kernel-security-class/`)

| File | Action | Purpose |
|------|--------|---------|
| `course.toml` | Create | Per-course config pointing to vault, lab, assignments |
| `course-state.md` | Create | Lecture history, tag index, knowledge stats |
| `knowledge/functions/*.md` | Create | ~30-50 kernel function registry entries |
| `knowledge/types/*.md` | Create | ~15-25 struct/type registry entries |
| `knowledge/concepts/*.md` | Create | ~20-30 concept registry entries |
| `knowledge/syscalls/*.md` | Create | ~10-15 syscall registry entries |
| `knowledge/tools/*.md` | Create | ~10-20 technique registry entries |

### Skill plugin (`~/.claude/skills/lecture-builder/`)

| File | Action | Purpose |
|------|--------|---------|
| `SKILL.md` | Create | Orchestrator — `/lecture` entry point |
| `skills/tikz-diagram/SKILL.md` | Create | `/tikz` sub-skill |
| `skills/exercise-builder/SKILL.md` | Create | `/exercise` sub-skill |
| `skills/meme-brainstorm/SKILL.md` | Create | `/meme` sub-skill |
| `references/slide-format.md` | Create | Obsidian Advanced Slides format rules |
| `references/tikz-patterns.md` | Create | 8 diagram patterns + style reference |
| `references/voice-guide.md` | Create | Writing voice rules from 2024 baseline |
| `references/course-toml-schema.md` | Create | TOML config field reference |
| `agents/slide-writer.md` | Create | Subagent instructions for slide generation |
| `agents/diagram-writer.md` | Create | Subagent instructions for TikZ generation |
| `agents/exercise-writer.md` | Create | Subagent instructions for exercise generation |

---

## Phase A: Bootstrap Course Data

### Task 1: Create `course.toml`

**Files:**
- Create: `~/teaching/linux-kernel-security-class/course.toml`

- [ ] **Step 1: Write course.toml**

Write the TOML config per the spec. All paths use `~` expansion. Key values:

```toml
[course]
name = "CY-4973 Linux Kernel Security"
code = "CY-4973/7790"
semester = "SPR-2026"
institution = "WPI"

[paths]
repo = "~/teaching/linux-kernel-security-class"
vault = "~/teaching/linux-malware-course-public"
lab = "~/teaching/aarch64-linux-qemu-lab"
assignments = "~/teaching/assignments-2026-spr"
tone_reference = "~/teaching/mcp_spr_2024/malware-course-public"

[paths.vault_dirs]
topics = "ObsidianVault/Lectures/Slides/Topics"
schedule = "ObsidianVault/Lectures/Slides/SPR-2026"
diagrams = "ObsidianVault/diagrams"
imgs = "ObsidianVault/Imgs/tikz"

[paths.repo_dirs]
inclass = "inclass"
hw = "hw"
lectures = "lectures"
knowledge = "knowledge"

[diagrams]
style = "ObsidianVault/diagrams/style/kernel-slide-style.tex"
build_cmd = "make"
install_cmd = "make install"

[slides]
templates = ["Base Slide", "Split Vertical", "Title Slide"]
separator = "---"
max_slides = 50
min_slides = 20
output_dir = "ObsidianVault/Lectures/Slides/Topics"

[state]
course_state = "course-state.md"
knowledge_registry = "knowledge"
```

- [ ] **Step 2: Verify TOML parses**

Run: `python3 -c "import tomllib; print(tomllib.load(open('course.toml','rb')))"`
Expected: dict output, no parse errors.

- [ ] **Step 3: Commit**

```bash
git add course.toml
git commit -m "Add course.toml for lecture-builder skill"
```

---

### Task 2: Create knowledge directory structure

**Files:**
- Create: `~/teaching/linux-kernel-security-class/knowledge/{functions,types,concepts,syscalls,tools}/.gitkeep`

- [ ] **Step 1: Create directories**

```bash
mkdir -p knowledge/{functions,types,concepts,syscalls,tools}
touch knowledge/{functions,types,concepts,syscalls,tools}/.gitkeep
```

- [ ] **Step 2: Commit**

```bash
git add knowledge/
git commit -m "Add knowledge registry directory structure"
```

---

### Task 3: Create `course-state.md`

**Files:**
- Create: `~/teaching/linux-kernel-security-class/course-state.md`

- [ ] **Step 1: Write course-state.md**

Populate from the SPR-2026 schedule. The lecture history table uses data from the schedule scan:

```markdown
# Course State — CY-4973 Linux Kernel Security

## Current Position
- **Last lecture:** reflective-payloads (2026-03-13)
- **Next planned:** TBD
- **Lectures delivered:** 13 (excluding empty 01-27)
- **Total slides delivered:** ~696

## Lecture History

| Date | Topic File | Slides | Concepts Introduced | Exercises |
|------|-----------|--------|---------------------|-----------|
| 01-13 | armv8-a_crash_course, aarch64_asm_snippets | 32 | AArch64 registers, EL0/EL1, adr/ldr, stp/ldp | — |
| 01-16 | linux-basics, basic_elf, ko-basics | 79 | syscalls, ELF format, insmod, chardev, module_init | — |
| 01-20 | compiler-linking, linux_file_io | 24 | dynamic linking, GOT/PLT, open/read/write | — |
| 01-30 | linux_filesystem_ops, elf-in-depth-and-process-creation | 73 | VFS, inodes, ELF sections, execve, fork | — |
| 02-03 | ld_preload_0 | 44 | LD_PRELOAD, symbol interposition, dlsym | — |
| 02-06 | linux-kernel-basics | 44 | task_struct, cdev, file_operations, ioctl | — |
| 02-10 | promote, kprobes | 107 | cred struct, prepare_creds, kprobe, kretprobe, pt_regs | — |
| 02-13 | ftrace | 60 | ftrace_ops, patchable function entry, nop-to-bl | — |
| 02-17 | kallsyms, syscall-table | 47 | kallsyms_lookup_name, sys_call_table, PTE write-protect | — |
| 02-20 | virtual-memory-deepdive | 53 | 4-level page tables, TTBR0/TTBR1, TLB, PTE format | — |
| 02-24 | capstone-kernel | 39 | capstone chain, vuln drivers, rootkit architecture | — |
| 02-27 | kernel-code-injection | 60 | mm_struct, VMA, PTE construction, kernel_clone | — |
| 03-13 | reflective-payloads | 34 | memfd_create, /dev/shm, dlopen, reflective SO | — |
```

Notes:
- Add kernel-shellcoding (2026-03-17) if it has been delivered by the time this runs.
- Additional topic files exist but are not yet on the schedule: kernel-contexts.md, process-hiding.md, kthreads.md, shellcode-loaders.md, capstone-environment.md, full-chain-rootkit.md. These should be noted as "unscheduled" in course-state.md for future scheduling.
- The "Concepts Introduced" column should be filled by cross-referencing the actual lecture content — use an Explore agent to scan each topic file for key function/type/concept introductions.

- [ ] **Step 2: Write Tag Index section**

Scan all topic files for their primary subject areas. Group into tags:

```markdown
## Tag Index

| Tag | Lectures |
|-----|----------|
| aarch64 | armv8-a_crash_course, aarch64_asm_snippets, kernel-shellcoding |
| elf | basic_elf, elf-in-depth-and-process-creation |
| syscalls | linux-basics, linux_file_io, linux_filesystem_ops |
| linking | compiler-linking, ld_preload_0 |
| kernel-basics | ko-basics, linux-kernel-basics |
| hooking | kprobes, ftrace, syscall-table, kallsyms |
| credentials | promote, kernel-shellcoding |
| virtual-memory | virtual-memory-deepdive, syscall-table |
| injection | kernel-code-injection, reflective-payloads |
| capstone | capstone-kernel |

## Knowledge Stats
- **Functions:** 0 registered (pending bootstrap)
- **Types/Structs:** 0 registered (pending bootstrap)
- **Concepts:** 0 registered (pending bootstrap)
- **Syscalls:** 0 registered (pending bootstrap)
- **Tools/Techniques:** 0 registered (pending bootstrap)
```

Stats start at 0 — updated in Task 4 after knowledge registry bootstrap.

- [ ] **Step 3: Commit**

```bash
git add course-state.md
git commit -m "Add course-state.md with SPR-2026 lecture history"
```

---

### Task 4: Bootstrap knowledge registry

**Files:**
- Create: ~80-120 `.md` files across `knowledge/{functions,types,concepts,syscalls,tools}/`
- Modify: `course-state.md` (update Knowledge Stats)

This is the largest task. Dispatch parallel Explore agents to scan:
1. All 13 topic files from SPR-2026 for function calls, type references, concept introductions
2. All lab modules in `~/teaching/aarch64-linux-qemu-lab/modules/` for kernel API usage
3. The capstone exploit files for additional API references

- [ ] **Step 1: Scan lectures for kernel API surface**

Dispatch an Explore agent per lecture cluster:
- **Agent A**: armv8-a, linux-basics, ko-basics, linux-kernel-basics (foundational)
- **Agent B**: kprobes, ftrace, kallsyms, syscall-table (hooking)
- **Agent C**: promote, virtual-memory-deepdive, kernel-code-injection (advanced)
- **Agent D**: capstone-kernel, reflective-payloads, kernel-shellcoding (capstone)

Each agent reads the full lecture text and extracts:
- Function names (anything that looks like `function_name()` in code blocks)
- Type/struct names (`struct name`, `typedef`)
- Concept introductions (section headers that teach a new idea)
- Syscall references

- [ ] **Step 2: Merge and deduplicate results**

Combine all agent outputs. For each unique item:
- Determine `type`: function, type, concept, syscall, or tool
- Determine `introduced_in`: earliest lecture that *explains* it (not just uses it)
- Determine `scope`: 1 lecture → introduced, 2 → used, 3+ → mastery
- List `prerequisites` based on what the explanation references

- [ ] **Step 3: Generate registry files**

For each item, create a `.md` file in the appropriate `knowledge/` subdirectory. Use the frontmatter format from the spec. Body should include:
- One-paragraph description
- `## Used In` section listing lectures
- `## See Also` section with related entries

Example for a function entry:
```markdown
---
name: register_kprobe
type: function
introduced_in: kprobes
introduced_date: 2026-02-10
scope: mastery
scope_history:
  - { lecture: kprobes, level: introduced, date: 2026-02-10 }
  - { lecture: ftrace, level: used, date: 2026-02-13 }
  - { lecture: syscall-table, level: used, date: 2026-02-17 }
prerequisites:
  - "[[struct kprobe]]"
  - "[[pt_regs]]"
docs:
  - https://docs.kernel.org/trace/kprobes.html
  - "[[kprobes#Registering a Kprobe]]"
tags: [kernel, hooking, kprobe]
---

# register_kprobe

Registers a kprobe on a kernel function. Takes a `struct kprobe *` with `.symbol_name` set to the target function and `.pre_handler` / `.post_handler` callbacks.

Returns 0 on success, negative errno on failure.

## Used In
- [[kprobes]] — primary introduction with trace_openat example
- [[ftrace]] — comparison with ftrace approach
- [[syscall-table]] — kprobe bootstrap for symbol resolution

## See Also
- [[unregister_kprobe]]
- [[struct kprobe]]
- [[kretprobe-hooking]]
```

- [ ] **Step 4: Update course-state.md stats**

Count files in each `knowledge/` subdirectory and update the Knowledge Stats section.

- [ ] **Step 5: Present registry to instructor for review**

List all entries grouped by category with their scope assignments. Ask the instructor to:
- Flag any incorrect scope levels
- Add missing entries they know should be there
- Correct any wrong `introduced_in` attributions

- [ ] **Step 6: Apply instructor feedback and commit**

```bash
git add knowledge/ course-state.md
git commit -m "Bootstrap knowledge registry from SPR-2026 lectures (~N entries)"
```

---

## Phase B: Reference Documents

Tasks 5-8 are independent — can be parallelized.

### Task 5: Write `references/slide-format.md`

**Files:**
- Create: `~/.claude/skills/lecture-builder/references/slide-format.md`

- [ ] **Step 1: Create skill directory structure and init git repo**

```bash
mkdir -p ~/.claude/skills/lecture-builder/{skills/{tikz-diagram,exercise-builder,meme-brainstorm},references,agents}
cd ~/.claude/skills/lecture-builder && git init
```

- [ ] **Step 2: Write slide-format.md**

Content should cover (from spec Section "Slide Format Reference" + real lecture analysis):

1. File structure: blank line → `<!-- tags: ... -->` → `---` → slides
2. Three templates with exact syntax:
   - Title Slide: `<!-- slide template="[[Title Slide]]" -->` + bare `#### Heading`
   - Base Slide: template comment + `::: title` / `#### Heading` / `:::` + content
   - Split Vertical: template comment + `::: title` + `::: left` + `::: right`
3. Balance rules: every `:::` opener needs a closer, blank lines around `:::` blocks
4. Diagram embeds: `![[name.svg|width]]`, common widths (480, 700, 950)
5. Cross-references: `[[lecture-name]]`, `[[lecture-name#Section Heading]]`
6. Tag format: `<!-- tags: tag1, tag2, tag3 -->` as HTML comment before first `---`
7. Code blocks: fenced with language tag (c, bash, asm, python)
8. Tables: standard markdown tables
9. Meme placeholders: HTML comment with template description + `<!-- TODO: Insert meme image here -->`
10. Common pitfalls: missing blank lines between slides, unbalanced `:::`, forgetting template comment

Include a complete example slide file (5-6 slides) showing all three template types.

- [ ] **Step 3: Commit**

```bash
cd ~/.claude/skills/lecture-builder && git add references/slide-format.md && git commit -m "Add slide format reference"
```

---

### Task 6: Write `references/tikz-patterns.md`

**Files:**
- Create: `~/.claude/skills/lecture-builder/references/tikz-patterns.md`

- [ ] **Step 1: Write tikz-patterns.md**

Structure:
1. **Shared style reference**: complete color palette (11 colors with hex), 8 node styles, 5 arrow styles, 4 label styles — copied from `kernel-slide-style.tex`
2. **Boilerplate**: every diagram starts with `\documentclass[tikz,border=10pt]{standalone}` + `\input{kernel-slide-style}` + `\begin{document}` + `\begin{tikzpicture}`
3. **8 patterns** — for each:
   - Name and 1-sentence description
   - "Use when" guidance
   - Structural template (the TikZ skeleton with placeholder comments)
   - Representative example file path for reference reading

Pattern examples to reference:
- Container: `kernel-basics/kb_userspace_kernel.tex` (47 lines)
- Vertical cascade: `pagetable/pt_page_walk.tex` (126 lines)
- Hook callout: `ftrace/ftrace_trampoline.tex` (40 lines)
- Side-by-side: `prochide/prochide_filter_compare.tex` (143 lines)
- Bitfield: `pagetable/pt_pte_format.tex` (134 lines)
- Timeline: `promote/promote_rcu_grace_period.tex` (80 lines)
- Flow chain: `capstone/capstone_chain_overview.tex` (78 lines)
- Parametric: `promote/promote_rcu_grace_period.tex` (80 lines, same as timeline — uses `\def`)

4. **Naming convention**: `<category>_<description>.tex`, lowercase, underscores
5. **Makefile update procedure**: add to `<CATEGORY>_TEX` list, add VPATH entry if new category
6. **Build + install**: `make <category>` then `make install`

- [ ] **Step 2: Commit**

```bash
cd ~/.claude/skills/lecture-builder && git add references/tikz-patterns.md && git commit -m "Add TikZ patterns reference"
```

---

### Task 7: Write `references/voice-guide.md`

**Files:**
- Create: `~/.claude/skills/lecture-builder/references/voice-guide.md`

- [ ] **Step 1: Write voice-guide.md**

Content from spec "Voice Guide" section, expanded with concrete examples from the 2024 baseline analysis:

1. **Do list** (12 rules) — each with a concrete example quote from existing lectures
2. **Don't list** (8 anti-patterns) — each with a bad example and a rewritten good version
3. **Calibration procedure**: before writing, read one lecture from `tone_reference` path in course.toml
4. **Meme guidelines**: SpongeBob preferred, text description + search term + caption, max 2-3 per lecture
5. **Self-check**: after writing all slides, scan for these exact strings and rewrite:
   - "Let's dive", "Let's explore", "Let's take a look"
   - "powerful", "elegant", "robust", "comprehensive", "crucial"
   - "It's worth noting", "Importantly", "Interestingly"
   - "As we've seen", "In this section we covered"
   - Any sentence starting with "Now that we've"
   - Three-item lists used for rhetorical effect (not for listing actual things)

Include real quotes from the 2024 lectures as positive examples:
- "WTF is happening inside of that do-while loop?"
- "I don't know why it is listed as optional. I don't think a PE can run without this section (but I could be wrong?)"
- "Do not piss off anybody who has more time and money than you."

- [ ] **Step 2: Commit**

```bash
cd ~/.claude/skills/lecture-builder && git add references/voice-guide.md && git commit -m "Add voice guide reference"
```

---

### Task 8: Write `references/course-toml-schema.md`

**Files:**
- Create: `~/.claude/skills/lecture-builder/references/course-toml-schema.md`

- [ ] **Step 1: Write course-toml-schema.md**

Document every field in course.toml:
- `[course]`: name, code, semester, institution — all strings
- `[paths]`: repo, vault, lab, assignments, tone_reference — absolute or ~-relative paths
- `[paths.vault_dirs]`: topics, schedule, diagrams, imgs — relative to `[paths].vault`
- `[paths.repo_dirs]`: inclass, hw, lectures, knowledge — relative to `[paths].repo`
- `[diagrams]`: style (relative to vault), build_cmd, install_cmd
- `[slides]`: templates (array of strings), separator, max_slides, min_slides, output_dir (relative to vault)
- `[state]`: course_state, knowledge_registry — both relative to `[paths].repo`

Include the discovery algorithm: the skill searches for `course.toml` in the current directory, then parent directories, up to `$HOME`.

- [ ] **Step 2: Commit**

```bash
cd ~/.claude/skills/lecture-builder && git add references/course-toml-schema.md && git commit -m "Add course.toml schema reference"
```

---

## Phase C: Agent Prompts

Tasks 9-11 depend on reference docs (Phase B) but are independent of each other.

### Task 9: Write `agents/slide-writer.md`

**Files:**
- Create: `~/.claude/skills/lecture-builder/agents/slide-writer.md`

- [ ] **Step 1: Write slide-writer.md**

This is the prompt given to the Agent tool when dispatching a slide-writing subagent. It must include:

1. **Role**: "You are a slide writer for a university-level security course."
2. **Inputs provided to you**: approved lecture plan (section breakdown, slide counts, code references, narrative arc), course.toml path, knowledge registry path
3. **Reference docs to read**: point to `references/slide-format.md` and `references/voice-guide.md` — the agent must read these before writing
4. **Calibration step**: read one lecture from `tone_reference` path before writing
5. **Output format**: a single `.md` file in Obsidian Advanced Slides format
6. **Process**:
   - Read the voice guide and slide format reference
   - Read the tone reference lecture for calibration
   - Read the knowledge registry to determine which concepts need explanation vs. recall vs. assumed
   - Write the lecture file following the approved plan
   - Self-check: scan output for voice guide anti-patterns, rewrite flagged phrases
   - Verify: count `:::` openers and closers (must be equal), count `---` separators (should match expected slide count ±2)
7. **Constraints**: no YAML frontmatter in the lecture file, first line is blank, tags go in HTML comment

- [ ] **Step 2: Commit**

```bash
cd ~/.claude/skills/lecture-builder && git add agents/slide-writer.md && git commit -m "Add slide-writer agent prompt"
```

---

### Task 10: Write `agents/diagram-writer.md`

**Files:**
- Create: `~/.claude/skills/lecture-builder/agents/diagram-writer.md`

- [ ] **Step 1: Write diagram-writer.md**

Prompt for the TikZ diagram subagent:

1. **Role**: "You are a TikZ diagram author for dark-theme lecture slides."
2. **Inputs**: diagram description, recommended pattern, category name, style file path, diagrams directory path
3. **Reference**: point to `references/tikz-patterns.md` — agent must read this and the shared style file
4. **Process**:
   - Read the pattern reference and identify which pattern to use
   - If a pattern matches, follow its structural template
   - If no pattern matches, compose freeform using the shared node/arrow styles
   - Write the `.tex` file to `diagrams/<category>/<category>_<description>.tex`
   - Update the Makefile: add to `<CATEGORY>_TEX` list, add to VPATH if new category
5. **Constraints**: standalone class, `\input{kernel-slide-style}`, no additional `\usepackage`, naming convention `<category>_<description>.tex`
6. **Build verification**: run `make <category>` and check exit code. On failure, read the `.log` file, fix the `.tex`, retry once.

- [ ] **Step 2: Commit**

```bash
cd ~/.claude/skills/lecture-builder && git add agents/diagram-writer.md && git commit -m "Add diagram-writer agent prompt"
```

---

### Task 11: Write `agents/exercise-writer.md`

**Files:**
- Create: `~/.claude/skills/lecture-builder/agents/exercise-writer.md`

- [ ] **Step 1: Write exercise-writer.md**

Prompt for the exercise subagent:

1. **Role**: "You are an exercise designer for a hands-on security course."
2. **Inputs**: topic, learning objective, time target (default 10 min), course.toml path (for cross-compile settings)
3. **Output structure**:
   - `inclass/<name>/README.md` — instructions, goal, hints
   - `inclass/<name>/<name>.c` — starter with TODOs and heavy comments
   - `inclass/<name>/<name>_solution.c` — clean solution
   - `inclass/<name>/expected_output.txt` — exact terminal output
   - `inclass/<name>/Makefile` — build rules
4. **Rules**:
   - Starter must compile and run (outputs wrong/placeholder result)
   - TODOs completable in time target
   - Comments explain why, not just what
   - Solution is clean (no solution block markers)
   - Makefile includes cross-compile support if course targets aarch64
5. **Verification**: build the starter and solution, run both, confirm solution matches expected output

- [ ] **Step 2: Commit**

```bash
cd ~/.claude/skills/lecture-builder && git add agents/exercise-writer.md && git commit -m "Add exercise-writer agent prompt"
```

---

## Phase D: Sub-Skills

Tasks 12-14 are independent of each other.

### Task 12: Write `/tikz` sub-skill

**Files:**
- Create: `~/.claude/skills/lecture-builder/skills/tikz-diagram/SKILL.md`

- [ ] **Step 1: Write SKILL.md**

```yaml
---
name: tikz-diagram
version: 1.0.0
description: |
  Create TikZ diagrams for lecture slides. Use when the user asks to create
  a diagram, draw a figure, visualize a concept, or when /tikz is invoked.
  Supports 8 common patterns (container, cascade, hook, side-by-side,
  bitfield, timeline, flow chain, parametric) plus freeform authoring.
  Reads course.toml for paths. Builds and verifies with lualatex.
  Also use when the /lecture orchestrator needs diagrams generated.
allowed-tools:
  - Read
  - Write
  - Edit
  - Bash
  - Glob
  - Grep
  - Agent
---
```

Body instructions:
1. Find and read `course.toml` for paths
2. Read `references/tikz-patterns.md` for pattern reference
3. Check `Imgs/tikz/` for reusable existing diagrams matching the request
4. If reuse found, return `![[name.svg|width]]` and done
5. Determine best pattern from description
6. Dispatch diagram-writer agent (read `agents/diagram-writer.md` for agent instructions)
7. After agent completes: verify `.tex` exists, run `make <category>`, run `make install`
8. On compile failure: read log, fix, retry once. If still fails, report to user.
9. Return embed syntax `![[name.svg|width]]`

- [ ] **Step 2: Commit**

```bash
cd ~/.claude/skills/lecture-builder && git add skills/tikz-diagram/SKILL.md && git commit -m "Add /tikz sub-skill"
```

---

### Task 13: Write `/exercise` sub-skill

**Files:**
- Create: `~/.claude/skills/lecture-builder/skills/exercise-builder/SKILL.md`

- [ ] **Step 1: Write SKILL.md**

```yaml
---
name: exercise-builder
version: 1.0.0
description: |
  Create in-class exercises for lecture sessions. Use when the user asks
  to create an exercise, lab activity, or in-class coding problem, or when
  /exercise is invoked. Generates starter code with TODOs, solution, README,
  expected output, and Makefile. Exercises are 10-minute activities with
  heavy comments. Also use when /lecture orchestrator needs exercises.
allowed-tools:
  - Read
  - Write
  - Edit
  - Bash
  - Glob
  - Grep
  - Agent
---
```

Body instructions:
1. Find and read `course.toml` for paths (inclass dir, cross-compile settings)
2. Accept topic + learning objective + optional time target
3. Dispatch exercise-writer agent (read `agents/exercise-writer.md`)
4. After agent completes: verify all expected files exist
5. Build starter and solution with `make`
6. Run solution, capture output, verify it matches expected_output.txt
7. Report: files created, build status, exercise summary

- [ ] **Step 2: Commit**

```bash
cd ~/.claude/skills/lecture-builder && git add skills/exercise-builder/SKILL.md && git commit -m "Add /exercise sub-skill"
```

---

### Task 14: Write `/meme` sub-skill

**Files:**
- Create: `~/.claude/skills/lecture-builder/skills/meme-brainstorm/SKILL.md`

- [ ] **Step 1: Write SKILL.md**

```yaml
---
name: meme-brainstorm
version: 1.0.0
description: |
  Brainstorm meme ideas for lecture slides. Use when the user asks for meme
  suggestions, humor ideas, or SpongeBob references for technical topics,
  or when /meme is invoked. Generates 2-3 meme suggestions as slide-ready
  markdown with template name, panel descriptions, caption, and search terms.
  Prefers SpongeBob and deep-cut references. Also use when /lecture needs memes.
allowed-tools:
  - Read
  - Write
---
```

Body instructions (inline, no agent dispatch — this skill is simple enough):
1. Accept a technical concept or lecture topic
2. Read `references/voice-guide.md` for meme guidelines
3. Generate 2-3 meme suggestions, each with:
   - Meme template name
   - Technical mapping (what each panel represents)
   - Caption text
   - Slide markdown (Base Slide template with HTML comment describing the meme)
   - Image search term
4. Output the suggestions as slide-ready markdown blocks the user can paste into their lecture
5. Prefer SpongeBob templates: Patrick's Wallet, Mocking SpongeBob, Ight Imma Head Out, Is This a Pigeon (Squidward variant), Two SpongeBobs (diapers), Imagination Rainbow, Surprised Patrick

- [ ] **Step 2: Commit**

```bash
cd ~/.claude/skills/lecture-builder && git add skills/meme-brainstorm/SKILL.md && git commit -m "Add /meme sub-skill"
```

---

## Phase E: Orchestrator Skill

### Task 15: Write `/lecture` orchestrator

**Files:**
- Create: `~/.claude/skills/lecture-builder/SKILL.md`

- [ ] **Step 1: Write SKILL.md**

```yaml
---
name: lecture-builder
version: 1.0.0
description: |
  Build complete lecture sessions for university courses. Use when the user
  wants to create a new lecture, plan a class session, or when /lecture is
  invoked. Reads course.toml for configuration, checks course-state.md for
  progression, queries the knowledge registry for what students know.
  Produces: Obsidian Advanced Slides, TikZ diagrams, in-class exercises,
  meme suggestions, and updates course state and knowledge registry.
  Works with any course configured via course.toml.
allowed-tools:
  - Read
  - Write
  - Edit
  - Bash
  - Glob
  - Grep
  - Agent
  - Skill
---
```

Body instructions implementing the 4-phase workflow from the spec:

**Phase 1: Context Gathering**
- Search for `course.toml` in cwd and parent dirs
- Read it, expand `~` paths
- Read `course-state.md` — extract last lecture, lecture count
- Scan schedule dir to cross-check
- Read knowledge registry index (glob `knowledge/**/*.md`, parse frontmatter)
- If topic provided as argument, proceed. Otherwise ask.
- Confirm last lecture with user: "Last lecture was X on DATE — correct?"

**Phase 2: Topic Design**
- Read relevant source code/modules (grep lab dir, existing lectures)
- Propose narrative arc, slide count, sections, diagrams, exercise, memes
- Flag knowledge gaps
- Wait for user approval before proceeding

**Phase 3: Production** (order matters — spec requires slides first)
- Dispatch slide-writer agent with approved plan + references
- After slides complete: invoke `/tikz` for each diagram (check reuse first), `/exercise` if planned, `/meme` for meme suggestions — these three can run in parallel
- Build and install diagrams
- Voice-check slide output

**Phase 4: Finalization**
- Create new knowledge registry entries for newly introduced concepts
- Update course-state.md (lecture history row, tag index, stats)
- Update MEMORY.md with deliverable record
- Add inline tags to lecture file
- Present summary

**Error handling**: partial artifacts kept, state NOT updated on failure, user told what succeeded/failed with actionable next steps.

- [ ] **Step 2: Commit**

```bash
cd ~/.claude/skills/lecture-builder && git add SKILL.md && git commit -m "Add /lecture orchestrator skill"
```

---

## Phase F: Commit All and Validate

### Task 16: Final commit and skill discovery check

- [ ] **Step 1: Verify all files committed**

```bash
cd ~/.claude/skills/lecture-builder && git status
# Should show clean working tree — all files committed incrementally in Tasks 5-15
# If any uncommitted files remain:
# git add -A && git commit -m "Add remaining lecture-builder skill files"
```

- [ ] **Step 2: Verify skill discovery**

Start a new Claude Code conversation in `~/teaching/linux-kernel-security-class/` and check that the skills appear in the available skills list. Verify:
- `/lecture` is listed
- `/tikz` is listed
- `/exercise` is listed
- `/meme` is listed

- [ ] **Step 3: Smoke test — `/meme` standalone**

Run `/meme "page table walks"` and verify it produces 2-3 meme suggestions in the expected slide markdown format.

- [ ] **Step 4: Smoke test — `/tikz` standalone**

Run `/tikz "vertical cascade showing ioctl → module_alloc → copy_from_user → execute → free"` and verify it:
- Reads course.toml
- Writes a `.tex` file
- Updates Makefile
- Builds successfully
- Installs SVG
- Returns embed syntax

- [ ] **Step 5: Smoke test — `/exercise` standalone**

Run `/exercise "write a chardev that doubles input" --time 10min` and verify it creates the expected directory structure with compilable starter and solution.

- [ ] **Step 6: Integration test — `/lecture`**

Run `/lecture "kernel thread basics for the capstone beacon"` and verify the full 4-phase workflow:
- Reads course.toml and course-state
- Proposes topic design
- Produces slides + diagrams after approval
- Updates course-state and knowledge registry

---

## Dependency Graph

```
Task 1 (course.toml) ─────┐
Task 2 (knowledge dirs) ──┼── Task 4 (bootstrap registry)
Task 3 (course-state)     ┤
                          │
Task 5 (slide-format.md)  │
Task 6 (tikz-patterns.md) ├── all independent, parallel
Task 7 (voice-guide.md)   │
Task 8 (toml-schema.md)   │
                          │
Task 5-8 ─────────────────┤
                          ├── Task 9  (slide-writer agent)
                          ├── Task 10 (diagram-writer agent)
                          ├── Task 11 (exercise-writer agent)
                          │
Task 9-11 + Task 6 ───────┼── Task 12 (/tikz skill)
Task 9-11 + Task 5,7 ─────┼── Task 13 (/exercise skill)
Task 7 ────────────────────┼── Task 14 (/meme skill)
                          │
Task 12-14 ────────────────┴── Task 15 (/lecture orchestrator)

Task 1-15 ─────────────────── Task 16 (validate)
```

**Parallelization opportunities:**
- Tasks 1-3 can run in parallel (no dependencies between them)
- Tasks 5-8 can run in parallel
- Tasks 9-11 can run in parallel (after 5-8)
- Tasks 12-14 can run in parallel (after 9-11)
- Task 4 depends on Tasks 1, 2, and 3 (needs course.toml paths, knowledge dirs, and course-state)
