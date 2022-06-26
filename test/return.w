#import "library/assert.w"

f :: (): i32 {
    print_i32(1);
    {
        return 100;
    }
    print_i32(2);
}

g :: (): i32 {
    h :: (): i32 {
        // for x in 1..10 {
        //     print_i32(x);
        //     if x == 2 {
        //         return 1000;
        //     }
        // }

        return 0;
    }

    return h();
}

main :: () #entry {
    assert(f() == 100);
    assert(g() == 0);

    print("Ok\n");
}
