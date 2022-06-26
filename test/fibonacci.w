#import "library/assert.w"

fibonacci :: (N: i32): i32 {
    a: i32 = 0;
    b: i32 = 1;

    for _ in 1..N {
        c: i32 = b;
        b = a + b;
        a = c;

        // TODO
        // a, b = a, a + b
    }

    return a;
}

fibonacci2 :: (n: i32): i32 {
    if n == 0 {
        return 0;
    } else if n == 1 {
        return 1;
    }

    return fibonacci2(n - 1) + fibonacci2(n - 2);
}

main :: () #entry {
    assert(fibonacci(8) == 21);
    assert(fibonacci2(8) == 21);

    print("Ok\n");
}
