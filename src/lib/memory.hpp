extern "C" void * calloc(unsigned long, unsigned long);
static void * allocate(u64 size) {
    return calloc(size, 1);
}

extern "C" void free(void *);
static void deallocate(void * pointer) {
    free(pointer);
}

extern "C" void * realloc(void *, unsigned long);
static void * reallocate(void * pointer, u64 size) {
    return realloc(pointer, size);
}

extern "C" void * memcpy(void *, void const *, unsigned long);
static void copy(void const * source, void * destination, u64 size) {
    memcpy(destination, source, size);
}

extern "C" void * memset(void *, int, unsigned long);
static void zero(void * destination, u64 size) {
    memset(destination, 0, size);
}

u64 align_up(u64 value, u64 alignment) {
    ASSERT(alignment != 0 && (alignment & (alignment - 1)) == 0);
    return (u64)(((i64)value + ((i64)alignment - 1)) & -(i64)alignment);
}

u64 is_power_of_2(u64 x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}

u64 const ALLOCATOR_BLOCK_SIZE = 16 * 1024 * 1024;

struct Allocator {
    void ** blocks = NULL;
    u64 number_of_blocks = 0;
    u64 number_of_bytes = 0;
};

void * allocate(Allocator & allocator, u64 number_of_bytes_to_allocate) {
    ASSERT(number_of_bytes_to_allocate <= ALLOCATOR_BLOCK_SIZE);

    u64 number_of_bytes_in_block = (allocator.number_of_bytes & (ALLOCATOR_BLOCK_SIZE - 1));
    if (allocator.blocks == NULL || number_of_bytes_to_allocate > (ALLOCATOR_BLOCK_SIZE - number_of_bytes_in_block)) {
        allocator.number_of_blocks += 1;

        allocator.blocks = (void **)reallocate(allocator.blocks, allocator.number_of_blocks * sizeof(void *));
        allocator.blocks[allocator.number_of_blocks - 1] = allocate(ALLOCATOR_BLOCK_SIZE);

        allocator.number_of_bytes = align_up(allocator.number_of_bytes, ALLOCATOR_BLOCK_SIZE);
        number_of_bytes_in_block = 0;
    }

    void * block = allocator.blocks[allocator.number_of_blocks - 1];
    void * pointer = (void *)((u8 *)block + number_of_bytes_in_block);

    allocator.number_of_bytes += number_of_bytes_to_allocate;

    return pointer;
}
