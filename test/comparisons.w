#import "library/assert.w"

main :: () #entry {
    a: u64 = 0xFFFFFFFFFFFFFFFF;
    b: u64 = 2;

    assert(a > b);

    print("Ok\n");
}
