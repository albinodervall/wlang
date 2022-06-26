#import "library/assert.w"

main :: () #entry {
    a: i32 = 1;
    b: i32 = 2;

    assert(a < b);
    assert(b > a);
    assert(a <= b);
    assert(b >= a);
    assert(a != b);
    assert(!(a == b));

    x: i32 = 10;
    y: i32 = 0;

    assert(x > y);
    assert(y < x);

    i: i32 = 10;
    j: i32 = 10;

    assert(i >= j);
    assert(j <= i);
    assert(i == j);

    print("Ok\n");
}
