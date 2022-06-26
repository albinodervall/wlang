#import "library/assert.w"

overflow :: () #entry {
    {
        a: u8 = 255;
        b: u8 = a + 1;
        assert(b == 0);
    }

    {
        a: u16 = 65535;
        b: u16 = a + 1;
        assert(b == 0);
    }

    {
        a: u32 = 4294967295;
        b: u32 = a + 1;
        assert(b == 0);
    }

    {
        a: u64 = 18446744073709551615;
        b: u64 = a + 1;
        assert(b == 0);
    }

    {
        a: i8 = 127;
        b: i8 = a + 1;
        assert(b == -128);
    }

    {
        a: i16 = 32767;
        b: i16 = a + 1;
        assert(b == -32768);
    }

    {
        a: i32 = 2147483647;
        b: i32 = a + 1;
        assert(b == -2147483648);
    }

    {
        a: i64 = 9223372036854775807;
        b: i64 = a + 1;
        assert(b == -9223372036854775808);
    }

    print("Ok\n");
}
