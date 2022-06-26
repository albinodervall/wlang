#import "library/assert.w"

main :: () #entry {
    x: i32 = {
        2 as u32;
        3
    };

    assert(x == 3);

    print("Ok\n");
}
