#import "library/assert.w"

f :: (digits: []u64) {
    for index, digit in digits {
        assert(digit == index);
    }
}

main :: () #entry {
    digits: []u64 = [ 0, 1, 2, 3, 4, ];
    f(digits);

    printf("Ok\n\0".pointer);
}
