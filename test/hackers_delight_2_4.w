#import "library/assert.w"

abs_1 :: (x: i32): i32 {
    y: i32 = x >> 31;
    return (x ^ y) - y;
}

abs_2 :: (x: i32): i32 {
    y: i32 = x >> 31;
    return (x + y) ^ y;
}

abs_3 :: (x: i32): i32 {
    y: i32 = x >> 31;
    return x - ((x + x) & y);
}

nabs_1 :: (x: i32): i32 {
    y: i32 = x >> 31;
    return y - (x ^ y);
}

nabs_2 :: (x: i32): i32 {
    y: i32 = x >> 31;
    return (y - x) ^ y;
}

nabs_3 :: (x: i32): i32 {
    y: i32 = x >> 31;
    return ((x * x) & y) - x;
}

abs_4 :: (x: i32): i32 {
    return ((x >> 30) | 1) * x;
}

main :: () #entry {
    a: i32 = abs_1(-1234);
    print_i32(a);
    assert(a == 1234);

    b: i32 = abs_2(-1234);
    print_i32(b);
    assert(b == 1234);

    c: i32 = abs_3(-1234);
    print_i32(c);
    assert(c == 1234);

    d: i32 = nabs_1(1234);
    print_i32(d);
    assert(d == -1234);

    e: i32 = nabs_2(1234);
    print_i32(e);
    assert(e == -1234);

    f: i32 = nabs_3(1234);
    print_i32(f);
    assert(f == -1234);

    g: i32 = abs_4(-1234);
    print_i32(g);
    assert(g == 1234);

    print("Ok\n");
}
