# git-ws

Command line utility to work with multiple git repositories at once <br/>
Currently intended for personal use only (use at your own risk)

<br/>

## Usage

Using `git-ws` in a directory calls git commands in all subdirectiories which are git repositories.

* `> git-ws ? (command name)`: displays auto-generated help for a command
* `> git-ws push`: push staged changes
* `> git-ws push -f`: force push staged changes
* `> git-ws pull`: pull latest version
* `> git-ws pull -s`: pull latest version, stashing all changes before pulling
* `> git-ws status`: displays short git status, ignoring dirty submodules
* `> git-ws submodule push`: commits all staged changes and pushes (intended to be used only after non-submodule changes have been pushed)
* `> git-ws submodule pull`: stashes all submodule local changes, pulls latest submodules recursively
* `> git-ws submodule au`: equivalent of `git-ws submodule pull` followed by `git-ws submodule push` and by `git submodule update`
* `> git-ws do 'command here'`: creates a subshell in every git repo folder, and executes a bash command in it