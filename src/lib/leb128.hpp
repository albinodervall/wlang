u64 leb_decode(u8 const * data, int & o) {
    u64 result = 0;
    u64 shift = 0;
    while (1) {
        u8 byte = *(u8 *)(data + o);
        o += sizeof(u8);

        result |= (byte & 0b01111111) << shift;

        if ((byte & 0b10000000) == 0) {
            break;
        }

        shift += 7;
    }

    return result;
}

i64 leb_decode_signed(u8 const * data, int & o) {
    i64 result = 0;
    i64 shift = 0;
    i64 size = 64;

    u8 byte;
    while (1) {
        byte = *(u8 *)(data + o);
        o += sizeof(u8);

        result |= ((byte & 0b1111111) << shift);
        shift += 7;
        if ((result & 0b10000000) == 0) {
            break;
        }
    }

    // sign bit of byte is second high order bit (0x40)
    if ((shift < size) && (byte & 0b01000000)) {
        result |= -(1 << shift);
    }

    return result;
}
