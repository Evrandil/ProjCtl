![CI](https://github.com/Evrandil/ProjCtl/actions/workflows/ci.yml/badge.svg)
Just for record I did have [this](https://github.com/Projctl/projctl) repo stolen by some guy that got my auth token so until I get my org with repo back here's gonna be continuation
# Project Control
Have you ever wondered if you're in gh or glab repo so what command to run?<br>
Have you ever mistook one for other?<br>
I did<br>
And it was annoying<br>
So here it is wrapper for git management \(haven't rly done anything on CodeBerg so I don't have that one wrapped\)<br>
Tho honestly it's useful not only for git projects<br><br>

Dependencies:
```fish
cmake 3.20
ninja
just
```
C++ standard is set to 23 so I guess that's requirement for compiler<br>
\(RIP folks on Windows\)<br>
# Installatoin
## If you have just
```fish
just install
```
## No just
Tbh just doesn't give you any advantage, it just lists those comand for shell so you don't have to think what commands to run<br>
```fish
cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release -DPROJCTL_BUILD_TESTS=OFF
cmake --build build-release
sudo install -m 755 -s build-release/projctl /usr/local/bin/projctl
```
Do use release build type there \(or at least -O3 bc binary does be able to get big XD\) with `-s` flag for install, trust me

# Usage \[fast docs\]

Overall concept<br>
```bash
projctl [option/s] [argument/s] [project_name]
```

And for many commands you can ommit like every option / argument, for grammar just read further<br>
For grammar:<br>
- `<var>` - some variable
- `[opt]` - optional value
- `this | other` - or

### List
Lists projects<br>
With gits will list only git repos<br>
`-n` / `--names` flag for names-only \(wihtout headers, without colors\), nice for completions<br>
```fish
projctl list [-n | gits [-n]]
```

### Status
Gives status of project \(/ all projects if used with --all flag\)<br>
Status contains<br>
```
    Name:               [Name]
    Path:               [Path]
    Exists:             [If project exists on disk, "No" if it has been removed / moved]
    Type:               [Lang / environment, currently supported are: { "C++/CMake", "Rust", "Node", "Python" } ]
    Git:                [(If git repo) current workspace/repo_name]
    Branch:             [(If git repo) current working branch]
    Full remote:        [(If git repo) full remote path]
    Status:             [(If git repo)If there were unpushed / uncommitted changes, if yes lists files with changes below]
```
```fish
projctl status [<name> | --all | -a]
```

### Add
Adds project to saved projects<br>
You can set it to auto-add current directory as project with name of dir<br>
Also you can set name of dir and add <br>
```fish
projctl add [[<path>] <name>]
```

### Remove
Removes project<br>
\(Doesn't actually remove files, just from projctl list\)<br>
Also it rejects calls without name or with "." \(on purpose\)
```fish
projctl remove | rm <name>
```

### Open
Opens neovim \(or your editor of config choice\) in project location<br>
```fish
projctl open [<name>]
```

### Path
Gives path of project<br>
Nice for `cd (projctl path $argv[1])` terminal macro<br>
```fish
projctl path [<name>]
```

### Build && run
Respectively builds and runs projects that either have customely added `build` and `run` commands or have supported default commands \(Rust && node supported for both, CMake + Ninja does have build\)<br>
```fish
projctl build [<name>]
projctl run [<name>]
```

## Git
Here we go into the rabbithole<br>
### Branch
Switches / creates and switches to branch of given name<br>
Or lists branches<br>
```fish
projctl branch [[<branch>] <name>]
```

### Fetch
Fetches project<br>
Supports --all flag \(performs fetch to all git projects\)<br>
```fish
projctl fetch [<name> | --all | -a]
```

### Log
Gives you log of repo, strongly recommended to pipe with `less` \(otherwise you **will** get log of every commit on your screen\)<br>
Supports `-o` flag for `--oneline` equivalent
```fish
projctl log [-o] [<name>]
```

### Diff
Gives you `git diff` preview of repo / file<br>
```fish
projctl diff [[<filename>] <name>]
```

### Pull
Updates project<br>
Supports --all flag \(performs pull for all git projects\)<br>
```fish
projctl pull [<name> | --all | -a]
```
### Commit
Adds all changes and makes commit with provided message \(equivalent to `git add . && git commit -m \[commit_message\]`\)<br>
Without providing msg it's gonna open $EDITOR for you to write msg to \(equivalent to `git add . && git commit`\)<br>
Not yet implemented but in soon future will be `-u` option \(to add changes only of already tracked files, like `git add -u`\)<br>
```fish
projctl commit [[<message>] <name>]
```

### Push
Pushes committed changes<br>
If used with branch pushes changes upstream to HEAD \(equivalent to `git push -u origin HEAD`\)<br>
```fish
projctl push [-u] [<name>]
```

### Merge
It performs just and only `git merge`, no other options tbh \(so for more specific merge options do use git command itself\), 
I made this just to have quick pipe merge -> commit -> push in one pc command, not to be actual useful merge<br>
```fish
projctl merge [<name>]
```

### Git pipeline
No pipes currently work after refactor, it was good refactor but will take care of pipelines tomorrow or so \[*or so...*\]<br>
Also git options will work in pieline \(as one option so you don't have to write project name every time\) with this order:<br>
automerge -> commit -> push \(with branch option\)
```fish
projctl pull merge commit push <name>
projctl pull merge commit <commit_msg> push <name>
projctl pull merge commit push branch <name>
projctl pull merge commit <commit_message> push branch <name>
```

## GitHub / GitLab

### Status
To see installed and authenticated status
```fish
projctl git
```

### Repos
To see all repos on both platforms
```fish
projctl repos
```

### Issues
List or create issues for repo<br>
```fish
projctl ilist [<name> | --all | -a]
projctl icreate [<name>]
projctl iedit [<name>]
projctl iview [<name>]
projctl iclose [<name>]
```

### Requests (PR \(GH\) / MR \(GLab\))
List, create requests or merge open requests for repo<br>
Reqmerge is just squash merge with delete source repo option
```fish
projctl reqlist [<name> | --all | -a]
projctl reqcreate [<name>]
projctl reqedit [<name>]
projctl reqview [<name>]
projctl reqclose [<name>]
projctl reqmerge [<name>]
```

<br><br><br>
Also minor thing but for process to be actually saving some time everything with --all flag is done in parrallel but for that to happen program stores output and when everything is done prints it in order so there's no classic `some console print -> some waiting -> some more printing` for example in pull command, everything gets printed all at once when waiting is done, but thanks to that all thing are async so it's kinda faster one could argue<br>
And it automatically detects non-interactive sessions so you can use it in nvim as `!pc [whatever functionality you want]`
