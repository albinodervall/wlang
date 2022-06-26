#import "library/assert.w"

print2 :: (argument: string) {
    assert(argument.number_of_bytes == 15);

    printf(argument.pointer);
}

main :: () #entry {
    print2("Hello, world!\n\0");

    printf("Ok\n\0".pointer);
}
