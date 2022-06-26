#import "library/assert.w"

A :: {
    x: i32;
    y: u64;
};

main :: () #entry {
    a: A;
    a.x = -5;
    a.y = 2;

    assert(a.x == -5);
    assert(a.y == 2);

    print("Ok\n");
}
