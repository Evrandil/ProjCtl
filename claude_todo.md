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
- [ ] **`open` / `build` / `run` without a name crash in their messages**: same pattern, `*name` used for printing
  - `projects.cpp:131` (`open` success message), `:150` (`build` "no default command"), `:171` (`run` "no default command")
  - [ ] use the real project name instead: `(*project)->first`

## 2. `--all` / `-a` (currently ignored)

The parser sets `cmd.all` correctly, but nothing reads it. `handle` passes only `cmd.name`, and the methods still check `name == "--all"`, which can never be true now. `pc status -a` inside a project only shows that project, and outside one it says "Current directory isn't a saved project".

- [ ] split into "one" and "all" versions, with `handle` choosing:
  - [ ] `status` / `status_all`: `git.cpp:83`
  - [ ] `git_pull` / `git_pull_all`: `git.cpp:190`
  - [ ] `git_fetch` / `git_fetch_all`: `git.cpp:232`
  - [ ] `issue_list` / `issue_list_all`: `issues.cpp:166`
  - [ ] `request_list` / `request_list_all`: `requests.cpp:160`
  - pattern: `return report(cmd.all ? status_all() : status(cmd.name));`
- [ ] check that nothing is left over: `grep -rn '"--all"\|== "\."' src/` should only find `parser.cpp`

cmd_of(project) / cmd(name) { runs cmd_of(project of arged name) } / cmd_all() { runs cmd_of(all projects) }

## 3. Handlers (`handle.cpp`)

- [ ] **one reporting helper for `expected<int, …>`**: `build`, `run`, `icreate` and `reqcreate` each repeat the error printing, and print errors to **stdout**
  - e.g. `int report_code(const std::expected<int, std::string>&) const;` (error → stderr, return 1; value → return it as the exit code)
- [ ] **`path` errors go to stdout** (`handle.cpp:60`), so `cd (pc path typo)` still tries to `cd` into the message → stderr
- [ ] **`Cmd::Unknown` with a message**
  - [ ] `struct Unknown { std::string message; };`
  - [ ] parser: pass `optional_name`'s errors along (`Cmd::Unknown{ .message = name.error() }`), plus short usage lines for the other cases (`"usage: projctl push [-u] [<name>]"`)
  - [ ] `handle(const Cmd::Unknown&)`: print the message to **stderr** (currently stdout, no reason given)
- [ ] `push`: one `git_push(name, bool upstream)` instead of `git_push` + `git_push_branch` (your TODO)
- [ ] `branch`: one function taking the optional branch name instead of `git_branch` + `git_branch_list` (your TODO)

## 4. Failures that look like success

These return a **successful** `expected` holding the `✗` text, so `report()` prints it as success and the exit code is 0:

- [ ] `git.cpp:129` `git_commit`, `:144` `git_push`, `:173` `git_push_branch`, `:186` pull (one), `:227` fetch (one), `:273` `git_branch_list`, `:288` `git_branch`
- [ ] pattern: `if (!result) { return std::unexpected(result.error()); }` first, then format only the success case (`git_merge` already does it right)
- [ ] for the `_all` versions: decide whether one failed project means exit code 1 (probably yes)

## 5. Output formatting

Rule: **interface layers return plain text or data → `ProjCtl` methods build the colourful output → `report()` prints it and styles errors.**

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
- [ ] `list_gits`: `*GitInterface::branch(...)` / `*GitInterface::remote_short(...)` without checking (`git.cpp:18`). A repo without `origin` is UB
- [ ] GitHub `author.name` → `author.login` (`githubinterface.cpp:96`, `:150`). `name` is often empty
- [ ] JSON parsing that can't throw: `json::parse(text, nullptr, false)` + `is_discarded()`, or a `try`/`catch (const nlohmann::json::exception&)` around the parsing loops
- [ ] `.cashe` → `.cache` (`systeminterface.cpp:214`)
- [ ] `zig run` → `zig build run` (`projectcontent.hpp:70`), and detect `build.zig` in `detect_project_type`
- [ ] `truncate`: cuts 5, adds 3 (`...`), so results come out 2 short (`helper.cpp`)
- [ ] README: new argument order, `-u`, `-a`, `add` forms, removed pipeline

## 7. Later

- [ ] git pipeline: `pull merge commit push -u <name>` as `Cmd::Git::Pipeline` (iterator + `take`), with tests
- [ ] fish completions: `pc list --names` + `completions/projctl.fish`
- [ ] CI: GitHub Actions running `ctest`
