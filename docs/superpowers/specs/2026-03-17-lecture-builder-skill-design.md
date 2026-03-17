# Lecture Builder Skill — Design Spec

**Date:** 2026-03-17
**Status:** Draft (rev 2 — post spec review)
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

### Deferred to v2

- Course template git repo (extract once schema stabilizes against SPR-2026)
- Slide render validation beyond structural checks (linting for Obsidian Advanced Slides quirks)
- Automatic module-to-topic discovery for new courses (v1 relies on user input or existing lecture references)

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

- `humanizer` — invoked inline (not as a separate skill call) as a prompt directive in the slide-writer agent instructions: "After writing all slides, review the full output against the voice guide anti-patterns and rewrite any flagged phrases."
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

## Slide Format Reference

This section defines the Obsidian Advanced Slides format. The full reference will live in `references/slide-format.md`; key rules are here for spec completeness.

### File structure

Every lecture file starts with a blank line, then `---` as the first slide separator. No YAML frontmatter in the lecture file itself — the first `---` is a slide boundary, not frontmatter.

### Template usage

**Title Slide** — used for the lecture title. No `::: title` / `:::` blocks. Just a bare heading:
```markdown
---
<!-- slide template="[[Title Slide]]" -->

#### Lecture Title — Subtitle Here

---
```

**Base Slide** — standard content slide. Uses `::: title` / `:::` for the heading:
```markdown
---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Slide Heading
:::

Prose content here. Code blocks, tables, images.

---
```

**Split Vertical** — two-column layout. Uses `::: title`, `::: left`, `::: right`:
```markdown
---
<!-- slide template="[[Split Vertical]]" -->

::: title
#### Slide Heading
:::

::: left

Left column content.

:::

::: right

![[diagram.svg|480]]

:::

---
```

### Tag format

Lectures get YAML-style tags in an HTML comment at the top of the file (after the initial blank line, before the first `---`). This avoids conflict with the slide separator while remaining parseable:

```markdown

<!-- tags: kernel, privesc, aarch64, shellcode -->
---
<!-- slide template="[[Title Slide]]" -->
```

The course-state tag index and knowledge registry are the authoritative sources. These inline tags are a convenience for Obsidian search/filtering. The skill generates them from the tag index during finalization.

### Diagram embeds

```markdown
![[diagram_name.svg|width]]
```

Common widths: 480 (in split column), 700 (full-width medium), 950 (full-width large).

### Cross-references

Wiki-links to other lectures: `[[kprobes]]`, `[[kernel-shellcoding#The Two-Call API]]`. These resolve in Obsidian when the vault contains the Topics directory.

---

## `/lecture` Workflow

### Input format

`/lecture` accepts flexible input:

- `/lecture` — no argument, skill asks what topic
- `/lecture kernel-shellcoding` — topic slug (matches a filename pattern)
- `/lecture "how process hiding works on AArch64"` — free-text description, skill interprets
- `/lecture` with a detailed plan pasted — skill executes the plan directly (Phase 2 becomes confirmation, not generation)

### Phase 1: Context Gathering

1. Find `course.toml` in the working directory or parent directories (or ask which course)
2. Read `course-state.md` (path relative to course repo root, per `[state].course_state`) for last lecture, progression
3. Scan the schedule directory to confirm what's been delivered
4. Read knowledge registry to know what students have seen
5. Ask: "What topic?" (or accept as argument)
6. Ask: "What was the last lecture?" (with best guess pre-filled for confirmation)

### Phase 2: Topic Design

7. If given just a topic name — read relevant source code, modules, existing related lectures. Discovery strategy: search knowledge registry for related concepts, grep lab modules for the topic, check existing lectures that wiki-link to related concepts.
8. Propose a **narrative arc** (e.g., derive from first principles → break it with an exploit)
9. Suggest slide count range, section breakdown, which modules/code to cover
10. Propose diagrams needed — for each, specify: description, recommended TikZ pattern, whether an existing diagram can be reused (search `Imgs/tikz/` for candidates)
11. Propose in-class exercise (if appropriate for the topic)
12. Propose meme concepts (2-3 SpongeBob/deep-cut suggestions)
13. Flag knowledge registry gaps ("this lecture introduces `flush_icache_range` — not yet in registry")
14. **User reviews and adjusts the plan**

### Phase 3: Production

Steps 15-17 use the Claude Code `Agent` tool to dispatch subagents. Each agent receives its instructions from `agents/<type>.md` plus the specific task context. Steps are sequenced as follows:

