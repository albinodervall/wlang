struct Buffer {
    u64 number_of_bytes_allocated = 0;
    u64 number_of_bytes = 0;
    u8 * bytes = NULL;

    u64 cursor = 0;
};

u64 buffer_reserve(Buffer & buffer, u64 number_of_bytes) {
    b32 appending = (buffer.cursor == buffer.number_of_bytes);

    if (appending) {
        if (buffer.number_of_bytes + number_of_bytes > buffer.number_of_bytes_allocated) {
            buffer.number_of_bytes_allocated = 2 * (buffer.number_of_bytes_allocated + 1);
            if (buffer.number_of_bytes_allocated < buffer.number_of_bytes + number_of_bytes) {
                buffer.number_of_bytes_allocated = buffer.number_of_bytes + number_of_bytes;
            }

            buffer.bytes = (u8 *)reallocate(buffer.bytes, buffer.number_of_bytes_allocated);
            zero(buffer.bytes + buffer.number_of_bytes, buffer.number_of_bytes_allocated - buffer.number_of_bytes);
        }
    }

    u64 offset = buffer.cursor;

    if (appending) {
        buffer.number_of_bytes += number_of_bytes;
    }

    buffer.cursor += number_of_bytes;

    return offset;
}

u64 buffer_reserve_8(Buffer & buffer) {
    return buffer_reserve(buffer, 1);
}

u64 buffer_reserve_16(Buffer & buffer) {
    return buffer_reserve(buffer, 2);
}

u64 buffer_reserve_32(Buffer & buffer) {
    return buffer_reserve(buffer, 4);
}

u64 buffer_reserve_64(Buffer & buffer) {
    return buffer_reserve(buffer, 8);
}

void buffer_patch(Buffer & buffer, u64 offset, void const * bytes, u64 number_of_bytes) {
    ASSERT(offset + number_of_bytes <= buffer.number_of_bytes_allocated);

    copy(bytes, buffer.bytes + offset, number_of_bytes);
}

void buffer_patch_u8(Buffer & buffer, u64 offset, u8 value) {
    buffer_patch(buffer, offset, &value, sizeof(value));
}

void buffer_patch_u16(Buffer & buffer, u64 offset, u16 value) {
    buffer_patch(buffer, offset, &value, sizeof(value));
}

void buffer_patch_u32(Buffer & buffer, u64 offset, u32 value) {
    buffer_patch(buffer, offset, &value, sizeof(value));
}

void buffer_patch_u64(Buffer & buffer, u64 offset, u64 value) {
    buffer_patch(buffer, offset, &value, sizeof(value));
}

void buffer_patch_i8(Buffer & buffer, u64 offset, i8 value) {
    buffer_patch(buffer, offset, &value, sizeof(value));
}

void buffer_patch_i16(Buffer & buffer, u64 offset, i16 value) {
    buffer_patch(buffer, offset, &value, sizeof(value));
}

void buffer_patch_i32(Buffer & buffer, u64 offset, i32 value) {
    buffer_patch(buffer, offset, &value, sizeof(value));
}

void buffer_patch_i64(Buffer & buffer, u64 offset, i64 value) {
    buffer_patch(buffer, offset, &value, sizeof(value));
}

void buffer_write(Buffer & buffer, void const * bytes, u64 number_of_bytes) {
    u64 offset = buffer_reserve(buffer, number_of_bytes);
    if (bytes != NULL) {
        buffer_patch(buffer, offset, bytes, number_of_bytes);
    }
}

void buffer_write_u8(Buffer & buffer, u8 value) {
    buffer_write(buffer, &value, sizeof(value));
}

void buffer_write_u16(Buffer & buffer, u16 value) {
    buffer_write(buffer, &value, sizeof(value));
}

void buffer_write_u32(Buffer & buffer, u32 value) {
    buffer_write(buffer, &value, sizeof(value));
}

void buffer_write_u64(Buffer & buffer, u64 value) {
    buffer_write(buffer, &value, sizeof(value));
}

void buffer_write_i8(Buffer & buffer, i8 value) {
    buffer_write(buffer, &value, sizeof(value));
}

void buffer_write_i16(Buffer & buffer, i16 value) {
    buffer_write(buffer, &value, sizeof(value));
}

void buffer_write_i32(Buffer & buffer, i32 value) {
    buffer_write(buffer, &value, sizeof(value));
}

void buffer_write_i64(Buffer & buffer, i64 value) {
    buffer_write(buffer, &value, sizeof(value));
}

void * buffer_peek(Buffer & buffer, u64 offset, u64 number_of_bytes) {
    ASSERT(offset + number_of_bytes <= buffer.number_of_bytes);

    return buffer.bytes + offset;
}

void const * buffer_peek(Buffer const & buffer, u64 offset, u64 number_of_bytes) {
    ASSERT(offset + number_of_bytes <= buffer.number_of_bytes);

    return buffer.bytes + offset;
}

u8 buffer_peek_u8(Buffer const & buffer, u64 offset) {
    return *(u8 const *)buffer_peek(buffer, offset, sizeof(u8));
}

u16 buffer_peek_u16(Buffer const & buffer, u64 offset) {
    return *(u16 const *)buffer_peek(buffer, offset, sizeof(u16));
}

u32 buffer_peek_u32(Buffer const & buffer, u64 offset) {
    return *(u32 const *)buffer_peek(buffer, offset, sizeof(u32));
}

u64 buffer_peek_u64(Buffer const & buffer, u64 offset) {
    return *(u64 const *)buffer_peek(buffer, offset, sizeof(u64));
}

void buffer_set_cursor(Buffer & buffer, u64 offset) {
    buffer.cursor = offset;
}

void buffer_reset_cursor(Buffer & buffer) {
    buffer.cursor = buffer.number_of_bytes;
}

u64 buffer_get_cursor(Buffer & buffer) {
    return buffer.cursor;
}

void buffer_copy(Buffer & destination, Buffer const & source) {
    buffer_write(destination, source.bytes, source.number_of_bytes);
}

void buffer_print_raw(void * bytes, u64 offset = 0, u64 size = 0, utf8 const * indent = NULL) {
    print("{!}| ", indent);

    for (u64 index = 0; index < size; index += 1) {
        u8 const * pointer = (u8 const *)bytes + offset + index;

        print("{02x} ", *pointer);
        if (index < size - 1) {
            if (index != 0 && ((index + 1) % 16) == 0) {
                print("\n{!}| ", indent);
            } else if (index != 0 && ((index + 1) % 8) == 0) {
                print(" ");
            }
        }
    }

    print("\n");
}

void buffer_print(Buffer const & buffer, u64 offset = 0, u64 size = 0xFFFFFFFFFFFFFFFF) {
    if (offset > buffer.number_of_bytes) {
        return;
    }

    if (offset + size > buffer.number_of_bytes) {
        size = buffer.number_of_bytes - offset;
    }

    buffer_print_raw(buffer.bytes, offset, size);
}
