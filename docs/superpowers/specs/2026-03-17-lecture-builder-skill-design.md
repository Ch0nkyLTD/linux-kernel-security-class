# Lecture Builder Skill — Design Spec

**Date:** 2026-03-17
**Status:** Draft
**Author:** Instructor + Claude

---

## Overview

A general-purpose lecture-building skill framework for Claude Code. Given a topic (or just a direction), it designs and produces a complete lecture session: Obsidian Advanced Slides, TikZ diagrams, in-class exercises, meme suggestions, and knowledge registry updates. Course-agnostic — configured per-course via a TOML file in the course repo.

### Goals

- Reduce lecture prep from hours to a guided conversation
- Maintain consistent quality and voice across lectures
- Track course progression, student knowledge, and concept dependencies
- Produce all artifacts (slides, diagrams, exercises, state updates) in one flow
- Support multiple courses in parallel via per-course config
- Keep the instructor in the loop at every decision point

### Non-Goals

- Autograding in-class exercises (lightweight only, solutions released after class)
- Replacing the assignment-creator skill (separate concern, can hand off)
- Generating final images for memes (suggests concepts + templates only)
- Auto-publishing or deploying lectures

---

## Skill Architecture

### Four skills, one orchestrator

| Skill | Invocation | Purpose |
|---|---|---|
| `lecture-builder` | `/lecture` | Orchestrator — full lecture session workflow |
| `tikz-diagram` | `/tikz` | Pattern-based + freeform TikZ diagram creation |
| `exercise-builder` | `/exercise` | In-class exercise with starter/solution/expected output |
| `meme-brainstorm` | `/meme` | SpongeBob / deep-cut meme suggestions for technical concepts |

All sub-skills are independently invokable. The orchestrator calls them internally during the full workflow.

### Integration with existing skills

- `humanizer` — post-pass on generated slide prose to strip AI patterns
- `assignment-creator` — if an exercise should become a graded assignment (user decides, not automatic)

### File layout

```
~/.claude/skills/lecture-builder/
├── SKILL.md                    # orchestrator (lecture-builder)
├── skills/
│   ├── tikz-diagram/
│   │   └── SKILL.md
│   ├── exercise-builder/
│   │   └── SKILL.md
│   └── meme-brainstorm/
│       └── SKILL.md
├── references/
│   ├── slide-format.md         # Obsidian Advanced Slides format rules
│   ├── tikz-patterns.md        # the 8 diagram patterns + style reference
│   ├── voice-guide.md          # writing voice rules (from 2024 baseline)
│   └── course-toml-schema.md   # config file spec
└── agents/
    ├── slide-writer.md         # instructions for slide-writing subagent
    ├── diagram-writer.md       # instructions for TikZ subagent
    └── exercise-writer.md      # instructions for exercise subagent
```

---

## `/lecture` Workflow

### Phase 1: Context Gathering

1. Find `course.toml` in the working directory (or ask which course)
2. Read `course-state.md` for last lecture, progression
3. Scan the schedule directory to confirm what's been delivered
4. Read knowledge registry to know what students have seen
5. Ask: "What topic?" (or accept as argument: `/lecture kernel-shellcoding`)
6. Ask: "What was the last lecture?" (with best guess pre-filled for confirmation)

### Phase 2: Topic Design

7. If given just a topic name — read relevant source code, modules, existing related lectures
8. Propose a **narrative arc** (e.g., derive from first principles → break it with an exploit)
9. Suggest slide count range, section breakdown, which modules/code to cover
10. Propose diagrams needed (descriptions, which TikZ patterns fit)
11. Propose in-class exercise (if appropriate for the topic)
12. Propose meme concepts (2-3 SpongeBob/deep-cut suggestions)
13. Flag knowledge registry gaps ("this lecture introduces `flush_icache_range` — not yet in registry")
14. **User reviews and adjusts the plan**

### Phase 3: Production (parallelizable via subagents)

