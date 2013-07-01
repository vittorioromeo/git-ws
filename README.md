# git-ws

Command line utility to work with multiple git repositories at once <br/>
Currently intended for personal use only, but quite functional


## How to build

Tested on: **Arch Linux**, with **g++ 4.7.2**

1. Clone this repo
```bash
git clone https://github.com/SuperV1234/git-ws
```

2. Pull all submodules recursively
```bash
cd git-ws
./init-repository.sh
```

3. Build all submodules + `git-ws`
```bash
./build-repository-git-ws.sh
```

4. You may have to append `/usr/local/lib` to the `$PATH` or `$LD_LIBRARY_PATH` environment variables to allow Open Hexagon to find the required libraries - if that doesn't work, try:
```bash
sudo ldconfig /usr/local/lib  
```

5. Use `git-ws`!


## Usage

Using `git-ws` in a directory calls git commands in all subdirectiories which are git repositories.

* `git-ws ?`: displays auto-generated help for all commands
* `git-ws ? (command name)`: displays auto-generated help for a command
* `git-ws query`: displays all repos detected by git-ws
* `git-ws push`: push staged changes
* `git-ws push -f`: force push staged changes
* `git-ws push -a`: push staged changes, only in repos ahead from remote
* `git-ws pull`: pull latest version
* `git-ws pull -s`: pull latest version, stashing all changes before pulling
* `git-ws status` or `git-ws st`: displays short git status, ignoring dirty submodules
* `git-ws submodule push` or `git-ws sub push`: commits all staged changes and pushes (intended to be used only after non-submodule changes have been pushed)
* `git-ws sub pull`: stashes all submodule local changes, pulls latest submodules recursively
* `git-ws sub au`: equivalent of `git-ws submodule pull` followed by `git-ws submodule push` and by `git submodule update`
* `git-ws do 'command here'`: creates a subshell in every git repo folder, and executes a bash command in it
* `git-ws do -c 'command here'`: creates a subshell in every git repo folder with changes, and executes a bash command in it
* `git-ws do -a 'command here'`: creates a subshell in every ahead git repo folder, and executes a bash command in it



## Why is this useful?

Real-life situation: I'm working on Open Hexagon and related libraries. <br/>
I create a `OHWorkspace` folder, where I clone all the required git repos.

* `mkdir OHWorkspace; cd OHWorkspace`
* `git clone http://.../SSVUtils`
* `git clone http://.../SSVUtilsJson`
* `git clone http://.../SSVStart`
* `git clone http://.../<etc>`
* `git clone http://.../SSVOpenHexagon`

Now my directory tree looks like this:

```
OHWorkspace/
	...
	SSVUtils/
		.git/
		...
	SSVUtilsJson/
		.git/
		...
	SSVStart/
		.git/
		...
	SSVOpenHexagon/
		.git/
		...
```

I work on all the repos simultaneously. I constantly need to check what repos I've modified.

* `cd OHWorkspace/; git-ws st` (st == status)
* returns a human-readable list of changes

After I review changes, it's time to commit.

* `git-ws gitg -c` or `git-ws do "open your favorite GUI here"` (this opens gitg in every repo with changes to commit) 

I use gitg to make my commits.

* `git-ws push -a; git-ws sub au` (these two commands save a lot of time: first, all staged commits in ahead repos are pushed to GitHub - then, every submodule is recursively pulled, updated, and pushed, so that the online submodules are up-to-date)
