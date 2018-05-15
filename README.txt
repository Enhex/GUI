right now `src` contains sub-folders for independent components that should be standalone libraries later on.
event - event system
layout - layout system
input - input system, builds on top of the event system
gui - core GUI system, integration of layout and input
framework - uses the GUI system, and provides the needed functionality to create applications.
