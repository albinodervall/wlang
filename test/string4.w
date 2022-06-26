#import "library/assert.w"

malloc :: (size: u64): &void #foreign;

print_string :: (argument: string) {
    assert(argument.number_of_bytes == 14);

    s: string;
    s.pointer = malloc(argument.number_of_bytes + 1) as &u8;
    s.number_of_bytes = argument.number_of_bytes + 1;

    for index in 0..(argument.number_of_bytes - 1) {
        s[index] = argument[index];
    }

    s[s.number_of_bytes - 1] = 0;

    printf(s.pointer);
}

main :: () #entry {
    print_string("Hello, world!\n");

    printf("Ok\n\0".pointer);
}
