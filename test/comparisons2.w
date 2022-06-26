#import "library/assert.w"

main :: () #entry {
    a: u64 = 1;
    b: u64 = 2;

    assert(a < b);
    assert(b > a);
    assert(a <= b);
    assert(b >= a);
    assert(a != b);
    assert(!(a == b));

    x: u64 = 10;
    y: u64 = 0;

    assert(x > y);
    assert(y < x);

    i: u64 = 10;
    j: u64 = 10;

    assert(i >= j);
    assert(j <= i);
    assert(i == j);

    print("Ok\n");
}
