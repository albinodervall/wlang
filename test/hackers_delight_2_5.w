#import "library/assert.w"

average_floored :: (x: u32, y: u32): u32 {
    return (x & y) + ((x ^ y) >> 1);
}

average_ceiled :: (x: u32, y: u32): u32 {
    return (x | y) - ((x ^ y) >> 1);
}

main :: () #entry {
    a: u32 = average_floored(7, 4);
    print_u32(a);
    assert(a == 5);

    b: u32 = average_ceiled(7, 4);
    print_u32(b);
    assert(b == 6);

    print("Ok\n");
}
