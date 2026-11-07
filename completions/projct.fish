# fish completions for projctl
# install: ~/.config/fish/completions/projctl.fish (just you)
#      or: /usr/local/share/fish/vendor_completions.d/projctl.fish (system-wide)

# saved project names, one per line
function __projctl_projects
    projctl list --names 2>/dev/null
end

# no file completion unless a command below asks for it
complete -c projctl -f


# Commands (only offered while no command has been typed yet)

complete -c projctl -n __fish_use_subcommand -a list      -d "List saved projects"
complete -c projctl -n __fish_use_subcommand -a status    -d "Show project status"
complete -c projctl -n __fish_use_subcommand -a add       -d "Add a project"
complete -c projctl -n __fish_use_subcommand -a remove    -d "Remove a project"
complete -c projctl -n __fish_use_subcommand -a rm        -d "Remove a project"
complete -c projctl -n __fish_use_subcommand -a path      -d "Print project path"
complete -c projctl -n __fish_use_subcommand -a open      -d "Open project in editor"
complete -c projctl -n __fish_use_subcommand -a build     -d "Build project"
complete -c projctl -n __fish_use_subcommand -a run       -d "Run project"
complete -c projctl -n __fish_use_subcommand -a branch    -d "List or switch branches"
complete -c projctl -n __fish_use_subcommand -a fetch     -d "git fetch"
complete -c projctl -n __fish_use_subcommand -a pull      -d "git pull"
complete -c projctl -n __fish_use_subcommand -a merge     -d "git merge"
complete -c projctl -n __fish_use_subcommand -a commit    -d "Add all changes and commit"
complete -c projctl -n __fish_use_subcommand -a push      -d "git push"
complete -c projctl -n __fish_use_subcommand -a log       -d "git log"
complete -c projctl -n __fish_use_subcommand -a diff      -d "git diff"
complete -c projctl -n __fish_use_subcommand -a git       -d "GitHub / GitLab CLI status"
complete -c projctl -n __fish_use_subcommand -a repos     -d "List remote repositories"
complete -c projctl -n __fish_use_subcommand -a ilist     -d "List issues"
complete -c projctl -n __fish_use_subcommand -a icreate   -d "Create an issue"
complete -c projctl -n __fish_use_subcommand -a iview     -d "View an issue"
complete -c projctl -n __fish_use_subcommand -a iedit     -d "Edit an issue"
complete -c projctl -n __fish_use_subcommand -a iclose    -d "Close an issue"
complete -c projctl -n __fish_use_subcommand -a reqlist   -d "List pull/merge requests"
complete -c projctl -n __fish_use_subcommand -a reqcreate -d "Create a pull/merge request"
complete -c projctl -n __fish_use_subcommand -a reqview   -d "View a pull/merge request"
complete -c projctl -n __fish_use_subcommand -a reqedit   -d "Edit a pull/merge request"
complete -c projctl -n __fish_use_subcommand -a reqclose  -d "Close a pull/merge request"
complete -c projctl -n __fish_use_subcommand -a reqmerge  -d "Merge a pull/merge request"


# Project names, for every command that takes one

complete -c projctl -n "__fish_seen_subcommand_from status remove rm path open build run branch fetch pull merge commit push log diff ilist icreate iview iedit iclose reqlist reqcreate reqview reqedit reqclose reqmerge" \
    -a "(__projctl_projects)" -d "Project"


# Options

complete -c projctl -n "__fish_seen_subcommand_from list" -a gits -d "Only git repositories"
complete -c projctl -n "__fish_seen_subcommand_from list" -s n -l names -d "Names only"

complete -c projctl -n "__fish_seen_subcommand_from status fetch pull ilist reqlist" -s a -l all -d "All projects"

complete -c projctl -n "__fish_seen_subcommand_from push" -s u -d "Set upstream"
complete -c projctl -n "__fish_seen_subcommand_from log"  -s o -d "One line per commit"


# Paths

complete -c projctl -n "__fish_seen_subcommand_from add" -a "(__fish_complete_directories (commandline -ct))" -d "Directory"
complete -c projctl -n "__fish_seen_subcommand_from diff" -F
