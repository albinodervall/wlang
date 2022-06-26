#import "library/print.w"

main :: () #entry {
    digits: []i32 = [ 1, 2, 3, 4, ];
    count: i32 = 4;

    print("Original\n");

    for index in 0..(count - 1) {
        print_i32(digits[index])
    }

    for index in 0..((digits.number_of_elements / 2) - 1) {
        temporary: i32 = digits[index];
        digits[index] = digits[digits.number_of_elements - 1 - index];
        digits[digits.number_of_elements - 1 - index] = temporary;
    }

    print("Reversed\n");

    for index in 0..(digits.number_of_elements - 1) {
        print_i32(digits[index])
    }

    print("Ok\n");
}
