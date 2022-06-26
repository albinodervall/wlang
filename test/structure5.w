#import "library/assert.w"

A :: {
    x: []i32;
};

main :: () #entry {
    y: []i32 = [ 1, 2, 3, ];

    a: A;
    a.x = y;

    a.x[0];

    assert(a.x[0] == 1);
    assert(a.x[1] == 2);
    assert(a.x[2] == 3);

    y[0] = 0;

    assert(a.x[0] == 0);
    assert(a.x[1] == 2);
    assert(a.x[2] == 3);

    print("Ok\n");
}
