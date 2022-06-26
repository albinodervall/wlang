#import "library/assert.w"

f :: () {
    g :: () {
        print("Ok\n");
    }

    g();
}

main :: () #entry {
    f();
}
