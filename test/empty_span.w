#import "library/assert.w"

main :: () #entry {
    y: []i32 = [];
    assert(y.number_of_elements == 0);

    print("Ok\n");
}
