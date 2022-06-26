#import "library/assert.w"

main :: () #entry {
    a: boolean = true;
    b: boolean = !a;
    assert(b == false);

    print("Ok\n");
}
