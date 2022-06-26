#import "library/assert.w"

main :: () #entry {
    digits: []u64 = [ 0, 1, 2, 3, 4, ];

    for index, digit in digits {
        print_u64(index);
        print_u64(digit);

        assert(digit == index);
    }

    print("Ok\n");
}
