#import "library/print.w"

main :: () #entry {
    y: [2]i32;
    y[1] = 0 as i32;
    x: i32 = y[1];

    print("Ok\n");
}
