#import "library/print.w"

main :: () #entry {
    a: [4096]u8;
    a[4095] = 1;

    print("Ok\n");
}
