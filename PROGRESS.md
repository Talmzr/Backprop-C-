# Progress

## Current milestone

**M0 — Setup.** Build setup written and passing from the command line (2026-10-12). Not yet confirmed from VS Code, and not yet committed or pushed.

## Done

- `CLAUDE.md` reviewed and revised with the user (2026-10-11).
- Project moved to `C:\dev\Backprop`. Git repo created, connected to GitHub, first commit pushed.
- M0 build setup (2026-10-12):
  - `CMakeLists.txt` and `CMakePresets.json` with three presets: `debug`, `asan`, `release`. Each builds into `build/<preset>`.
  - Catch2 v3.16.1 via `FetchContent`; one example test in `tests/example_test.cpp`.
  - Two targets: `backprop` (from `Main.cpp`) and `tests`.
  - Debug: Catch2's CRT leak report is on, and a test whose output contains a leak report fails.
  - ASan: `/fsanitize=address`, without `/RTC1` and incremental linking; the runtime DLL is copied beside the executables.
  - `.vscode` settings for CMake Tools, `.gitignore`, `NOTES.md`.
  - Configure, build and the example test pass in all three presets, both inside the Visual Studio developer environment and from a plain shell.

## Next step

1. User: disable the C/C++ Runner extension for this workspace, then pick a preset in VS Code and run the tests from the Testing panel.
2. User: commit and push.
3. Ask the user for M0 takeaways for `NOTES.md`, then start M1a.

## Open problems / questions

- **VS Code: build confirmed, tests not yet.** The user built the `debug` preset from VS Code on 2026-10-12 (exit code 0; CMake Tools found the CMake bundled with Visual Studio). Running the tests from the Testing panel, and the `asan` and `release` presets, are still unconfirmed there.
- **The C/C++ Runner extension keeps writing into `.vscode`.** It re-added its gdb entry to `launch.json` right after the file was replaced. Once the user disables it for this workspace, that entry can be removed.
- **Where the engine code will live is not set up in CMake yet.** `backprop` and `tests` share no code so far. The user designs the file layout in M1a; CMake gets a shared target then.
- **Neither memory checker has been seen catching anything** (see Decisions log, 2026-10-12). ARM64 support in MSVC ASan is recent.

## Machine

- Windows 11 on ARM64 (Snapdragon X Plus). Presets build native ARM64.
- Visual Studio Community 2026 (18.3, Insiders), MSVC 14.50. CMake 4.1.2 and Ninja 1.12.1 ship inside it; neither is on `PATH`.
- Command-line builds need the developer environment first: `VsDevCmd.bat -arch=arm64 -host_arch=arm64` (in `Common7\Tools` of the Visual Studio install).
- VS Code has CMake Tools and the C/C++ extension.

## Decisions log

- 2026-10-11: Project lives at `C:\dev\Backprop`, outside OneDrive; GitHub is the only backup. Reason: OneDrive is full, and sync would lock build files.
- 2026-10-11: The Git remote uses the personal GitHub account Talmzr (`https://Talmzr@github.com/Talmzr/Backprop-C-.git`). Reason: this machine was signed in as Talmzrahi, which cannot push to the repo.
- 2026-10-11: When `CLAUDE.md` and this file disagree, Claude tells the user what each says and asks.
- 2026-10-11: Decisions are raised when needed, not in advance (user's preference).
- 2026-10-12: MSVC comes from Visual Studio Community 2026, not the standalone Build Tools `CLAUDE.md` mentions. Same compiler; it was already installed.
- 2026-10-12: The deliberate leak and out-of-bounds checks are dropped from M0 (removed from `CLAUDE.md` too). Reason: the user knows the tools and doesn't want to spend time proving them.
- 2026-10-12: Ninja with one build folder per preset, instead of a Visual Studio multi-configuration project. Reason: ASan needs different flags for every target including Catch2, which is simplest as a separate build tree.
- 2026-10-12: ASan is the Debug build type plus a `BACKPROP_ASAN` option, not a custom build type. Reason: keeps the standard Debug flags and debug CRT, minus `/RTC1`.
- 2026-10-12: `/W4` on the project's own targets, warnings not treated as errors.
- 2026-10-12: `.vscode` stays tracked in Git. Reason: it now holds project settings with no machine-specific paths.
