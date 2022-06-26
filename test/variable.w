#import "library/assert.w"

main :: () #entry {
    x: i32 = 2 + 2;
    y: i32 = x + 2;

    assert(x == 4);
    assert(y == 6);

    print("Ok\n");
}
