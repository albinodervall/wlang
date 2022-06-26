enum Error : u32 {
    ERROR_COMPILER                                      = 1000,
    ERROR_UNTERMINATED_STRING_LITERAL                   = 1001,
    ERROR_UNKNOWN_CHARACTER                             = 1002,
    ERROR_UNEXPECTED_TOKEN                              = 1003,
    ERROR_UNKNOWN_TYPE                                  = 1004,
    ERROR_INVALID_EXPRESSION_IN_CONTEXT                 = 1005,
    ERROR_PARAMETERS_ARGUMENTS_MISMATCH                 = 1006,
    ERROR_UNKNOWN_NAME                                  = 1007,
    ERROR_YIELD_IN_FUNCTION_BODY                        = 1008,
    ERROR_FUNCTION_RETURN_MISMATCH                      = 1009,
    ERROR_FUNCTION_MISSING_RETURN                       = 1010,
    ERROR_FUNCTION_MISSING_BODY                         = 1011,
    ERROR_YIELD_MISMATCH                                = 1012,
    ERROR_RANGE_MISMATCH                                = 1013,
    ERROR_GENERIC_TYPE_PROBLEM                          = 1014,
    ERROR_ASSIGNMENT_TYPE_MISMATCH                      = 1015,
    ERROR_TYPE_MISMATCH                                 = 1016,
    ERROR_NO_SUCH_FIELD                                 = 1017,
    ERROR_EXPRESSION_DOES_NOT_DENOTE_TYPE               = 1018,
    ERROR_INVOKED_EXPRESSION_NOT_FUNCTION_OR_STRUCT     = 1019,
    ERROR_ARRAY_INDEX_NOT_INTEGER                       = 1020,
    // ERROR_REDEFINITION_OF_SYMBOL                        = 1021,
    ERROR_UNDEFINED_SYMBOL                              = 1022,
    ERROR_RETURN_OUTSIDE_FUNCTION                       = 1023,
    ERROR_ILLEGAL_DIRECTIVE                             = 1024,
    ERROR_BODY_ON_FOREIGN_FUNCTION                      = 1025,
    ERROR_UNKNOWN_DIRECTIVE                             = 1026,
    ERROR_FIELDS_ON_OPAQUE_STRUCTURE                    = 1027,
    ERROR_CANNOT_INSTANTIATE_OPAQUE_TYPE                = 1028,
    ERROR_INCOMPATIBLE_ARRAY_ELEMENTS                   = 1029,
    ERROR_INDEXED_TYPE_IS_NOT_ARRAY                     = 1030,
    ERROR_NO_SUCH_IMPORT                                = 1031,
    ERROR_INTEGRAL_RANGE                                = 1032,
    ERROR_NON_INTEGRAL_ARRAY_LENGTH                     = 1032,
    ERROR_ENTRY_FUNCTION_PARAMETERS                     = 1033,
    ERROR_ENTRY_FUNCTION_RETURN                         = 1034,
    ERROR_MULTIPLE_ENTRY                                = 1035,
    ERROR_NO_ENTRY                                      = 1036,
    ERROR_INTEGER_LITERAL_TOO_LARGE                     = 1037,
    ERROR_INCOMPATIBLE_CAST                             = 1038,
    ERROR_VARIABLE_TYPE_FUNCTION                        = 1039,
    ERROR_DEFAULT_VALUE_NOT_MATCHING_PARAMETER_TYPE     = 1040,
    ERROR_NON_DEFAULT_VALUE_FOLLOWS_DEFAULT_VALUE       = 1041,
    ERROR_NON_CONSTANT_ARRAY_LENGTH                     = 1042,
    ERROR_NO_SUCH_PARAMETER                             = 1043,
    ERROR_UNNAMED_ARGUMENT_AFTER_NAMED_ARGUMENT         = 1044,
    ERROR_PARAMETER_SPECIFIED_MULTIPLE_TIMES            = 1045,
    ERROR_MISSING_ARGUMENT_FOR_PARAMETER                = 1046,
    ERROR_NO_MATCHING_FUNCTION_OVERLOAD                 = 1047,
    ERROR_MULTIPLE_MATCHING_FUNCTION_OVERLOADS          = 1048,
    ERROR_FUNCTIONS_MARKED_FOREIGN_CANNOT_BE_OVERLOADED = 1049,
    ERROR_FUNCTIONS_MARKED_ENTRY_CANNOT_BE_OVERLOADED   = 1050,
    ERROR_FUNCTION_DUPLICATE_OVERLOAD                   = 1051,
    ERROR_FUNCTION_RETURN_OVERLOAD                      = 1052,
    ERROR_EMPTY_DIRECTIVE                               = 1053,
    ERROR_INTEGER_LITERAL_NEGATIVE                      = 1054,
};

struct Context {
    b32 verbose;
    b32 quiet;
    b32 profile;

    Target target;

    utf8 * input_file_name = NULL;

    utf8 * output_directory_name = NULL;
    utf8 * output_file_name = NULL;

    utf8 ** linker_arguments = NULL;
    u64 linker_arguments_capacity = 0;
    u64 linker_arguments_count = 0;

    Array<Unit> units;
    Array<Error> errors;

    f64 time_merging;
    f64 time_writing_object;
    f64 time_linking;
};

// TODO: Report should maybe not be a function here, but rather passed in as part of the context
//       as a function!

template<class ... T>
void report(utf8 const * format, T ... arguments) {
    print("\u001b[1m" "\u001b[31m" "error" "\u001b[0m" ": ");
    print(format, arguments...);
    print("\n");

    #if TRACE
        debug_stack();
    #endif
}

template<class ... T>
void report(Context & context, utf8 const * format, T ... arguments) {
    if (!context.quiet) {
        print("\u001b[1m" "\u001b[31m" "error" "\u001b[0m" ": ");
        print(format, arguments...);
        print("\n");
    }

    #if TRACE
        debug_stack();
    #endif
}

template<class ... T>
void report(Context & context, Error error, utf8 const * format, T ... arguments) {
    if (!context.quiet) {
        print("\u001b[1m" "\u001b[31m" "error {}" "\u001b[0m" ": ", (u32)error);
        print(format, arguments...);
        print("\n");
    }

    push(context.errors, error);

    #if TRACE
        debug_stack();
    #endif
}

template<class ... T>
void report(Context & context, Error error, Location const & location, utf8 const * format, T ... arguments) {
    if (!context.quiet) {
        print("\u001b[1m" "{}:{}:{}: " "\u001b[31m" "error {}" "\u001b[0m" ": ", location.file, location.begin.line, location.begin.column, (u32)error);
        print(format, arguments...);
        print("\n{}", location);
    }

    push(context.errors, error);

    #if TRACE
        debug_stack();
    #endif
}
