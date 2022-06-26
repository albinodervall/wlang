#import "library/assert.w"

main :: () #entry {
    y: i32 = 0;
    x: i32 = if y == 0 { 2 } else if y == 1 { 3 } else { 4 };

    assert(x == 2);

    print("Ok\n");
}
