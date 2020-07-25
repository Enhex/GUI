# Project structure

right now `src` contains sub-folders for independent components that should be standalone libraries later on.
event - event system
layout - layout system
input - input system, builds on top of the event system
gui - core GUI system, integration of layout and input
framework - uses the GUI system, and provides the needed functionality to create applications.


# Building

This project uses Conan + Premake setup.
Make sure they're installed and run `python ./build-debug.py` (or `build-release.py`).

The build scripts are configured to use Visual Studio generator on Windows and [Visual Studio Code](https://github.com/Enhex/premake-vscode) on Linux.
You can change these generators in `build.py`.


# Documentation

## layout

the layout system is responsible for positioning and sizing of GUI elements. 

layout element

a layout elements is a rectangle. it can have child elements that will be positioned relative to it, and laid out according to it's layout object. 

layout object

layout objects control how child elements are arranged inside their parent element.

box - lists children one after another horizontally/vertically, and size them evenly in the orientation direction by evenly split the parent size between them.


## input

TODO
events
focus - focused & hovered elements
context


## style

style element automatically searches for style properties for it's name. 
