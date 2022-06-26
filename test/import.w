#import "library/assert.w"
#import "library/math.w"

main :: () #entry {
    assert(add(1, 2) == 3);

    print("Ok\n");
}