15. Write slide markdown (dispatches slide-writer agent)
16. Write TikZ diagrams (dispatches diagram-writer agent per diagram)
17. Write exercise code if applicable (dispatches exercise-writer agent)
18. Build diagrams (`make` + verify compile)
19. Install SVGs (`make install`)
20. Run humanizer pass on slide prose

### Phase 4: Finalization

21. Update knowledge registry with new concepts/functions introduced
22. Update `course-state.md` with new lecture
23. Update Obsidian tags in lecture file
24. Update `MEMORY.md` with deliverable record
25. Present summary: files created, slide count, diagram count, what to review

---

## `course.toml` — Per-Course Configuration

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
knowledge = "ObsidianVault/Knowledge"

[paths.repo_dirs]
inclass = "inclass"
hw = "hw"
lectures = "lectures"

[diagrams]
style = "ObsidianVault/diagrams/style/kernel-slide-style.tex"
build_cmd = "make"
install_cmd = "make install"

[slides]
templates = ["Base Slide", "Split Vertical", "Title Slide"]
separator = "---"
max_slides = 50
min_slides = 20

[state]
course_state = "course-state.md"
knowledge_registry = "knowledge"
```

Lives in the course repo root. A new course clones from the template repo and fills in paths/metadata. Nothing course-content-specific in the schema — all content lives in the knowledge registry and state file.

---

## Knowledge Registry

### Structure

Per-concept Obsidian markdown files in the course repo, organized by category:

```
knowledge/
├── functions/
│   ├── prepare_kernel_cred.md
│   ├── commit_creds.md
│   ├── copy_from_user.md
│   └── ...
├── types/
│   ├── task_struct.md
│   ├── struct_cred.md
│   └── ...
├── concepts/
│   ├── callee-saved-registers.md
│   ├── page-table-walk.md
│   └── ...
├── syscalls/
│   ├── openat.md
│   ├── ioctl.md
│   └── ...
└── tools/
    ├── kprobe-hooking.md
    ├── ftrace-callback.md
    └── ...
```

### File format

Obsidian markdown is the primary representation. YAML frontmatter provides machine-queryable metadata. `[[wiki-links]]` used regardless of whether Obsidian is present — files are portable.

```markdown
---
name: prepare_kernel_cred
type: function
introduced_in: kernel-shellcoding
introduced_date: 2026-03-17
scope: mastery
prerequisites:
  - "[[struct_cred]]"
  - "[[task_struct]]"
  - "[[EL1 execution]]"
docs:
  - https://elixir.bootlin.com/linux/v6.6/source/kernel/cred.c
  - "[[kernel-shellcoding#The Two-Call API]]"
tags: [kernel, credentials, privesc]
---

# prepare_kernel_cred

Allocates a new `struct cred` with root privileges (all UIDs/GIDs = 0).

`prepare_kernel_cred(NULL)` copies from `init_cred`. Pass a `task_struct *`
to copy from a specific task instead.

Returns a `struct cred *` in `x0`. Pair with [[commit_creds]] to install.

## Used In
- [[kernel-shellcoding]] — the core privesc shellcode payload
- [[capstone-kernel]] — Challenge 7 (vuln_rwx exploit)

## See Also
- [[commit_creds]]
- [[struct_cred]]
```

### Scope levels

| Level | Meaning | Skill behavior |
|---|---|---|
| `introduced` | First shown in this lecture | Full explanation on slide |
| `used` | Referenced but not the focus | Brief reminder: "recall from [[X]]" |
| `mastery` | Students should know this cold | No explanation, use freely |

### Querying

The skill reads frontmatter from all files in `knowledge/` to build an in-memory index. For a new lecture:
- Concepts with `mastery` scope → use without explanation
- Concepts with `used` scope → brief callout with wiki-link
- Concepts not in registry → flag as new, needs introduction or explicit scope assignment

---

## `course-state.md` Format

```markdown
# Course State — CY-4973 Linux Kernel Security

## Current Position
- **Last lecture:** kernel-shellcoding (2026-03-17)
- **Next planned:** TBD
- **Lectures delivered:** 14
- **Total slides delivered:** ~480

## Lecture History

