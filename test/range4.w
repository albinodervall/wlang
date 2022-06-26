#import "library/assert.w"

main :: () #entry {
    for value: u64 in 0..4 {
        print_u64(value);
    }

    print("Ok\n");
}
