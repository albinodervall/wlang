#import "library/assert.w"

main :: () #entry {
    π: i32 = 314;

    assert(π == 314);

    print("Ok\n");
}
