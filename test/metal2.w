#import "vendor/glfw.w"
#import "library/metal.w"
#import "library/assert.w"

mach_absolute_time :: (): u64 #foreign;

main :: () #entry {
    glfwInit();
    // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(0x00022001, 0);

    window: &GLFWwindow = glfwCreateWindow(640, 480, "Triangle\0".pointer, 0, 0);
    assert(window != 0);

    native_window: &NSWindow = glfwGetCocoaWindow(window);

    device: &MTLDevice = MTLCreateSystemDefaultDevice();
    command_queue: &MTLCommandQueue = MTLDevice_newCommandQueue(device);

    layer: &CAMetalLayer = CAMetalLayer_layer();
    CAMetalLayer_setDevice(layer, device);

    view: &NSView = NSWindow_contentView(native_window);
    NSView_setLayer(view, layer);
    // NSView_setWantsLayer(view, YES);
    NSView_setWantsLayer(view, 1);

    start: u64 = mach_absolute_time();

    run: boolean = true;
    while run {
        glfwPollEvents();

        if glfwWindowShouldClose(window) == 1 {
            run = false;
        }

        // @autoreleasepool {
            drawable: &CAMetalDrawable = CAMetalLayer_nextDrawable(layer);

            descriptor: &MTLRenderPassDescriptor = MTLRenderPassDescriptor_renderPassDescriptor();
            color_attachment: &MTLRenderPassColorAttachmentDescriptor = MTLRenderPassDescriptor_colorAttachment(descriptor, 0);
            // MTLRenderPassColorAttachmentDescriptor_setClearColor(color_attachment, color);
            MTLRenderPassColorAttachmentDescriptor_setLoadAction(color_attachment, 2); // MTLLoadActionClear
            MTLRenderPassColorAttachmentDescriptor_setStoreAction(color_attachment, 1); // MTLStoreActionStore
            MTLRenderPassColorAttachmentDescriptor_setTexture(color_attachment, CAMetalDrawable_texture(drawable));

            command_buffer: &MTLCommandBuffer = MTLCommandQueue_commandBuffer(command_queue);
            command_encoder: &MTLRenderCommandEncoder = MTLCommandBuffer_renderCommandEncoderWithDescriptor(command_buffer, descriptor);
            MTLRenderCommandEncoder_endEncoding(command_encoder);
            MTLCommandBuffer_presentDrawable(command_buffer, drawable);
            MTLCommandBuffer_commit(command_buffer);
        // }

        if mach_absolute_time() - start > 10000000 {
            run = false;
        }
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    print("Ok\n");
}
