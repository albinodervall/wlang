#import "library/assert.w"

f :: (x: i32): i32 {
    return x + x;
}

g :: (x: i32): i32 {
    return x * x;
}

main :: () #entry {
    assert(f(2) == 4);
    assert(g(2) == 4);

    print("Ok\n");
}
