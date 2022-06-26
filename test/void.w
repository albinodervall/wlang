#import "library/print.w"

print2 :: () {
    x : string = "hej\n";
    print(x);
}

main :: () #entry {
    print2();

    print("Ok\n");
}
