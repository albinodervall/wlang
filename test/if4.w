#import "library/print.w"

add :: (a: i32, b: i32): i32 {
    return a + b;
}

main :: () #entry {
    a: i32 = 4;
    b: i32 = 2;
    c: i32 = add(a, b);

    if c == 6 {
        print("Correct!\n");
    } else {
        print("Incorrect!\n");
    }

    print("Ok\n");
}
