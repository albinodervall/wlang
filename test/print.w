#import "library/print.w"

main :: () #entry {
    print_i32(-123);
    print_i32(0);
    print_i32(1);
    print_i32(2);
    print_i32(9);
    print_i32(123);
    print_i32(12345);
    print_i32(0x7FFFFFFF);

    print_u64(0);
    print_u64(1);
    print_u64(2);
    print_u64(9);
    print_u64(123);
    print_u64(12345);
    print_u64(0xFFFFFFFFFFFFFFFE);

    print("Ok\n");
}
