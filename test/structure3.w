#import "library/assert.w"

A :: {
    x: i32;
};

B :: {
    a: A;
}

main :: () #entry {
    b: B;
    b.a.x = 2;

    // y: i32 = b.a.x;
    assert(b.a.x == 2);

    print("Ok\n");
}
