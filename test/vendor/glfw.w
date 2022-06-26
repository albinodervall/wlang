#linker "-Ltest/vendor/glfw-3.3.8.bin.MACOS/lib-universal -lglfw3"
#linker "-framework Cocoa"
#linker "-framework IOKit"

GLFWmonitor :: #opaque;
GLFWwindow :: #opaque;

glfwInit :: (): i32 #foreign;
glfwTerminate :: () #foreign;
glfwCreateWindow :: (width: i32, height: i32, title: &u8, monitor: &GLFWmonitor, share: &GLFWwindow): &GLFWwindow #foreign;
glfwDestroyWindow :: (window: &GLFWwindow) #foreign;
glfwWindowShouldClose :: (window: &GLFWwindow): i32 #foreign;
glfwSetWindowShouldClose :: (window: &GLFWwindow, value: i32) #foreign;
glfwPollEvents :: () #foreign;

glfwWindowHint :: (hint: i32, value: i32) #foreign;

glfwSetErrorCallback :: (callback: &(error_code: i32, description: &u8)) #foreign;

glfwMakeContextCurrent :: (window: &GLFWwindow) #foreign;
glfwSwapInterval :: (interval: i32) #foreign;
glfwSwapBuffers :: (window: &GLFWwindow) #foreign;

// TODO: These actually return the previous function but we don't handle that
glfwSetWindowSizeCallback :: (window: &GLFWwindow, callback: &(window: &GLFWwindow, width: i32, height: i32)) #foreign;
glfwSetKeyCallback :: (window: &GLFWwindow, callback: &(window: &GLFWwindow, key: i32, scancode: i32, action: i32, mods: i32)) #foreign;

NSWindow :: #opaque;

glfwGetCocoaWindow :: (window: &GLFWwindow): &NSWindow #foreign;
