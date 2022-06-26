printf :: (format: &u8): i32 #foreign;

main :: () #entry {
    printf2: &(format: &u8): i32 = printf;
    printf2("Ok\n\0".pointer);
}
