#import "library/assert.w"

main :: () #entry {
    v: i32 = 10;
    while v > 0 {
        v = v - 1;
    }

    assert(v == 0);

    print("Ok\n");
}
