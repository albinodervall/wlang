#import "library/assert.w"

main :: () #entry {
    a: i32 = 2147483640;
    print_i32(a);

    b: i32 = a >> 2;
    print_i32(b);

    assert(b == 536870910);

    print("Ok\n");
}
