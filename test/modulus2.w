#import "library/assert.w"

my_modulus :: (a: u64, b: u64): u64 {
    return a - (b * (a / b));
}

main :: () #entry {
    assert(my_modulus(0xF, 10) == 5);
    assert(my_modulus(0xE, 10) == 4);
    assert(my_modulus(0xD, 10) == 3);
    assert(my_modulus(0xC, 10) == 2);
    assert(my_modulus(0xB, 10) == 1);
    assert(my_modulus(0xA, 10) == 0);
    assert(my_modulus(0x9, 10) == 9);
    assert(my_modulus(0x8, 10) == 8);
    assert(my_modulus(0x7, 10) == 7);
    assert(my_modulus(0x6, 10) == 6);
    assert(my_modulus(0x5, 10) == 5);
    assert(my_modulus(0x4, 10) == 4);
    assert(my_modulus(0x3, 10) == 3);
    assert(my_modulus(0x2, 10) == 2);
    assert(my_modulus(0x1, 10) == 1);
    assert(my_modulus(0x0, 10) == 0);

    print("--\n");

    assert(my_modulus(0x7FFFFFFF, 10) == 7);
    assert(my_modulus(0x7FFFFFFE, 10) == 6);
    assert(my_modulus(0x7FFFFFFD, 10) == 5);
    assert(my_modulus(0x7FFFFFFC, 10) == 4);
    assert(my_modulus(0x7FFFFFFB, 10) == 3);
    assert(my_modulus(0x7FFFFFFA, 10) == 2);
    assert(my_modulus(0x7FFFFFF9, 10) == 1);
    assert(my_modulus(0x7FFFFFF8, 10) == 0);
    assert(my_modulus(0x7FFFFFF7, 10) == 9);
    assert(my_modulus(0x7FFFFFF6, 10) == 8);
    assert(my_modulus(0x7FFFFFF5, 10) == 7);
    assert(my_modulus(0x7FFFFFF4, 10) == 6);
    assert(my_modulus(0x7FFFFFF3, 10) == 5);
    assert(my_modulus(0x7FFFFFF2, 10) == 4);
    assert(my_modulus(0x7FFFFFF1, 10) == 3);
    assert(my_modulus(0x7FFFFFF0, 10) == 2);

    print("--\n");

    assert(my_modulus(0xFFFFFFFF, 10) == 5);
    assert(my_modulus(0xFFFFFFFE, 10) == 4);
    assert(my_modulus(0xFFFFFFFD, 10) == 3);
    assert(my_modulus(0xFFFFFFFC, 10) == 2);
    assert(my_modulus(0xFFFFFFFB, 10) == 1);
    assert(my_modulus(0xFFFFFFFA, 10) == 0);
    assert(my_modulus(0xFFFFFFF9, 10) == 9);
    assert(my_modulus(0xFFFFFFF8, 10) == 8);
    assert(my_modulus(0xFFFFFFF7, 10) == 7);
    assert(my_modulus(0xFFFFFFF6, 10) == 6);
    assert(my_modulus(0xFFFFFFF5, 10) == 5);
    assert(my_modulus(0xFFFFFFF4, 10) == 4);
    assert(my_modulus(0xFFFFFFF3, 10) == 3);
    assert(my_modulus(0xFFFFFFF2, 10) == 2);
    assert(my_modulus(0xFFFFFFF1, 10) == 1);
    assert(my_modulus(0xFFFFFFF0, 10) == 0);

    print("--\n");

    assert(my_modulus(0xFFFFFFFFFFFFFFF, 10) == 5);
    assert(my_modulus(0xFFFFFFFFFFFFFFE, 10) == 4);
    assert(my_modulus(0xFFFFFFFFFFFFFFD, 10) == 3);
    assert(my_modulus(0xFFFFFFFFFFFFFFC, 10) == 2);
    assert(my_modulus(0xFFFFFFFFFFFFFFB, 10) == 1);
    assert(my_modulus(0xFFFFFFFFFFFFFFA, 10) == 0);
    assert(my_modulus(0xFFFFFFFFFFFFFF9, 10) == 9);
    assert(my_modulus(0xFFFFFFFFFFFFFF8, 10) == 8);
    assert(my_modulus(0xFFFFFFFFFFFFFF7, 10) == 7);
    assert(my_modulus(0xFFFFFFFFFFFFFF6, 10) == 6);
    assert(my_modulus(0xFFFFFFFFFFFFFF5, 10) == 5);
    assert(my_modulus(0xFFFFFFFFFFFFFF4, 10) == 4);
    assert(my_modulus(0xFFFFFFFFFFFFFF3, 10) == 3);
    assert(my_modulus(0xFFFFFFFFFFFFFF2, 10) == 2);
    assert(my_modulus(0xFFFFFFFFFFFFFF1, 10) == 1);
    assert(my_modulus(0xFFFFFFFFFFFFFF0, 10) == 0);

    print("--\n");

    assert(my_modulus(0x7FFFFFFFFFFFFFFF, 10) == 7);
    assert(my_modulus(0x7FFFFFFFFFFFFFFE, 10) == 6);
    assert(my_modulus(0x7FFFFFFFFFFFFFFD, 10) == 5);
    assert(my_modulus(0x7FFFFFFFFFFFFFFC, 10) == 4);
    assert(my_modulus(0x7FFFFFFFFFFFFFFB, 10) == 3);
    assert(my_modulus(0x7FFFFFFFFFFFFFFA, 10) == 2);
    assert(my_modulus(0x7FFFFFFFFFFFFFF9, 10) == 1);
    assert(my_modulus(0x7FFFFFFFFFFFFFF8, 10) == 0);
    assert(my_modulus(0x7FFFFFFFFFFFFFF7, 10) == 9);
    assert(my_modulus(0x7FFFFFFFFFFFFFF6, 10) == 8);
    assert(my_modulus(0x7FFFFFFFFFFFFFF5, 10) == 7);
    assert(my_modulus(0x7FFFFFFFFFFFFFF4, 10) == 6);
    assert(my_modulus(0x7FFFFFFFFFFFFFF3, 10) == 5);
    assert(my_modulus(0x7FFFFFFFFFFFFFF2, 10) == 4);
    assert(my_modulus(0x7FFFFFFFFFFFFFF1, 10) == 3);
    assert(my_modulus(0x7FFFFFFFFFFFFFF0, 10) == 2);

    print("--\n");

    assert(my_modulus(0xFFFFFFFFFFFFFFFF, 10) == 5);
    assert(my_modulus(0xFFFFFFFFFFFFFFFE, 10) == 4);
    assert(my_modulus(0xFFFFFFFFFFFFFFFD, 10) == 3);
    assert(my_modulus(0xFFFFFFFFFFFFFFFC, 10) == 2);
    assert(my_modulus(0xFFFFFFFFFFFFFFFB, 10) == 1);
    assert(my_modulus(0xFFFFFFFFFFFFFFFA, 10) == 0);
    assert(my_modulus(0xFFFFFFFFFFFFFFF9, 10) == 9);
    assert(my_modulus(0xFFFFFFFFFFFFFFF8, 10) == 8);
    assert(my_modulus(0xFFFFFFFFFFFFFFF7, 10) == 7);
    assert(my_modulus(0xFFFFFFFFFFFFFFF6, 10) == 6);
    assert(my_modulus(0xFFFFFFFFFFFFFFF5, 10) == 5);
    assert(my_modulus(0xFFFFFFFFFFFFFFF4, 10) == 4);
    assert(my_modulus(0xFFFFFFFFFFFFFFF3, 10) == 3);
    assert(my_modulus(0xFFFFFFFFFFFFFFF2, 10) == 2);
    assert(my_modulus(0xFFFFFFFFFFFFFFF1, 10) == 1);
    assert(my_modulus(0xFFFFFFFFFFFFFFF0, 10) == 0);

    print("Ok\n");
}
