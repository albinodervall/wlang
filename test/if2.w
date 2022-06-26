#import "library/print.w"

main :: () #entry {
    x: i32 = 3;
    if x == 2 {
        print("Incorrect\n");
    } else {
        print("Correct\n");
    }

    print("Ok\n");
}
