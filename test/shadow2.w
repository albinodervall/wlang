#import "library/assert.w"

main :: () #entry {
    x: i32 = 2;

    {
        x: i32 = 4;
        assert(x == 4);
    }

    assert(x == 2);

    print("Ok\n");
}
