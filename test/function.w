#import "library/print.w"

f1 :: () {
};

// f2 :: (): i32 {
//     2
// }

f2r :: (): i32 {
    return 2;
};

f2ri :: (): i32 {
    // return 2 // Not ok!
    return 2;
}

f3 :: (a: i32) {
}

// f4 :: (a: i32): i32 {
//     a * a
// }

f4r :: (b: i32): i32 {
    return b * b;
}

// f1();
// print_i32(f2());
// print_i32(f2r());
// print_i32(f2ri());
// f3(1);
// print_i32(f4(2));
// print_i32(f4r(2));

// x: i32 = 3;
// g :: () {
//     y: i32 = x;
//     print_i32(y);
// }

// {
//     x: boolean = false;
//     g();
// }

h :: (): i32 {
    if true {
        return 1;
    } else {
        // return false;
        return 2;
    }
}

i :: (x: boolean): i32 {
    if x {
        return 1;
    }

    if !x {
        return 2;
    }

    return 0;
}

j :: (y: boolean): i32 {
    if y {
        return 1;
    } else {
        return 2;
        // return false;
    }
}

k :: (z: boolean) {
    if z {
        return;
    }

    print_i32(2);
}

// print_i32(j(true));
// print_i32(j(false));

l :: (): i32 {
    return 2;
}

llll :: () {
    ll: i32 = {
        lll: i32 = l();
        lll
    };
}

m :: (): i32 {
    mm :: (w: i32): i32 {
        return w * w;
    }

    return mm(2);
}

main :: () #entry {
    print("Ok\n");
}
