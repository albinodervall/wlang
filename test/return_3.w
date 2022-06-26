#import "library/assert.w"

f :: (): i32 {
    return 3;
}

g :: () #entry {
    assert(f() == 3);

    print("Ok\n");
}
