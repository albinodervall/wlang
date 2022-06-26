#import "library/assert.w"

main :: () #entry {
    a: [2]i32 = [ 2, 3, ];

    assert(a[0] == 2);
    assert(a[1] == 3);

    i: u64 = 0;
    assert(a[i] == 2);

    j: u64 = 1;
    assert(a[j] == 3);

    print("Ok\n");
}
