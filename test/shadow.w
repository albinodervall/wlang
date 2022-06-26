main :: () #entry {
    printf :: (format: &u8): i32 #foreign;

    printf("Ok\n\0".pointer);
}
