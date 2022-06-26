#import "library/assert.w"

main :: () #entry {
    for index: i32, value: i32 in -4..0 {
        assert(index - 4 == value);
    }

    print("Ok\n");
}
