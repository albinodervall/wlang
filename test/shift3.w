#import "library/assert.w"

main :: () #entry {
    a: u64 = 0xFFFFFFFFFFFFFFFF;
    print_u64(a);

    b: u64 = a >> 1;
    print_u64(b);

    assert(b == 0x7FFFFFFFFFFFFFFF);

    print("Ok\n");
}