| Date | Topic File | Slides | Concepts Introduced | Exercises |
|------|-----------|--------|---------------------|-----------|
| 01-13 | armv8-a_crash_course | 35 | registers, EL0/EL1, adr/ldr | — |
| 01-16 | linux-basics, ko-basics | 45 | syscalls, insmod, chardev | — |
| ... | ... | ... | ... | ... |
| 03-17 | kernel-shellcoding | 21 | movz/movk, prepare_kernel_cred | — |

## Tag Index

| Tag | Lectures |
|-----|----------|
| kernel | ko-basics, kprobes, ftrace, syscall-table, ... |
| hooking | kprobes, ftrace, syscall-table |
| aarch64 | armv8-a_crash_course, kernel-shellcoding |
| privesc | promote, kernel-shellcoding, capstone-kernel |

## Knowledge Stats
- **Functions:** 47 registered
- **Types/Structs:** 23 registered
- **Concepts:** 31 registered
- **Syscalls:** 12 registered
- **Tools/Techniques:** 18 registered
```

Read at the start of every `/lecture` invocation. Updated at the end.

---

## Voice Guide

### Do

- Second person, direct address ("You're at EL1", "Your shellcode runs here")
- Code-first — show the code, then explain what it does
- Explicit agenda slides at the start of each lecture
- Polished, consistent formatting (tables, aligned code blocks)
- "Oops" / "What Can Go Wrong" slides that own mistakes and pitfalls
- Vary slide density — some slides 2 sentences, some 10
- Backward references with `[[wiki-links]]`: "recall from [[kprobes]] that..."
- Personality in code: creative variable names, opinionated comments
- Own uncertainty when genuine: "I think this is right but check me"
- Asymmetric depth — obsess on hard parts, gloss tangents with a link
- Connect to capstone/assignments: "this is Challenge 7"
- Meme slides with placeholder + description

### Don't

- "Let's dive into", "Now let's explore", "Let's take a look at"
- Rule-of-three rhetoric ("X, Y, and Z" for rhetorical effect)
- Trailing summaries restating what was just shown
- "As we've seen...", "In this section we covered..."
- "Powerful", "elegant", "robust", "comprehensive", "crucial"
- "It's worth noting that", "Importantly", "Interestingly"
- Uniform slide length — if every slide is 5 sentences, something's wrong
- Explaining things students already have `mastery` scope on in the knowledge registry

### Meme guidelines

- SpongeBob preferred, deep-cut references welcome
- Text description + suggested image search term + caption
- Placed as slides with image placeholder comment
- Maximum 2-3 per lecture, at natural break points
- Must land with a technical audience — no generic motivational memes

---

## `/tikz` — TikZ Diagram Sub-Skill

**Input:** Natural language description + optional pattern hint
**Output:** `.tex` file, Makefile update, built + installed SVG

### Process

1. Read `course.toml` for diagram paths and style file
2. Match description against 8 known patterns:
   - **Container** — userspace/kernel split with cross-boundary arrows
   - **Vertical cascade** — page table walk, module lifecycle, call chain
   - **Hook callout** — ftrace/kprobe interception point branching off normal flow
   - **Side-by-side comparison** — before/after, with/without
   - **Bitfield** — PTE format, instruction encoding, struct layout
   - **Timeline** — RCU grace period, execution sequence
   - **Flow chain** — A → B → C → D with annotations between nodes
   - **Parametric layout** — configurable distances, reusable structure
3. If no pattern fits, freeform author using shared style
4. Write `.tex` to `diagrams/<category>/`
5. Update Makefile (add to category TEX list, add VPATH if new category)
6. Run `make <category>` — verify compile
7. Run `make install` — copy SVG to imgs dir
8. Return embed syntax: `![[name.svg|width]]`

### Constraints

- Must use `\documentclass[tikz,border=10pt]{standalone}`
- Must `\input{kernel-slide-style}` (no additional `\usepackage`)
- Requires lualatex, Liberation Sans/Mono fonts
- Naming: `<category>_<description>.tex` (lowercase, underscores)
- Always build-verify before reporting success

---

## `/exercise` — Exercise Builder Sub-Skill

**Input:** Topic + learning objective + time target (default 10 min)
**Output:** Directory in `inclass/` with starter, solution, README, expected output

### Generated structure

```
inclass/<exercise_name>/
├── README.md              # instructions, learning goal, hints
├── <name>.c               # starter with TODOs + heavy comments
├── <name>_solution.c      # complete solution
├── expected_output.txt    # what correct execution looks like
├── Makefile               # build (cross-compile aware from course.toml)
└── inc/                   # any shared headers (if needed)
```

### Rules

- TODOs completable in stated time target
- Comments explain the "why" not just the "what"
- Starter compiles and runs (just doesn't do the right thing yet)
- Solution is clean code (no `// <solution>` blocks)
- Expected output shows exact terminal output for comparison
- Makefile supports cross-compilation if `course.toml` specifies a target architecture

