#import "library/assert.w"

main :: () #entry {
    for value: i32 in 0..4 {
        print_i32(value);
    }

    print("Ok\n");
}
