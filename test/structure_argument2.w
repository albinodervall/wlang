#import "library/assert.w"

A :: {
    x: i32;
    y: i32;
};

f :: (a: A) {
    assert(a.x == 5);
    assert(a.y == 6);
}

main :: () #entry {
    a: A;
    a.x = 5;
    a.y = 6;

    f(a);

    print("Ok\n");
}
