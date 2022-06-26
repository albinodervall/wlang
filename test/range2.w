#import "library/assert.w"

main :: () #entry {
    for index: i32, value: i32 in 0..4 {
        assert(index == value);
    }

    print("Ok\n");
}
