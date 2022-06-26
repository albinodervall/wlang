#import "library/assert.w"

i :: (a: i32) {
}

j :: (b: i32) {
}

main :: () #entry {
    f: &(z: i32) = i;
    g: &(z: i32) = j;

    print("Ok\n");
}
