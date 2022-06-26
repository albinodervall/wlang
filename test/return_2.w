#import "library/assert.w"

f :: () {
    // TODO
    // if 2 > 3 {
    //     return;
    // }

    x: i32 = 4;
    x: i32 = 5;
    x: i32 = 6;
}

main :: () #entry {
    f();

    print("Ok\n");
}
