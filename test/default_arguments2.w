#import "library/assert.w"

f :: (a: i32, b: i32 = 2) {
    assert(a == 1);
    assert(b == 2);
}

main :: () #entry {
    f(1);
    f(1, 2);

    print("Ok\n");
}
