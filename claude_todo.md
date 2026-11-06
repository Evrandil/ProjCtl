# ProjCtl: TODO

## 3. Handlers (`handle.cpp`)

- [x] **`Cmd::Unknown` with a message**
  - [x] `struct Unknown { std::string message; };`
  - [x] parser: pass `optional_name`'s errors along (`Cmd::Unknown{ .message = name.error() }`), plus short usage lines for the other cases (`"usage: projctl push [-u] [<name>]"`)
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
