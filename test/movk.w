#import "library/assert.w"

movk :: () #entry {
    x: i32 = 2147483647;
    assert(x == 2147483647);

    print("Ok\n");
}
