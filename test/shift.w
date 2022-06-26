#import "library/assert.w"

main :: () #entry {
    a: u32 = 4;
    print_u32(a);

    b: u32 = a >> 1;
    print_u32(b);

    assert(b == 2);

    print("Ok\n");
}
