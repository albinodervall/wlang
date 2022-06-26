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

    b: B;
    b.a = a;

    a.x = 7;

    assert(a.x == 7);
    assert(b.a.x == 5);

    print("Ok\n");
}
