#import "library/assert.w"

main :: () #entry {
    a: i32 = 1;
    p: &i32 = &a;

    b: i32 = *p;
    assert(b == 1);

    print("Ok\n");
}
