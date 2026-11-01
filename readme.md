Just for record I did have [this](https://github.com/Projctl/projctl) repo stolen by some guy that got my auth token so until I get my org with repo back here's gonna be continuation
# Project Control
Have you ever wondered if you're in gh or glab repo so what command to run?<br>
Have you ever mistook one for other?<br>
I did<br>
And it was annoying<br>
So here it is wrapper for git management \(haven't rly done anything on CodeBerg so I don't have that one wrapped\)<br>
Tho honestly it's useful not only for git projects<br><br>

It's nice to alias `pc=projctl` and `p` as `cd (projctl path $argv[1])` \(I use fish Idrk about bash or zsh\)

Dependencies:
```fish
cmake 3.20
ninja
just
```
C++ standard is set to 2023 so I guess that's requirement for compiler<br>
Also have $XDG_CONFIG_HOME and $XDG_CACHE_HOME set to some location<br>
```fish
./install.fish
```

# Usage \[fast docs\]

Overall concept<br>
```bash
projctl [option/s] [project_name] [argument/s]
```

### List
Lists projects<br>
With gits or git will list only git repos<br>
```fish
projctl list 
projctl list gits
```

### Status
Gives status of project \(/ all projects if used with --all flag\)<br>
Status contains<br>
```
    Name:               [Name]<br>
    Path:               [Path]<br>
    Exists:             [If project exists on disk, "No" if it has been removed / moved]<br>
    Type:               [Lang / environment, currently supported are: { "C++/CMake", "Rust", "Node", "Python" } ]<br>
    Git:                [(If git repo) current workspace/repo_name]<br>
    Branch:             [(If git repo) current working branch]<br>
    Full remote:        [(If git repo) full remote path]<br>
    Status:             [(If git repo)If there were unpushed / uncommitted changes, if yes lists files with changes below]<br>
```
```fish
projctl status [name]
projctl status --all
```

### Add
Adds project to saved projects<br>
You can set it to auto-add current directory as project with name of dir<br>
Also you can set name of dir and add <br>
USE OBJECTIVE PATHS for now at least<br>
Tho "." I did make to be converted to objective path of current dir<br>
```fish
projctl add [name] [path]
projctl add [name] .
projctl add .
```

### Remove
Removes project<br>
\(Doesn't actually remove files, just from projctl list\)<br>
```fish
projctl remove [name]
projctl rm [name]
```

### Open
Opens neovim in project location<br>
Soon \(I hope\) will open editor set in config but well not on that stage yet<br>
```fish
projctl open [name]
```

### Path
Gives path of project<br>
Nice for `cd (projctl path $argv[1])` terminal macro<br>
```fish
projctl path [name]
```

### Build && run
Respectively builds and runs projects that either have customely added `build` and `run` commands or have supported default commands \(Rust && node supported for both, CMake + Ninja does have build\)<br>
Build also supports --all flag<br>
```fish
projctl build [name]
projctl build --all
projctl run [name]
```

## Git
Here we go into the rabbithole<br>
### Branch
Switches / creates and switches to branch of given name<br>
Or lists branches<br>
```fish
projctl branch [name] [branch_name]
projctl branch [name]
```

### Fetch
Fetches project<br>
Supports --all flag \(performs fetch to all git projects\)<br>
```fish
projctl fetch [name]
projctl fetch --all
```

### Pull
Updates project<br>
Supports --all flag \(performs pull for all git projects\)<br>
```fish
projctl pull [name]
projctl pull --all
```
### Commit
Adds all changes and makes commit with provided message \(equivalent to `git add . && git commit -m \[commit_message\]`\)<br>
Without providing msg it's gonna open $EDITOR for you to write msg to \(equivalent to `git add . && git commit`\)<br>
```fish
projctl commit [name]
projctl commit [name] [commit_message]
```

### Push
Pushes committed changes<br>
If used with branch pushes changes to uninitialized branch \(equivalent to `git push -u origin \[current_branch\]`\)<br>
```fish
projctl push [name]
projctl push branch [name]
```

### Commit push
Performs both actions, you can combo it with branch as well to set upstream to new branch<br>
```fish
projctl commit push [name]
projctl commit push [name] [commit_message]
projctl commit push branch [name]
projctl commit push branch [name] [commit_message]
```

### Merge
It performs just and only `git merge`, no other options tbh \(so for more specific merge options do use git command itself\), 
I made this just to have quick pipe merge -> commit -> push in one pc command, not to be actual useful merge<br>
```fish
projctl automerge [name]
projctl automerge commit [name]
projctl automerge commit [name] [commit_msg]
```

### Git pipeline
Also git options will work in pieline \(as one option so you don't have to write project name every time\) with this order:<br>
automerge -> commit -> push \(with branch option\)
```fish
projctl pull automerge commit push [name]
projctl pull automerge commit [commit_msg] push [name]
projctl pull automerge commit push branch [name]
projctl pull automerge commit [commit_message] push branch [name]
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
prjctl repos
```

### Issues
List or create issues for repo<br>
```fish
projctl ilist [name]
projctl icreate [name]
```

### Requests (PR \(GH\) / MR \(GLab\))
List or create requests for repo<br>
```fish
projctl reqlist [name]
projctl reqcreate [name]
```

<br><br><br>
Also minor thing but for process to be actually saving some time everything with --all flag is done in parrallel but for that to happen program stores output and when everything is done prints it in order so there's no classic `some console print -> some waiting -> some more printing` for example in pull command, everything gets printed all at once when waiting is done, but thanks to that all thing are async so it' kinda faster one could argue<br>
And just for record I do expect one to use this with some concept they wanna do so haven't tested any and every combination of commands / args you could do around that git pipeline so yeah beware
