#import "library/assert.w"

main :: () #entry {
    a: []i8 = [ 0, 1, 2, ];
    for index, value in a {
        assert(value as u64 == index + 0);
    }

    b: []i16 = [ 3, 4, 5, ];
    for index, value in b {
        assert(value as u64 == index + 3);
    }

    c: []i32 = [ 6, 7, 8, ];
    for index, value in c {
        assert(value as u64 == index + 6);
    }

    d: []i64 = [ 9, 10, 11, ];
    for index, value in d {
        assert(value as u64 == index + 9);
    }

    e: []u8 = [ 12, 13, 14, ];
    for index, value in e {
        assert(value as u64 == index + 12);
    }

    f: []u16 = [ 15, 16, 17, ];
    for index, value in f {
        assert(value as u64 == index + 15);
    }

    g: []u32 = [ 18, 19, 20, ];
    for index, value in g {
        assert(value as u64 == index + 18);
    }

    h: []u64 = [ 21, 22, 23, ];
    for index, value in h {
        assert(value as u64 == index + 21);
    }

    print("Ok\n");
}
