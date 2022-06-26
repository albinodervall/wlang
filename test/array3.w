#import "library/assert.w"

main :: () #entry {
    y: []i32 = [ 4, 8, 15, ];
    y[2] = y[1];

    assert(y[0] == 4);
    assert(y[1] == 8);
    assert(y[2] == 8);

    print("Ok\n");
}
