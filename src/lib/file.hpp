extern "C" char *getcwd(char buf[], unsigned long size);
extern "C" char *realpath(const char * path, char * resolved_path);

static utf8 * get_absolute_path(utf8 * path) {
    return realpath(path, NULL);
}

static utf8 * get_working_directory() {
    return getcwd(NULL, 0);
}

// TODO: The semantics of this function is a bit weird
static utf8 * get_directory_name(utf8 const * path) {
    utf8 const * file_name = utf8_find_last(path, "/");
    if (file_name == NULL) {
        return NULL;
    }

    return utf8_copy(path, file_name);
}

static utf8 const * get_file_name(utf8 const * path) {
    utf8 const * file_name = utf8_find_last(path, "/");
    if (file_name == NULL) {
        return path;
    }

    return utf8_next(file_name);
}

static utf8 const * get_file_name_without_file_extension(utf8 const * path) {
    utf8 const * file_name = get_file_name(path);

    utf8 const * file_extension = utf8_find_last(file_name, ".");
    if (file_extension == NULL) {
        return file_name;
    }

    return utf8_copy(file_name, file_extension);
}

// TODO: Not very portable, but I really don't want to include stdio.h
struct FILE;
extern "C" FILE * fopen(const char *, const char *);
extern "C" int fseek(FILE *, long, int);
extern "C" unsigned long fread(void *, unsigned long, unsigned long, FILE *);
extern "C" int fclose(FILE *);
extern "C" long ftell(FILE *);
extern "C" unsigned long fwrite(const void *, unsigned long, unsigned long, FILE *);
#if defined(__APPLE__)
    extern "C" FILE * popen(const char *, const char *);
    extern "C" int pclose(FILE *);
    extern "C" char * fgets(char *, int, FILE *);
#endif
extern "C" int mkdir(const char *, int); // This actually takes mode_t as the second argument, but ISO C++ forbids forward declares to enums

#define SEEK_SET 0
#define SEEK_END 2

static b32 file_exists(utf8 const * file_name) {
    FILE * file = fopen(file_name, "rb");
    if (file) {
        fclose(file);
        return TRUE;
    }

    return FALSE;
}

struct Get_File_Result {
    u64 number_of_bytes;
    u8 * bytes;
};

static Get_File_Result get_file(char const * file_name, b32 null_terminate) {
    FILE * file = fopen(file_name, "rb");
    ASSERT(file != NULL);

    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char * file_contents = (char *)allocate(size + (null_terminate ? 1 : 0));
    if (null_terminate) {
        file_contents[size] = '\0';
    }

    fread(file_contents, 1, size, file);

    fclose(file);

    return Get_File_Result{
        .number_of_bytes = (u64)size,
        .bytes = (u8 *)file_contents,
    };
}

static b32 write_file(utf8 const * file_name, u8 const * bytes, u64 number_of_bytes) {
    utf8 const * directory_name = get_directory_name(file_name);
    if (directory_name != NULL) {
        mkdir(directory_name, 0777);
    }

    FILE * file = fopen(file_name, "wb");
    ASSERT(file != NULL);

    fwrite(bytes, number_of_bytes, 1, file);

    fclose(file);

    return TRUE;
}
