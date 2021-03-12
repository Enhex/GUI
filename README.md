# Project structure

right now `src` contains sub-folders for independent components that should be standalone libraries later on.  
`event` - event system  
`layout` - layout system  
`input` - input system, builds on top of the event system  
`gui` - core GUI system, integration of layout and input  
`framework` - uses the GUI system, and provides the needed functionality to create applications.


# Building

This project uses [Conan](https://conan.io/) + [Premake](https://github.com/premake/premake-core) setup.
Make sure they're installed and run `python ./build.py` (or `build.py -r` for release).

The build scripts are configured to use Visual Studio generator on Windows and [Visual Studio Code](https://github.com/Enhex/premake-vscode) on Linux.
You can change these generators in `build.py`.


# Documentation

## **layout**

the layout system is responsible for positioning and sizing of GUI elements.

### **layout element**

a layout elements is a rectangle. it can have child elements that will be positioned relative to it, and laid out according to it's layout object.

a layout element can expand in each axis to fill up space inside its parent.

GUI element inherits from layout element.

### **layout object**

layout objects control how child elements are arranged inside their parent element.

**box** - lists children one after another horizontally/vertically, and for expanding children size them evenly in the orientation direction by evenly split the parent size between them.

**shrink** - shrinks to fit around the children, without positioning them.

**forward** - don't perform any layouting and let the children perform layouting.

**null** - an element can also have its layout object set to `nullptr`, in which case it won't perform any layouting and prevent its children from performing layouting.

### **layout file**
layouts can be described in a [Deco format](https://github.com/Enhex/Deco) file which an element can load to instanciate the file's elements as its children, using [application::load_layout()](src/framework/application.h).


## **input**
elements can subscribe a callback to an event using `input_manager::subscribe()`, and the callback will be called when the event happens and the element is focused.  
for example see [button::button()](src/gui/button.cpp).

`input_manager::subscribe_global()` can be used to subscribe to events without an element.

### **events**
there are events for key & mouse inputs, element focus & hover, and frame start.

a list of events can be found in [events.h](src/input/events.h).

new events can be added by inheriting from `input::event::base`.


## **style**
each element class defines its style properties, for example colors.

a style is a map from property name to value.  
each style has a name.

element has a style name.  
element uses its element name by default.  
for example `button` will use `"button"` as its style by default.

styles are stored in [style::manager::styles](src/style/style.h).

### **style file**
styles can be described in a [Deco format](https://github.com/Enhex/Deco) file which can be loaded using [application::load_style_file()](src/framework/application.h).