15. Write slide markdown — dispatch slide-writer agent with the approved plan, voice guide, and slide format reference
16. Write TikZ diagrams — dispatch one diagram-writer agent per diagram (these run in parallel with each other, but after step 15 so slide content informs diagram details)
17. Write exercise code if applicable — dispatch exercise-writer agent (parallel with step 16)
18. Build diagrams — run `make <category>` in the diagrams directory for each new diagram. **On compile failure:** read the lualatex log, fix the `.tex` file, retry once. If second attempt fails, report the error to the user and continue with remaining diagrams.
19. Install SVGs — run `make install`
20. Voice check — the slide-writer agent instructions include anti-pattern detection from the voice guide. If the orchestrator spots residual AI-slop in the output, it rewrites flagged sections before proceeding.

### Phase 4: Finalization

21. Update knowledge registry — create new `.md` files in `knowledge/` for each newly introduced concept/function/type. Set `scope: introduced` and `introduced_in:` to this lecture.
22. Update `course-state.md` — add row to lecture history, update tag index, bump stats
23. Generate inline tags in the lecture file (HTML comment at top)
24. Update `MEMORY.md` with deliverable record
25. Present summary: files created, slide count, diagram count, registry entries added, what to review

### Error handling and partial completion

If the workflow is interrupted or a subagent fails:
- **Partial artifacts are kept.** The skill does not delete files on failure. Slides, diagrams, and exercises that were successfully written remain on disk.
- **Course state is NOT updated** until all Phase 3 steps succeed. This prevents the state file from recording a lecture that's only half-built.
- **The user is told** exactly what succeeded and what failed, with actionable next steps ("diagram X failed to compile — run `/tikz` to retry it standalone").

---

## `course.toml` — Per-Course Configuration

```toml
[course]
name = "CY-4973 Linux Kernel Security"
code = "CY-4973/7790"
semester = "SPR-2026"
institution = "WPI"

[paths]
# All paths are absolute or ~-relative
repo = "~/teaching/linux-kernel-security-class"
vault = "~/teaching/linux-malware-course-public"
lab = "~/teaching/aarch64-linux-qemu-lab"
assignments = "~/teaching/assignments-2026-spr"
tone_reference = "~/teaching/mcp_spr_2024/malware-course-public"

[paths.vault_dirs]
# Relative to [paths].vault
topics = "ObsidianVault/Lectures/Slides/Topics"
schedule = "ObsidianVault/Lectures/Slides/SPR-2026"
diagrams = "ObsidianVault/diagrams"
imgs = "ObsidianVault/Imgs/tikz"

[paths.repo_dirs]
# Relative to [paths].repo
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

# Where generated lecture .md files are written
# Relative to [paths].vault
output_dir = "ObsidianVault/Lectures/Slides/Topics"

[state]
# Both relative to [paths].repo
course_state = "course-state.md"
knowledge_registry = "knowledge"
```

Lives in the course repo root. A new course fills in paths/metadata. Nothing course-content-specific in the schema — all content lives in the knowledge registry and state file.

---

## Knowledge Registry

### Location

The `knowledge/` directory lives in the **course repo** (at the path specified by `[paths.repo_dirs].knowledge`, relative to `[paths].repo`). NOT in the Obsidian vault. If the course uses Obsidian, add the course repo (or symlink `knowledge/`) as a vault folder for wiki-link resolution.

### Structure

Per-concept Obsidian markdown files organized by category:

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
scope_history:
  - { lecture: kernel-shellcoding, level: introduced, date: 2026-03-17 }
  - { lecture: capstone-kernel, level: used, date: 2026-02-24 }
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

### Scope levels and evolution

| Level | Meaning | Skill behavior |
|---|---|---|
| `introduced` | First shown in this lecture | Full explanation on slide |
| `used` | Referenced but not the focus | Brief reminder: "recall from [[X]]" |
| `mastery` | Students should know this cold | No explanation, use freely |

The `scope` field is the **current** (latest) scope level. The `scope_history` array tracks how it evolved across lectures. The skill promotes scope automatically:

- A concept used in 3+ lectures after introduction → promoted to `mastery`
- A concept referenced in a later lecture → at minimum `used`
- The instructor can manually override scope in the frontmatter

### Querying

The skill reads frontmatter from all files in `knowledge/` to build an in-memory index. For a new lecture:
- Concepts with `mastery` scope → use without explanation
- Concepts with `used` scope → brief callout with wiki-link
- Concepts not in registry → flag as new, needs introduction or explicit scope assignment

