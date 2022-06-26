printf :: (format: &u8): i32 #foreign;

print :: () {
    printf("Ok\n\0".pointer);
}

main :: () #entry {
    print2: &() = print;
    print2();
}
