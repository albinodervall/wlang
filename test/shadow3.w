#import "library/assert.w"

f :: (f: i32): i32 {
    return f;
}

main :: () #entry {
    assert(f(2) == 2);

    print("Ok\n");
}
