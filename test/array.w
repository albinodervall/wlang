#import "library/assert.w"

main :: () #entry {
    y: []i32 = [ 4, 8, 15, 16, 23, 42, ];
    assert(y[0] == 4);
    assert(y[1] == 8);
    assert(y[2] == 15);

    y[1] = 0;
    y[2] = y[1];

    assert(y[0] == 4);
    assert(y[1] == 0);
    assert(y[2] == 0);

    print("Ok\n");
}
