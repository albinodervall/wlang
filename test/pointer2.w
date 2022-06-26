#import "library/assert.w"

main :: () #entry {
    a: i32 = 1;
    b: i32 = 2;

    p: &i32 = &a;
    p = &b;

    *p = 10;

    assert(a == 1);
    assert(b == 10);

    print("Ok\n");
}
