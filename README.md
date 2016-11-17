# CIS460 Final Project - Mini Minecraft #


### Attention

* `master` branch is used for the stable version. **Please do not directly `git push` to the `master` branch.** Use `git config --global push.default matching` to configure your `git push` command to push to the corresponding branch.

* There're 3 other branches, corresponding to the three parts of the week1 milestone. Please use `git clone -b <your-branch-name> <remote-repo>` to clone your branch to your machine.

* `git clone <remote-repo>` directly also works. But it clones the `master` branch by default. **Use `git checkout <your-branch-name>` to switch your branch from `master` to `<your-branch-name>`.**

* push用这个指令，以我的为例：git push origin dev-procedural-terrain:dev-procedural-terrain，第一个dev-procedural-terrain指的是本地的dev-procedural-terrain，第二个指的是远程仓库的dev-procedural-terrain。不要直接用git push，会把所有的分支都push上去。

* git pull自己分支的指令是：`git pull origin <your-branch-name`