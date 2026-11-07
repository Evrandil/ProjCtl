# ProjCtl: TODO

- [ ] git pipeline: `pull merge commit push -u <name>` as `Cmd::Git::Pipeline` (iterator + `take`), with tests
- [ ] GitHub Actions CI: a workflow that builds and runs `ctest` on every push, so a broken commit shows a red ✗ on GitHub. Then a second workflow that, when you push a tag, builds a Release binary and attaches it to the GitHub release automatically. Your release process becomes git tag v0.6.0 && git push --tags.
- [ ] An AUR package \(PKGBUILD\)
- [ ] pc checkout <ref> for tags and commits (git switch --detach), if you ever want it.
- [ ] Sanitizers in Debug: the -fsanitize=address,undefined lines in projctl_warnings, so memory bugs show up at the exact line during development.

