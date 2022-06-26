#import "library/assert.w"

main :: () #entry {
    a: u64 = 5;
    b: i32 = a as i32;

    print_u64(a);
    print_i32(b);

    print("Ok\n");
}
