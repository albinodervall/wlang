#import "library/assert.w"

A :: {
    x: i32;
};

main :: () #entry {
    a: A;
    a.x = 5;

    assert(a.x == 5);

    print("Ok\n");
}
