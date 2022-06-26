#import "library/assert.w"

main :: () #entry {
    a: i8 = -1;
    b: i16 = -1;
    c: i32 = -1;
    d: i64 = -1;

    assert(a == -1);
    assert(b == -1);
    assert(c == -1);
    assert(d == -1);

    print("Ok\n");
}
