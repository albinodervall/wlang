#import "library/assert.w"

main :: () #entry {
    y: i32 = 2;
    assert(y == 2);

    x: i32;
    x = 2;
    assert(x == 2);

    print("Ok\n");
}
