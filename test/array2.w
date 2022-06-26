#import "library/assert.w"

f :: (): i32 {
    y: []i32 = [ 4,];
    return y[0];
}

main :: () #entry {
    assert(f() == 4);

    print("Ok\n");
}
