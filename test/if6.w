#import "library/assert.w"

f :: (): i32 {
    x: boolean = true;
    if x {
        return 1;
    } else {
        return 0;
    }
}

main :: () #entry {
    assert(f() == 1);

    print("Ok\n");
}
