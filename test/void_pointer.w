#import "library/print.w"

f :: (value: &void) {
    print("Ok\n");
}

main :: () #entry {
    x: i32 = 2;
    f(&f);
}
