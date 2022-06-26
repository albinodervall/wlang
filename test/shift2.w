#import "library/assert.w"

main :: () #entry {
    a: u32 = 0xFFFFFFFF;
    print_u32(a);

    b: u32 = a >> 1;
    print_u32(b);

    assert(b == 0x7FFFFFFF);

    print("Ok\n");
}
