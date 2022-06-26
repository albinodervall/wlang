#import "library/print.w"

print2 :: (argument: string) {
    print_u64(argument.number_of_bytes);
    printf(argument.pointer);
}

main :: () #entry {
    s: string = "Hello, world!\n\0";

    print2(s);

    printf("Ok\n\0".pointer);
}
