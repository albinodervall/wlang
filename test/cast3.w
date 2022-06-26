#import "library/assert.w"

main :: () #entry {
    a: i32 = -5;
    b: u32 = a as u32;
    c: u64 = a as u64;

    print_i32(a);
    print_u64(b as u64);
    print_u64(c);

    print("Ok\n");
}