### Bootstrap algorithm (for existing courses)

When populating the registry from existing lectures:

1. Scan each lecture file for code blocks, function names, type references, and concept keywords
2. Cross-reference with lab module source code to identify kernel API usage
3. For each identified item, determine the earliest lecture that explains it (not just uses it) — that's the `introduced_in` value
4. Set initial scope based on frequency: used in 1 lecture → `introduced`, 2 lectures → `used`, 3+ → `mastery`
5. Generate the `.md` files with frontmatter and a stub body
6. **Present the registry to the instructor for review** before committing — automated scope assignment is a heuristic, not ground truth

---

## `course-state.md` Format

Path: `[state].course_state` relative to `[paths].repo` (course repo root).

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

Read at the start of every `/lecture` invocation. Updated at the end. **Not updated if Phase 3 fails** (see error handling).

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

### Calibration

Before writing slides, the slide-writer agent reads one recent lecture from `[paths].tone_reference` as a voice calibration sample. This grounds the output in the instructor's actual writing style, not just the rule list.

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
2. **Check for reusable existing diagrams** — search `Imgs/tikz/` for SVGs matching the description. If a good match exists, return the embed syntax and skip creation.
3. Match description against 8 known patterns (see pattern definitions below)
4. If no pattern fits, freeform author using shared style
5. Write `.tex` to `diagrams/<category>/`
6. Update Makefile (add to category TEX list, add VPATH if new category)
7. Run `make <category>` — verify compile. **On failure:** read lualatex log, fix `.tex`, retry once. If second attempt fails, report error.
8. Run `make install` — copy SVG to imgs dir
9. Return embed syntax: `![[name.svg|width]]`

### Pattern definitions

Each pattern has a representative structure. The full reference with example `.tex` per pattern lives in `references/tikz-patterns.md`.

**Container** — two regions (e.g., userspace/kernel) with nodes inside and cross-boundary arrows.
```
\begin{scope}[on background layer]
  \node[usercontainer, fit=(...)] {};
  \node[kernelcontainer, fit=(...)] {};
\end{scope}
```
Use when: showing the boundary between privilege levels, address spaces, or system layers.

**Vertical cascade** — nodes stacked top-to-bottom with flow arrows.
Use when: call chains, page table walks, module lifecycle stages.

**Hook callout** — normal flow with a branch to a hook/callback node.
Use when: ftrace/kprobe interception, ioctl dispatch, syscall hooking.

**Side-by-side comparison** — two parallel columns with optional connecting arrow.
Use when: before/after, with/without hook, normal vs. malicious path.

**Bitfield** — horizontal bar divided into colored segments with labels.
Use when: PTE format, instruction encoding, struct memory layout.

**Timeline** — horizontal time axis with events, phases, and markers.
Use when: RCU grace periods, execution sequences, lifecycle events.

**Flow chain** — linear A → B → C → D with annotations between nodes.
Use when: data flow, exploit chain stages, processing pipeline.

**Parametric layout** — `\def` commands at top for distances, reusable spacing.
Use when: complex diagrams that may need resizing or are templates for a family of similar diagrams.

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

## Course Template Repo (v2)

Deferred to v2. For v1, the skill works against the existing `linux-kernel-security-class` repo with a `course.toml` added to its root. Once the schema stabilizes, a git template repo will be extracted with the standard directory structure:

```
course-template/
├── course.toml
├── course-state.md
├── CLAUDE.md
├── .gitignore
├── inclass/
├── hw/
├── lectures/
├── knowledge/{functions,types,concepts,syscalls,tools}/
└── scratch/
```

---

## Test Plan

### Bootstrap test: SPR-2026

1. Add `course.toml` to `linux-kernel-security-class/`
2. Scan all 14 delivered SPR-2026 lectures to populate `course-state.md`
3. Scan lectures + source modules to populate `knowledge/` registry using the bootstrap algorithm (scan → cross-reference → heuristic scope → instructor review)
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
- [ ] Instructor has reviewed and approved bootstrap scope assignments
- [ ] `/lecture` produces slides that render in Obsidian Advanced Slides
- [ ] All `:::` blocks in generated slides are balanced
- [ ] No blank-line or separator issues between slides
- [ ] TikZ diagrams compile with `make`
- [ ] Generated exercises compile and run
- [ ] Voice guide rules are followed (no AI-slop phrases)
- [ ] `course-state.md` is correctly updated after a new lecture
- [ ] Knowledge registry is updated with new entries after a new lecture
- [ ] Inline tags in lecture file match course-state tag index
