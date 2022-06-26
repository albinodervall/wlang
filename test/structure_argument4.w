#import "library/assert.w"

A :: {
    x: u64;
    y: u64;
    z: u64;
};

f :: (a: A) {
    assert(a.x == 5);
    assert(a.y == 6);
    assert(a.z == 7);
}

main :: () #entry {
    a: A;
    a.x = 5;
    a.y = 6;
    a.z = 7;

    f(a);

    print("Ok\n");
}
