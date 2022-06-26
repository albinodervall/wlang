#import "library/assert.w"

main :: () #entry {
    a: u32 = 0;
    assert(~a == 0xffffffff);

    b: u32 = 1;
    assert(~b == 0xfffffffe);

    c: u32 = 0b10101010101010101010101010101010;
    assert(~c == 0b01010101010101010101010101010101);

    print("Ok\n");
}
