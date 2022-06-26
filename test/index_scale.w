#import "library/assert.w"

main :: () #entry {
    a: []i8 = [ 0, 1, 2, ];
    assert(a[2] == 2);

    b: []i16 = [ 3, 4, 5, ];
    assert(b[2] == 5);

    c: []i32 = [ 6, 7, 8, ];
    assert(c[2] == 8);

    d: []i64 = [ 9, 10, 11, ];
    assert(d[2] == 11);

    e: []u8 = [ 12, 13, 14, ];
    assert(e[2] == 14);

    f: []u16 = [ 15, 16, 17, ];
    assert(f[2] == 17);

    g: []u32 = [ 18, 19, 20, ];
    assert(g[2] == 20);

    h: []u64 = [ 21, 22, 23, ];
    assert(h[2] == 23);

    print("Ok\n");
}
