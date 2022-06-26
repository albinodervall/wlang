#import "library/assert.w"

f :: (a: i32 = 2) {
    assert(a == 2);
}

main :: () #entry {
    f();
    f(2);

    print("Ok\n");
}
