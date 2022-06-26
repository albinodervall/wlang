#import "library/assert.w"

main :: () #entry {
    x: string = "hej";

    assert(x.number_of_bytes == 3);

    print("Ok\n");
}
