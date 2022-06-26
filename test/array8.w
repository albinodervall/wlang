#import "library/assert.w"

main :: () #entry {
    y: []i32 = [ 4, 8, 15, ];

    assert(y[0] == 4);
    assert(y[1] == 8);
    assert(y[2] == 15);

    print("Ok\n");
}
