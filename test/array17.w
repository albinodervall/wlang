#import "library/assert.w"

main :: () #entry {
    y: []i32;
    y = [ 8, ];

    assert(y.number_of_elements == 1);
    assert(y[0] == 8);

    print("Ok\n");
}
