#import "library/assert.w"

main :: () #entry {
    x: i32 = 1;
    y: i32 = 2;

    assert((x + y) == 3);
    assert((x - y) == -1);
    assert((x * y) == 2);
    assert((x / y) == 0);
    assert((x & y) == 0);
    assert((x | y) == 3);
    assert((x ^ y) == 3);
    assert((x << y) == 4);
    assert((x >> y) == 0);
    assert((x < y) == true);
    assert((x > y) == false);
    assert((x <= y) == true);
    assert((x >= y) == false);
    assert((x == y) == false);
    assert((x != y) == true);

    print("Ok\n");
}
