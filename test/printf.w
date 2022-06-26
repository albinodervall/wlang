printf :: (s: &u8): i32 #foreign;

main :: () #entry {
    printf("Ok\0\n".pointer);
}
