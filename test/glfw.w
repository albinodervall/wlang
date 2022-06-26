#import "vendor/glfw.w"
#import "library/assert.w"

mach_absolute_time :: (): u64 #foreign;

glfw_window_size_callback :: (window: &GLFWwindow, width: i32, height: i32) {
    print("Window size callback!\n");
}

main :: () #entry {
    glfwInit();

    window: &GLFWwindow = glfwCreateWindow(800, 600, "glfw\0".pointer, 0, 0);
    assert(window != 0);

    glfwSetWindowSizeCallback(window, glfw_window_size_callback);

    start: u64 = mach_absolute_time();

    run: boolean = true;
    while run {
        glfwPollEvents();

        if glfwWindowShouldClose(window) == 1 {
            run = false;
        }

        if mach_absolute_time() - start > 10000000 {
            run = false;
        }
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    print("Ok\n");
}
