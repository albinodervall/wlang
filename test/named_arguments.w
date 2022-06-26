#import "library/assert.w"

f :: (x: i32): i32 {
    return x;
}

main :: () #entry {
    assert(f(2) == 2);
    assert(f(x = 2) == 2);

    print("Ok\n");
}
