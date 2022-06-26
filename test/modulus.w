#import "library/assert.w"

my_modulus :: (a: i32, b: i32): i32 {
    return a - (b * (a / b));
}

main :: () #entry {
    a: i32 = 3;
    b: i32 = 2;
    c: i32 = my_modulus(a, b);

    assert(c == 1);

    print("Ok\n");
}
