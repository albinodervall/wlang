#import "library/assert.w"

A :: {
    x: i32;
};

f :: (a: A) {
    assert(a.x == 5);
}

main :: () #entry {
    a: A;
    a.x = 5;

    f(a);

    print("Ok\n");
}
