#import "library/assert.w"

main :: () #entry {
    count: i32 = 4;

    assert(count == 4);

    print("Ok\n");
}
