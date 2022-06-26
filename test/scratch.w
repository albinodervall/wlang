#import "library/print.w"

main :: () #entry {
    {
        a: i32 = 0;
        b: i32 = 0;
        c: i32 = a + b;
        print_i32(a);

        a: i32 = 2;
        print_i32(a);
    }

    // T :: {
    //     a: i32;
    // };

    print("Ok\n");
}
