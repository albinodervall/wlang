#import "library/assert.w"

main :: () #entry {
    s: string = "Hello, World!";

    for i, c in s {
        print_ascii(c);
        print("\n");
        assert(c == s[i]);
    }

    print("Ok\n");
}
