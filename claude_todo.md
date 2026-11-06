# ProjCtl: TODO

## 5. Output formatting

Rule: **interface layers return plain text or data → `ProjCtl` methods build the colourful output → `report()` prints it.**

- [ ] **one switch for colours**: add a member like
  ```cpp
  std::string_view paint(Style::Color color) const;   // "" when colours are off
  ```
  and replace `color_code(config.xxx)` with `paint(config.xxx)` everywhere (`RESET` too, e.g. `reset()`). Colours are off when:
  - `config.colors_enabled == false` (read from config, currently never used)
  - or stdout isn't a terminal: `!isatty(STDOUT_FILENO)` (`<unistd.h>`), so `pc list | grep x` and `cd (pc path x)` get clean text

## 6. Leftovers from the first review

- [ ] `host()`: HTTPS remotes (`https://github.com/...`) fail to parse (`gitinterface/host.cpp`). Find the host part after `://` or `@`, then check whether it contains `github` / `gitlab`
- [ ] README: new argument order, `-u`, `-a`, `add` forms, removed pipeline

## 7. Later

- [ ] git pipeline: `pull merge commit push -u <name>` as `Cmd::Git::Pipeline` (iterator + `take`), with tests
- [ ] fish completions: `pc list --names` + `completions/projctl.fish`
- [ ] CI: GitHub Actions running `ctest`
