printf :: (format: &u8): i32 #foreign;
calloc :: (num: u64, size: u64): &void #foreign;
free :: (pointer: &void) #foreign;

main :: () #entry {
    pointer: &u8 = calloc(1, 1);

    printf(pointer);

    free(pointer);

    printf("Ok\n\0".pointer);
}
