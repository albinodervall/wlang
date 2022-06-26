#import "library/assert.w"

main :: () #entry {
    a: [4]i32;
    assert(a[0] == 0);
    assert(a[1] == 0);
    assert(a[2] == 0);
    assert(a[3] == 0);

    print("Ok\n");
}
