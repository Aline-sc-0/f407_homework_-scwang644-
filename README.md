# STM32F407 Homework

本仓库是基于 STM32F407IGH6 (C板) 和 STM32F407VGT6 (最小系统板) 的作业仓库模板

## Design

我们的作业设计将尽可能围绕对灯效的操作展开，每次的内容都会在上一次的内容的基础上进行增加与精进，尽量能够覆盖之前的内容并有一个良好的延续性。

请大家良好的进行版本管理并体会版本管理的需求与优越性，之后的作业都通过git仓库链接进行提交，同时可以通过 LOG.md 书写每次完成的任务与任务截图。

## Usage

创建个人仓库的时候，可以通过

```bash
git clone https://github.com/HKUSTGZ-ROBOMASTER-PNX/stm32f407_hw_template.git
```

将本仓库克隆到本地，创建自己的仓库，设置可见性为 public

![create](assets/image.png)

并将仓库命名为 f407_homework_[your name or number e.g. szhang123]，之后的所有作业应当在你的仓库进行提交与版本管理。

可以只选择你拿到的板子文件夹并重新放到自己的仓库。

基础的操作包括以下几个：

```bash
git remote add upstream [your repository url]
```

设置项目的上游仓库，如果你是现在本地创建的文件夹希望同步到你的仓库

```bash
git add .
```

在每一次添加新内容之后，都需要执行这个命令让git了解你的更改

```bash
git commit -m "[init/feat/chore/fix/refactor]:[conclusion of change]"
```

添加完内容之后，需要对本次提交添加描述，描述请严格按照上面的格式进行，可以分为

- init: 初始化本仓库，用于第一次提交
- feat: 为仓库添加新功能
- fix: 修复了仓库的某些bug
- chore: 进行一些与主题代码无关的改动
- refactor: 对代码架构进行重构

后面接着对当前修改的简要描述。

```bash
git push origin
```

将修改推送到远端。