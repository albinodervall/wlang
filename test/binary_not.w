#import "library/assert.w"

main :: () #entry {
    a: u8 = 0;
    assert(~a == 0xff);

    a: u16 = 0;
    assert(~a == 0xffff);

    a: u32 = 0;
    assert(~a == 0xffffffff);

    a: u64 = 0;
    assert(~a == 0xffffffffffffffff);

    a: i8 = 0;
    assert(~a == 0xff);

    a: i16 = 0;
    assert(~a == 0xffff);

    a: i32 = 0;
    assert(~a == 0xffffffff);

    a: i64 = 0;
    assert(~a == 0xffffffffffffffff);

    print("Ok\n");
}
