#import "library/print.w"

main :: () #entry {
    x: boolean = true;
    if x {
        print("Correct\n");
    } else {
        print("Incorrect\n");
    }

    y: boolean = false;
    if y {
        print("Incorrect\n");
    } else {
        print("Correct\n");
    }

    if !x {
        print("Incorrect\n");
    } else {
        print("Correct\n");
    }

    // TODO: This causes issues because what is the type of 2 and 3? We can't generated
    //       code for it. Since there was nothing to inferr the type from, one could assume
    //       this will always be able to be constant folded, in this case to false.
    // if 2 > 3 {
    //     print("Incorrect\n");
    // } else {
    //     print("Correct\n");
    // }

    z: i32 = 1;

    if z > 0 {
        print("Correct\n");
    } else {
        print("Inorrect\n");
    }

    if z == 0 {
        print("Inorrect\n");
    } else {
        print("Correct\n");
    }

    if z == 1 {
        print("Correct\n");
    } else {
        print("Inorrect\n");
    }

    if z == 0 {
        print("Inorrect\n");
    } else if z == 1 {
        print("Correct\n");
    } else {
        print("Incorrect\n");
    }

    print("Ok\n");
}
