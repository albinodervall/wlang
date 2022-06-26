#import "library/assert.w"

main :: () #entry {
    digits: []u64 = [ 0, 1, 2, 3, 4, ];

    for index in 0..(digits.number_of_elements - 1) {
        assert(digits[index] == index);
    }

    for index in 0..(digits.number_of_elements - 1) {
        assert(digits[index] == index);
    }

    print("Ok\n");
}