---

## `/meme` — Meme Brainstorm Sub-Skill

**Input:** Technical concept or lecture topic
**Output:** 2-3 meme suggestions as slide-ready markdown

### Each suggestion includes

- Meme template name (e.g., "Patrick's Wallet", "Mocking SpongeBob")
- Technical mapping (what the meme panels represent)
- Caption text
- Slide markdown with image placeholder comment
- Suggested search term for finding the template

### Example output

```markdown
---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Callee-Saved Registers
:::

<!-- meme: "Patrick's Wallet" template
     Panel 1: "Is this your address?" (x20 holds commit_creds addr)
     Panel 2: "Yes"
     Panel 3: *blr x19 runs* (prepare_kernel_cred clobbers x0-x18)
     Panel 4: "Is this your address?" (x20 still has it)
     Panel 5: "Yes"
     Search: "patrick wallet meme template"
-->

<!-- TODO: Insert meme image here -->
<!-- element style="width: 60%; margin: 0 auto;" -->
```

---

## Course Template Repo

Git template for new courses:

```
course-template/
├── course.toml                    # fill in per-course
├── course-state.md                # empty, skill populates
├── CLAUDE.md                      # project instructions for Claude Code
├── .gitignore
├── inclass/
│   └── .gitkeep
├── hw/
│   └── .gitkeep
├── lectures/
│   └── .gitkeep
├── knowledge/
│   ├── functions/
│   │   └── .gitkeep
│   ├── types/
│   │   └── .gitkeep
│   ├── concepts/
│   │   └── .gitkeep
│   ├── syscalls/
│   │   └── .gitkeep
│   └── tools/
│       └── .gitkeep
└── scratch/
    └── .gitkeep
```

Knowledge directory lives in the course repo. Uses `[[wiki-links]]` syntax regardless of Obsidian — files are portable. If the course uses Obsidian, add the course repo as a vault folder for link resolution.

---

## Test Plan

### Bootstrap test: SPR-2026

1. Add `course.toml` to `linux-kernel-security-class/`
2. Scan all 14 delivered SPR-2026 lectures to populate `course-state.md`
3. Scan lectures + source modules to populate `knowledge/` registry
4. Verify knowledge registry covers all functions/types/concepts used in delivered lectures
5. Use `/lecture` to build the next lecture topic — validates the full workflow
6. Use `/tikz` standalone on a diagram description — validates independent invocation
7. Use `/exercise` standalone — validates exercise generation
8. Use `/meme` standalone — validates meme suggestions

### Verification criteria

- [ ] `course.toml` parses without error
- [ ] `course-state.md` accurately reflects 14 lectures delivered
- [ ] Knowledge registry has entries for all kernel functions, types, and concepts taught
- [ ] Registry entries have correct `introduced_in` and `scope` values
- [ ] `/lecture` produces slides that render in Obsidian Advanced Slides
- [ ] All `:::` blocks in generated slides are balanced
- [ ] TikZ diagrams compile with `make`
- [ ] Generated exercises compile and run
- [ ] Voice guide rules are followed (no AI-slop phrases)
- [ ] `course-state.md` is correctly updated after a new lecture
