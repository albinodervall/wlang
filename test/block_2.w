#import "library/assert.w"

main :: () #entry {
    x: i32 = {
        2 as u32;
        3
    };

    y: i32 = x + 2;

    assert(y == 5);

    print("Ok\n");
}
