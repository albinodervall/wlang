#import "library/print.w"

main :: () #entry {
    x: i32 = 2;
    if x == 1 {
        print("Incorrect\n");
    } else if x == 2 {
        print("Correct\n");
    } else {
        print("Incorrect\n");
    }

    print("Ok\n");
}
