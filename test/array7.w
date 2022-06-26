#import "library/assert.w"

main :: () #entry {
    x: [3]i32;
    x[0] = 72;
    x[1] = 100;
    x[2] = 108;

    assert(x[0] == 72);
    assert(x[1] == 100);
    assert(x[2] == 108);

    print("Ok\n");
}
