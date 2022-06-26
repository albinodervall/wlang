#import "library/assert.w"

f :: () {
    y: []i32 = [ 4, 8, 15, ];
    assert(y[0] == 4);
    assert(y[1] == 8);
    assert(y[2] == 15);

    y[0] = 0;
    y[1] = 0;
    y[2] = 0;
}

main :: () #entry {
    f();
    f();

    print("Ok\n");
}
