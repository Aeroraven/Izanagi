# AGENTS

Purpose: quick guide for anyone adding experiments.

- Structure: keep each experiment self-contained in its own folder with its own build files; avoid cross-project includes; prefer CMake for C++.
- Languages: C++, C#, Python. Use CMake (>=3.20) as the default build system for C++ samples.
- Documentation: each new experiment gets a short README explaining the goal, build steps, and dependencies. Update the root README when adding or removing experiments.
- Assets & outputs: do not commit build outputs, large binaries, or generated files. Keep assets small and clearly licensed.
- Quality: favor minimal, clear samples over frameworks; add concise comments only where intent is non-obvious; keep main branches clean.
