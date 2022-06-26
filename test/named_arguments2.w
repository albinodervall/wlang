#import "library/assert.w"

f :: (x: i32, y: i32 = 2): i32 {
    return x * y;
}

main :: () #entry {
    assert(f(1) == 2); // ok

    // TODO

    assert(f(1, y = 2) == 2); // ok
    assert(f(y = 1, x = 2) == 2); // ok

    // assert(f(y = 1)); // not ok
    // assert(f(x = 1, 2)); // not ok

    print("Ok\n");
}
