<div align="center">
<img src="deeptags.png" alt="DeepTags" width="150"> 
</div>

<h1 align="center">DeepTags</h1>

<!-- <div align="center"> -->
<!-- <a href="https://github.com/SZinedine/DeepTags/releases"><img src="https://img.shields.io/github/v/release/SZinedine/DeepTags" alt="Github Releases" /></a> <a href="https://travis-ci.org/SZinedine/DeepTags"><img src="https://travis-ci.org/SZinedine/DeepTags.svg?branch=master" alt="Build Status" /></a> <a href="https://ci.appveyor.com/project/SZinedine/DeepTags"><img src="https://ci.appveyor.com/api/projects/status/github/SZinedine/DeepTags" alt="Build Status Windows" /></a> -->
<!-- </div> -->

**DeepTags** is a Markdown notes manager that organizes notes according to tags.

DeepTags supports nested tags and offers simple ways to edit them, for example by dragging and dropping a tag on a note. These notes could be read either with the integrated editor or with one or multiple third party markdown editors installed on your system. Make sure to add them into the the app from `Edit -> Markdown Readers`

![Screenshot of DeepTags on a Linux machine running plasma 5](Screenshot.png)


## Features

* **Nested tags**: You can create a hierarchy of tags to organize your notes in a tree structure. For example, the representation of the nested tag: `places/africa/algeria`, would be:

<p align="center">
<img src="tag_hierarchy.png" alt="tag hierarchy">
</p>

* **Drag and Drop** a tag into a note to add it to it
* **Search** through your notes by title
* **No-Cloud**: DeepTags is running completely offfline.
* **External Editors**: You can use your favorite markdown editor.


## Dependencies

- The Qt framework (>=)5.5
- A C++14 compiler


## Downloads

You can download the latest release [here](https://github.com/SZinedine/DeepTags/releases/latest).

### Linux

#### Building from source

```bash
git clone --recursive https://github.com/SZinedine/DeepTags.git
cd DeepTags
qmake && make
```

#### Install on Arch Linux

```bash
git clone https://aur.archlinux.org/deeptags.git
cd deeptags
makepkg -sic
```

or
```bash
yay -S deeptags
```

## Credit
- [QMarkdownTextEdit](https://github.com/pbek/qmarkdowntextedit) used as an integrated reader.
- Yannick Lung's [icons](https://www.iconfinder.com/yanlu) are used throughout the app.

