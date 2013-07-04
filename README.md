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

Auto generated help
```bash
<? || help>  (OPTARG Command name) [-v || --verbose] 

>>Show help for all commands or a single command.

	Optional arguments:
* (OPTARG Command name)
  --Command name
  --Name of the command to get help for.
  --Leave blank to get general help.


	Flags:
* [-v || --verbose]
  --Verbose general help?



<push>   [-f || --force] [-a || --ahead-only] 

>>Pushes every git repo.

	Flags:
* [-f || --force]
  --Forced pull?

* [-a || --ahead-only]
  --Run the command only in folders where repos are ahead of the remote?



<pull>   [-s || --stash] [-f || --force-checkout] [-c || --changed-only] 

>>Pulls every git repo.

	Flags:
* [-s || --stash]
  --Stash all changes before pulling?

* [-f || --force-checkout]
  --Run a force checkout before pulling?

* [-c || --changed-only]
  --Run the command only in folders where repos have changes?



<sub || submodule> (ARG Action)  [-c || --changed-only] 

>>Work with git submodules in every repo.

	Required arguments:
* (ARG Action)
  --Action
  --Action to run for every submodule. Can be 'push', 'pull' or 'au'.
  --'push' commits all changes in the repo and pushes them to the remote. Do not run this unless all non-submodule changes have been taken care of!
'pull' recursively pulls the latest submodules from the remote.
'au' calls 'pull' then 'push' in succession.


	Flags:
* [-c || --changed-only]
  --Run the command only in folders where repos have changes?



<st || status>   [-a || --showall] 

>>Prints the status of all repos.

	Flags:
* [-a || --showall]
  --Print empty messages?



<gitg>   [-c || --changed-only] 

>>Open the gitg gui application in every repo folder.

	Flags:
* [-c || --changed-only]
  --Open gitg only in folders where repos have changes?



<do> (ARG Command to run)  [-c || --changed-only] [-a || --ahead-only] 

>>Runs a shell command in every repo folder.

	Required arguments:
* (ARG Command to run)
  --Command to run
  --This is the command that will be called in every repo folder.
  --Consider wrapping a more complex command with quotes.


	Flags:
* [-c || --changed-only]
  --Run the command only in folders where repos have changes?

* [-a || --ahead-only]
  --Run the command only in folders where repos are ahead of the remote?



<query>    

>>Queries the status of all the repos, returning whether they are changed or ahead.

```

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
