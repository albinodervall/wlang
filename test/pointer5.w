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

    assert(p.a == 2);
    assert(p.b == 4);

    print("Ok\n");
}
