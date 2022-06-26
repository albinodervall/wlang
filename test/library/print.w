printf :: (format: &u8): i32 #foreign;
calloc :: (num: u64, size: u64): &void #foreign;
free :: (pointer: &void) #foreign;
memcpy :: (dest: &void, src: &void, count: u64): &void #foreign;

print :: (value: string) {
    pointer: &u8 = calloc(value.number_of_bytes + 1, 1) as &u8;
    // TODO: What we really want here is:
    // pointer[:value.number_of_bytes] = value[:]
    memcpy(pointer, value.pointer, value.number_of_bytes);

    printf(pointer);

    free(pointer);
}

modulus :: (a: u64, b: u64): u64 {
    return a - (b * (a / b));
}

print_u64 :: (value: u64) {
    digits: [21]u8;
    count: u64 = 0;

    // Generate the digits
    if value > 0 {
        while value > 0 {
            digit: u8 = modulus(value, 10) as u8;
            value = value / 10;

            digits[count] = 0x30 + digit;
            count = count + 1;
        }
    } else {
        digits[count] = 0x30;
        count = count + 1;
    }

    // Reverse the digits
    if count > 1 {
        for index in 0..((count / 2) - 1) {
            temporary: u8 = digits[index];
            digits[index] = digits[count - 1 - index];
            digits[count - 1 - index] = temporary;
        }
    }

    // Print the digits
    s: string;
    s.pointer = &digits;
    s.number_of_bytes = count + 1;

    printf(s.pointer);
    printf("\n\0".pointer);
}

print_u32 :: (value: u32) {
    print_u64(value as u64);
}

print_i64 :: (value: i64) {
    if value < 0 {
        printf("-\0".pointer);
        value = -value;
    }

    print_u64(value as u64);
}

print_i32 :: (value: i32) {
    print_i64(value as i64);
}

print_ascii :: (value: u8) {
    bytes: [2]u8;
    bytes[0] = value;

    s: string;
    s.pointer = &bytes;
    s.number_of_bytes = 2;

    printf(s.pointer);
}
