printf :: (format: &u8): i32 #foreign;
malloc :: (size: u64): &void #foreign;

main :: () #entry {
    x: string;
    // TODO: The as &u8 should not be needed but the compiler has very special handling
    //       for assigning to fields for now that rely on the types being the exact same...
    x.pointer = malloc(7) as &u8;
    x.number_of_bytes = 6;

    x[0] = 0x48; // H
    x[1] = 0x65; // e
    x[2] = 0x6c; // l
    x[3] = 0x6c; // l
    x[4] = 0x6f; // o
    x[5] = 0x0a; // \n
    x[6] = 0;

    printf(x.pointer);

    printf("Ok\n\0".pointer);
}
