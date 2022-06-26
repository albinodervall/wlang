#import "library/assert.w"

main :: () #entry {
    v: i32 = 10;
    if v > 0 {
        v = -10;
    }

    print_i32(v);
    assert(v == -10);

    print("Ok\n");
}
