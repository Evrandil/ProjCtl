# ProjCtl: TODO

State as of `a6162be` ("No way it compiled"). Parser done, 294/294 tests green, whole project compiles.
Items in order: fix the crashes first, then make features work again, then the cleanup.

---

## 1. Crashes (empty optional dereferenced)

- [ ] **`add` without a name crashes**: `pc add`, `pc add .`, `pc add /path .`
  - `handle.cpp:71`: `cmd.name == "."` is never true anymore (the parser turns `.` into an empty optional), so `add_current()` is never called
  - `projects.cpp:71`: `{*name, …}` dereferences the empty optional
  - [ ] in `add()`: `path` = `std::filesystem::absolute(*path)` if given, else `current_path()`
  - [ ] in `add()`: `name` = `*name` if given, else the folder name: `path.filename()`, or `path.parent_path().filename()` when the path ends with `/` (then `filename()` is empty)
  - [ ] check the **resolved** name against `projects.contains(...)`, not only when one was typed
  - [ ] delete `add_current()` (`projects.cpp:79`, `projctl.hpp:43`) and the `if` in `handle(const Cmd::Base::Add&)`
  - [ ] optional: reject names containing `/` in `optional_name` (`pc add proj /path`, old argument order, currently creates a project *named* `/path`) and add a test

## 2. `--all` / `-a` (currently ignored)

The parser sets `cmd.all` correctly, but nothing reads it. `handle` passes only `cmd.name`, and the methods still check `name == "--all"`, which can never be true now. `pc status -a` inside a project only shows that project, and outside one it says "Current directory isn't a saved project".

- [ ] check that nothing is left over: `grep -rn '"--all"\|== "\."' src/` should only find `parser.cpp`

cmd_of(project) / cmd(name) { runs cmd_of(project of arged name) } / cmd_all() { runs cmd_of(all projects) }

## 3. Handlers (`handle.cpp`)

- [ ] **`Cmd::Unknown` with a message**
  - [ ] `struct Unknown { std::string message; };`
  - [ ] parser: pass `optional_name`'s errors along (`Cmd::Unknown{ .message = name.error() }`), plus short usage lines for the other cases (`"usage: projctl push [-u] [<name>]"`)
  - [x] `handle(const Cmd::Unknown&)`: print the message to **stderr** (currently stdout, no reason given)

## 5. Output formatting

Rule: **interface layers return plain text or data → `ProjCtl` methods build the colourful output → `report()` prints it.**

- [ ] **errors as plain text**: remove `color_code(...)` / `Style::RESET` from every `std::unexpected(...)` message. `report()` already adds the red colour and `✗`, so these get double-styled:
  - `projects.cpp:65` (already exists), `:150` / `:171` (no default build/run command), the error branches in `issues.cpp` / `requests.cpp`
- [ ] **one switch for colours**: add a member like
  ```cpp
  std::string_view paint(Style::Color color) const;   // "" when colours are off
  ```
  and replace `color_code(config.xxx)` with `paint(config.xxx)` everywhere (`RESET` too, e.g. `reset()`). Colours are off when:
  - `config.colors_enabled == false` (read from config, currently never used)
  - or stdout isn't a terminal: `!isatty(STDOUT_FILENO)` (`<unistd.h>`), so `pc list | grep x` and `cd (pc path x)` get clean text
- [ ] `GitInterface` / `GitHubInterface` / `GitLabInterface` / `SystemInterface` stay colour-free (they already are, keep it that way)

## 6. Leftovers from the first review

- [ ] `host()`: HTTPS remotes (`https://github.com/...`) fail to parse (`gitinterface/host.cpp`). Find the host part after `://` or `@`, then check whether it contains `github` / `gitlab`
- [ ] README: new argument order, `-u`, `-a`, `add` forms, removed pipeline

## 7. Later

- [ ] git pipeline: `pull merge commit push -u <name>` as `Cmd::Git::Pipeline` (iterator + `take`), with tests
- [ ] fish completions: `pc list --names` + `completions/projctl.fish`
- [ ] CI: GitHub Actions running `ctest`
