#import "library/assert.w"

main :: () #entry {
    y: []i32;
    x: [1]i32;
    y = x;

    assert(y.number_of_elements == 1);

    print("Ok\n");
}
