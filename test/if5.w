#import "library/assert.w"

f :: (): i32 {
    x: i32 = 1;
    if x > 1 {
        return 1;
    } else {
        return 0;
    }
}

main :: () #entry {
    assert(f() == 0);

    print("Ok\n");
}
