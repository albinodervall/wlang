#import "library/assert.w"

main :: () #entry {
    digits: [5]u64 = [ 0, 1, 2, 3, 4, ];

    for index, digit in digits {
        assert(digit == index);
        print_u64(index);
        print_u64(digit);
    }

    print("Ok\n");
}
