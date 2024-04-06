# Description
This project provides a simple terminal based interface for creating, modifying and rendering Bézier curves.
The interface is created using [FTXUI](https://github.com/ArthurSonzogni/FTXUI).

# Installation
Clone this repository and run `./build.sh && build/binaries/main`. 
While Windows is theoretically supported, it is not officially tested and designed to work on Windows.

# Usage
When you run the binary, this UI will show up:
![](/assets/img.png)

### Object list
On the left you see a list of all points and curves. In the middle you have the main canvas, where all points and curves are displayed.

### Canvas
You can drag the canvas with your mouse while holding the right mouse button and zoom in and out with the scroll wheel.
To add a point you can click the left mouse button. This will create a point at the current cursor position. You can also drag around a point by holding the left mouse button.

### Controls
On the right hand side there is a list of controls. In the top section you can enable and disable the rendering of certain objects.
You can also switch between placing points or curves. When in curve mode, you can click points to add them to the current curve. Once you place somewhere else in the canvas, the curve is created.

### Saving
You can quit the program by hitting `q` or `ESC`. When you quit, all points and curves are saved to the file `save`. Data from this file is also automatically loaded when starting again.
If you want to change the name or path of this save file, you can pass the path as first program argument: `build/binaries/main myTestFile`