#linker "-framework Metal"

NSString :: #opaque;
NSArray :: #opaque;

objc_selector :: #opaque;

sel_registerName :: (str: &u8): &objc_selector #foreign;
objc_getClass :: (str: &u8): &void #foreign;

MTLDevice :: #opaque;
MTLCommandQueue :: #opaque;

MTLTexture :: #opaque;

MTLCopyAllDevices :: (): &NSArray #foreign;
MTLCreateSystemDefaultDevice :: (): &MTLDevice #foreign;

MTLDevice_newCommandQueue :: (device: &MTLDevice): &MTLCommandQueue {
    objc_msgSend :: (device: &MTLDevice, selector: &objc_selector): &MTLCommandQueue #foreign;
    return objc_msgSend(device, sel_registerName("newCommandQueue\0".pointer));
}

MTLCommandBuffer :: #opaque;

MTLCommandQueue_commandBuffer :: (commandQueue: &MTLCommandQueue): &MTLCommandBuffer {
    objc_msgSend :: (commandQueue: &MTLCommandQueue, selector: &objc_selector): &MTLCommandBuffer #foreign;
    return objc_msgSend(commandQueue, sel_registerName("commandBuffer\0".pointer));
}

MTLRenderPassDescriptor :: #opaque;

MTLRenderPassDescriptor_renderPassDescriptor :: (): &MTLRenderPassDescriptor {
    objc_msgSend :: (class: &void, selector: &objc_selector): &MTLRenderPassDescriptor #foreign;
    return objc_msgSend(objc_getClass("MTLRenderPassDescriptor\0".pointer), sel_registerName("renderPassDescriptor\0".pointer));
}

MTLRenderPassColorAttachmentDescriptor :: #opaque;

MTLRenderPassColorAttachmentDescriptorArray :: #opaque;

MTLRenderPassDescriptor_colorAttachment :: (pass: &MTLRenderPassDescriptor, index: u64): &MTLRenderPassColorAttachmentDescriptor {
    colorAttachments: &MTLRenderPassColorAttachmentDescriptorArray;

    {
        objc_msgSend :: (pass: &MTLRenderPassDescriptor, selector: &objc_selector): &MTLRenderPassColorAttachmentDescriptorArray #foreign;
        colorAttachments =  objc_msgSend(pass, sel_registerName("colorAttachments\0".pointer));
    }

    {
        objc_msgSend :: (pointer: &MTLRenderPassColorAttachmentDescriptorArray, selector: &objc_selector, index: u64): &MTLRenderPassColorAttachmentDescriptor #foreign;
        return objc_msgSend(colorAttachments, sel_registerName("objectAtIndexedSubscript:\0".pointer), index);
    }
}

// MTLRenderPassColorAttachmentDescriptor_setClearColor :: (descriptor: &MTLRenderPassColorAttachmentDescriptor, ) {
//     objc_msgSend :: (descriptor: &MTLRenderPassColorAttachmentDescriptor, selector: &objc_selector, ???) #foreign;
//     objc_msgSend(layer, sel_registerName("setClearColor_\0".pointer), device);
// }

MTLRenderPassColorAttachmentDescriptor_setLoadAction :: (descriptor: &MTLRenderPassColorAttachmentDescriptor, loadAction: u64) {
    objc_msgSend :: (descriptor: &MTLRenderPassColorAttachmentDescriptor, selector: &objc_selector, loadAction: u64) #foreign;
    objc_msgSend(descriptor, sel_registerName("setLoadAction:\0".pointer), loadAction);
}

MTLRenderPassColorAttachmentDescriptor_setStoreAction :: (descriptor: &MTLRenderPassColorAttachmentDescriptor, storeAction: u64) {
    objc_msgSend :: (descriptor: &MTLRenderPassColorAttachmentDescriptor, selector: &objc_selector, storeAction: u64) #foreign;
    objc_msgSend(descriptor, sel_registerName("setStoreAction:\0".pointer), storeAction);
}

