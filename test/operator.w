#import "library/assert.w"

// TODO

Pair :: {
    x: i32;
    y: i32;
}

// add #operator(+) :: (a: Pair, b: Pair): Pair {
//     return Pair(a.x + b.x, a.y + b.y);
// }

main :: () #entry {
    a: Pair; // = Pair(2, 3);
    a.x = 2;
    a.y = 3;
    b: Pair; // = Pair(5, 6);
    b.x = 5;
    b.y = 6;
    // c: Pair = a + b;

    assert(a.x == 2);
    assert(a.y == 3);
    assert(b.x == 5);
    assert(b.y == 6);
    // assert(c.x = 7);
    // assert(c.y = 9);

    print("Ok\n");
}
