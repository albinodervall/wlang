#import "library/assert.w"

main :: () #entry {
    y: []i32;
    y = [ 4, 8, 15, 16, 23, 42, ];

    assert(y.number_of_elements == 6);

    assert(y[0] == 4);
    assert(y[1] == 8);
    assert(y[2] == 15);
    assert(y[3] == 16);
    assert(y[4] == 23);
    assert(y[5] == 42);

    y[1] = 0;
    y[2] = y[1];

    assert(y[0] == 4);
    assert(y[1] == 0);
    assert(y[2] == 0);
    assert(y[3] == 16);
    assert(y[4] == 23);
    assert(y[5] == 42);

    print("Ok\n");
}
