printf :: (format: &u8): i32 #foreign;
malloc :: (size: u64): &void #foreign;
free :: (pointer: &void) #foreign;

main :: () #entry {
    pointer: &void = malloc(128);
    free(pointer);

    printf("Ok\n\0".pointer);
}
