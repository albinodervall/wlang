#import "library/print.w"

f :: () {
}

main :: () #entry {
    g: &() = f;
    g();

    print("Ok\n");
}
