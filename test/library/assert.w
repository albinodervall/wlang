#import "print.w"

assert :: (assertion: boolean) {
    if assertion {
        print("Correct!\n");
    } else {
        print("Incorrect!\n");
    }
}
