#import "library/assert.w"

f :: (array: []i32) {
    for index, value in array {
        print_i32(value);
        assert(index as i32 == value);
    }
}

main :: () #entry {
    digits: [3]i32 = [ 0, 1, 2, ];
    f(digits);

    printf("Ok\n\0".pointer);
}
