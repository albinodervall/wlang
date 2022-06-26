enum Stage {
    STAGE_PENDING = 0,
    STAGE_DISCARDED,
    STAGE_SUCCEEDED,
    STAGE_FAILED,

    STAGE_PARSE,
    STAGE_FIND_DEPENDENCIES,
    STAGE_WAIT_FOR_DEPENDENCIES,
    STAGE_RESOLVE,
    STAGE_GENERATE,
    STAGE_CONVERT,

    STAGE_FIRST_PROFILED = STAGE_PARSE,
    STAGE_LAST_PROFILED = STAGE_CONVERT,
};

utf8 const * stage_names[] = {
    "pending",
    "discarded",
    "succeeded",
    "failed",

    "parse",
    "find dependencies",
    "wait for dependencies",
    "resolve",
    "generate",
    "convert",
};

struct Dependency {
    b32 fulfilled = FALSE;
    utf8 const * label = NULL;
    Location usage;
};

struct Unit {
    Stage stage;
    Stage previous_stage;

    utf8 const * label;
    utf8 const * source;

    Expression * tree;

    Array<Dependency> dependencies;

    Array<Symbol> exports;
    Code intermediate_code;
    Array<Register> registers;

    Object_Environment output_environment;
    Code machine_code;

    f64 time_per_stage[STAGE_LAST_PROFILED - STAGE_FIRST_PROFILED + 1];
};
