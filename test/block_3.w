#import "library/assert.w"

main :: () #entry {
    y: i32 = {
        x: boolean = true;
        if x { 2 } else { 3 }
    };

    assert(y == 2);

    print("Ok\n");
}
