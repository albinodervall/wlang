printf :: () #foreign;

main :: () #entry {
    printf2: &(format: &u8): i32 = printf as (format: &u8): i32;
    printf2("Ok\n\0".pointer);
}
