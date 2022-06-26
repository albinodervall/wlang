#import "library/assert.w"

main :: () #entry {
    p: &void = 0;
    assert(p == 0);

    x: i32 = 2;
    p: &i32 = &x;
    assert(p != 0);

    print("Ok\n");
}
