#import "library/assert.w"

main :: () #entry {
    a: [3]i8 = [ 0, 1, 2, ];
    assert(a[2] == 2);

    b: [3]i16 = [ 3, 4, 5, ];
    assert(b[2] == 5);

    c: [3]i32 = [ 6, 7, 8, ];
    assert(c[2] == 8);

    d: [3]i64 = [ 9, 10, 11, ];
    assert(d[2] == 11);

    e: [3]u8 = [ 12, 13, 14, ];
    assert(e[2] == 14);

    f: [3]u16 = [ 15, 16, 17, ];
    assert(f[2] == 17);

    g: [3]u32 = [ 18, 19, 20, ];
    assert(g[2] == 20);

    h: [3]u64 = [ 21, 22, 23, ];
    assert(h[2] == 23);

    print("Ok\n");
}
