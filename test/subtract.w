#import "library/assert.w"

subtract :: (a: i32, b: i32): i32 {
    return a - b;
}

main :: () #entry {
    a: i32 = 4;
    b: i32 = 2;
    c: i32 = subtract(a, b);

    assert(c == 2);

    print("Ok\n");
}
