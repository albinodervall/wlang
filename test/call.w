#import "library/assert.w"

f :: (): i32 {
    return 8;
}

main :: () #entry {
    assert(f() == 8);

    print("Ok\n");
}
