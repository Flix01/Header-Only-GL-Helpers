# Header-Only-GL-Helpers
A collection of header files that can ease OpenGL programming.

| Filename             | Language | Needs OpenGL | Description                                                     |
|----------------------|----------|--------------|-----------------------------------------------------------------|
| teapot.h             |   C/C++  |      Yes     | The basic file that is used in all the demos                    |
| dynamic_resolution.h |   C/C++  |      Yes     | Implements dynamic resolution and the first shadow mapping pass |
| im_matrix_stack.h    |   C/C++  |      No      | Implements a matrix stack and some other helper methods         |

# Demos
Three demos are available: test_teapot.c, test_shadows.c and test_matrix_stack.c.
Command-lines to compile them on Linux, Windows and Emscripten are present at the top of the files.

### Dependencies (demos only)
* glut (or freeglut)
* glew (Windows only)



