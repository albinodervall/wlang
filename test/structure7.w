#import "library/assert.w"

A :: {
    x: i32;
}

B :: {
    a: A;
}

main :: () #entry {
    a: A;
    a.x = 5;

    assert(a.x == 5);

    b: B;
    b.a = a;

    assert(b.a.x == 5);

    print("Ok\n");
}
