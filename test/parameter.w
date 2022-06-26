#import "library/assert.w"

f :: (x: i32): i32 {
    return x;
}

main :: () #entry {
    x: i32 = 4;
    assert(f(x) == 4);

    print("Ok\n");
}
