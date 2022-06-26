#import "library/assert.w"

A :: {
    x: i32;
    y: i32;
};

main :: () #entry {
    a: A;
    a.x = 5;
    a.y = 2;

    assert(a.x == 5);
    assert(a.y == 2);

    b: A = a;

    assert(b.x == 5);
    assert(b.y == 2);

    print("Ok\n");
}
