#import "library/assert.w"

f :: (x: u64, y: u64, z: u64, w: u64, i: u64, j: u64) {
    assert(x == 1);
    assert(y == 2);
    assert(z == 3);
    assert(w == 4);
    assert(i == 5);
    assert(j == 6);
}

main :: () #entry {
    f(1, 2, 3, 4, 5, 6);

    print("Ok\n");
}
