#import "library/print.w"

main :: () #entry {
    a: i32 = 1;
    b: i32 = 2;
    c: i32 = 3;

    {
        a: i32 = 4;
        b: i32 = 5;

        {
            a: i32 = 6;
            print_i32(a); // 6
            print_i32(b); // 5
            print_i32(c); // 3
        }

        print_i32(a); // 4
        print_i32(b); // 5
        print_i32(c); // 3
    }

    print_i32(a); // 1
    print_i32(b); // 2
    print_i32(c); // 3


    x: i32 = 2;
    {
        x: i32 = x + 2;
        print_i32(x);
    }
    print_i32(x);

    print("Ok\n");
}
