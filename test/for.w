#import "library/assert.w"

main :: () #entry {
    j: i32 = 5;
    for i: i32 in 5..12 {
        assert(i == j);

        j = j + 1;
    }

    print("Ok\n");
}
