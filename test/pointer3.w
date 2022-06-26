#import "library/assert.w"

A :: {
    x: i32;
}

main :: () #entry {
    a: A;
    a.x = 5;

    p: &A = &a;

    b: A = *p;

    assert(a.x == 5);
    assert(b.x == 5);

    b.x = 10;

    assert(a.x == 5);
    assert(b.x == 10);

    print("Ok\n");
}
