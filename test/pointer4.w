#import "library/assert.w"

A :: {
    a: i32;
    b: i32;
};

main :: () #entry {
    a: A;
    a.a = 2;
    a.b = 4;

    p: &A = &a;

    b: A = *p;

    assert(b.a == 2);
    assert(b.b == 4);

    print("Ok\n");
}
