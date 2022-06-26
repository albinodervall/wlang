#import "library/assert.w"

A :: {
    x: i32;
};

f :: (a: A) {
    assert(a.x == 5);
    a.x = 10;
    assert(a.x == 10);
}

g :: (a: &A) {
    assert(a.x == 5);
    a.x = 10;
    assert(a.x == 10);
}

main :: () #entry {
    a: A;
    a.x = 5;

    f(a);

    assert(a.x == 5);

    g(&a);

    assert(a.x == 10);

    print("Ok\n");
}