MTLRenderPassColorAttachmentDescriptor_setTexture :: (descriptor: &MTLRenderPassColorAttachmentDescriptor, texture: &MTLTexture) {
    objc_msgSend :: (descriptor: &MTLRenderPassColorAttachmentDescriptor, selector: &objc_selector, texture: &MTLTexture) #foreign;
    objc_msgSend(descriptor, sel_registerName("setTexture:\0".pointer), texture);
}

MTLRenderCommandEncoder :: #opaque;

MTLCommandBuffer_renderCommandEncoderWithDescriptor :: (commandBuffer: &MTLCommandBuffer, descriptor: &MTLRenderPassDescriptor): &MTLRenderCommandEncoder {
    objc_msgSend :: (commandBuffer: &MTLCommandBuffer, selector: &objc_selector, descriptor: &MTLRenderPassDescriptor): &MTLRenderCommandEncoder #foreign;
    return objc_msgSend(commandBuffer, sel_registerName("renderCommandEncoderWithDescriptor:\0".pointer), descriptor);
}

MTLCommandBuffer_presentDrawable :: (commandBuffer: &MTLCommandBuffer, drawable: &CAMetalDrawable) {
    objc_msgSend :: (commandBuffer: &MTLCommandBuffer, selector: &objc_selector, drawable: &CAMetalDrawable) #foreign;
    objc_msgSend(commandBuffer, sel_registerName("presentDrawable:\0".pointer), drawable);
}

MTLCommandBuffer_commit :: (commandBuffer: &MTLCommandBuffer) {
    objc_msgSend :: (commandBuffer: &MTLCommandBuffer, selector: &objc_selector) #foreign;
    objc_msgSend(commandBuffer, sel_registerName("commit\0".pointer));
}

MTLRenderCommandEncoder_endEncoding :: (encoder: &MTLRenderCommandEncoder) {
    objc_msgSend :: (encoder: &MTLRenderCommandEncoder, selector: &objc_selector) #foreign;
    objc_msgSend(encoder, sel_registerName("endEncoding\0".pointer));
}

CAMetalLayer :: #opaque;

CAMetalLayer_layer :: (): &CAMetalLayer {
    objc_msgSend :: (class: &void, selector: &objc_selector): &CAMetalLayer #foreign;
    return objc_msgSend(objc_getClass("CAMetalLayer\0".pointer), sel_registerName("layer\0".pointer));
}

CAMetalLayer_setDevice :: (layer: &CAMetalLayer, device: &MTLDevice) {
    objc_msgSend :: (layer: &CAMetalLayer, selector: &objc_selector, device: &MTLDevice) #foreign;
    objc_msgSend(layer, sel_registerName("setDevice:\0".pointer), device);
}

CAMetalDrawable :: #opaque;

CAMetalLayer_nextDrawable :: (layer: &CAMetalLayer): &CAMetalDrawable {
    objc_msgSend :: (layer: &CAMetalLayer, selector: &objc_selector): &CAMetalDrawable #foreign;
    return objc_msgSend(layer, sel_registerName("nextDrawable\0".pointer));
}

CAMetalDrawable_texture :: (drawable: &CAMetalDrawable): &MTLTexture {
    objc_msgSend :: (drawable: &CAMetalDrawable, selector: &objc_selector): &MTLTexture #foreign;
    return objc_msgSend(drawable, sel_registerName("texture\0".pointer));
}

NSWindow :: #opaque;
NSView :: #opaque;

NSWindow_contentView :: (window: &NSWindow): &NSView {
    objc_msgSend :: (window: &NSWindow, selector: &objc_selector): &NSView #foreign;
    return objc_msgSend(window, sel_registerName("contentView\0".pointer));
}

NSView_setLayer :: (view: &NSView, layer: &CAMetalLayer) {
    objc_msgSend :: (view: &NSView, selector: &objc_selector, layer: &CAMetalLayer) #foreign;
    objc_msgSend(view, sel_registerName("setLayer:\0".pointer), layer);
}

NSView_setWantsLayer :: (view: &NSView, value: i8) {
    objc_msgSend :: (view: &NSView, selector: &objc_selector, value: i8) #foreign;
    objc_msgSend(view, sel_registerName("setWantsLayer:\0".pointer), value);
}
