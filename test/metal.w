#linker "-framework Foundation"
#import "library/metal.w"
printf :: (format: &u8): i32 #foreign;

main :: () #entry {
    devices: &NSArray = MTLCopyAllDevices();

    device: &MTLDevice;
    {
        objc_msgSend :: (pointer: &NSArray, selector: &objc_selector, index: u64): &MTLDevice #foreign;
        device = objc_msgSend(devices, sel_registerName("objectAtIndex:\0".pointer), 0);
    }

    name: &NSString;
    {
        objc_msgSend :: (pointer: &MTLDevice, selector: &objc_selector): &NSString #foreign;
        name = objc_msgSend(device, sel_registerName("name\0".pointer));
    }

    s: &u8;
    {
        objc_msgSend :: (pointer: &NSString, selector: &objc_selector, encoding: u64): &u8 #foreign;
        s = objc_msgSend(name, sel_registerName("cStringUsingEncoding:\0".pointer), 4); // NSUTF8StringEncoding
    }

    printf(s);
    printf("\n\0".pointer);

    printf("Ok\n\0".pointer);
}
