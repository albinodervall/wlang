enum Instruction_Type : u8 {
    INSTRUCTION_ADD = 1,
    INSTRUCTION_SUBTRACT = 2,
    INSTRUCTION_MULTIPLY = 3,
    INSTRUCTION_DIVIDE = 4,
    INSTRUCTION_NOT = 5,
    INSTRUCTION_AND = 6,
    INSTRUCTION_OR = 7,
    INSTRUCTION_XOR = 8,
    INSTRUCTION_SHIFT_LEFT = 9,
    INSTRUCTION_SHIFT_RIGHT = 10,

    INSTRUCTION_LESS_THAN = 11,
    INSTRUCTION_GREATER_THAN = 12,
    INSTRUCTION_LESS_THAN_OR_EQUAL_TO = 13,
    INSTRUCTION_GREATER_THAN_OR_EQUAL_TO = 14,
    INSTRUCTION_EQUALS = 15,
    INSTRUCTION_NOT_EQUALS = 16,

    INSTRUCTION_CALL = 17,
    INSTRUCTION_CALL_POINTER = 18,
    INSTRUCTION_RETURN = 19,

    INSTRUCTION_JUMP = 20,
    INSTRUCTION_JUMP_IF_ZERO = 21,
    INSTRUCTION_JUMP_IF_NOT_ZERO = 22,

    INSTRUCTION_MOVE = 24,
    INSTRUCTION_MOVE_SYMBOL_POINTER = 25,

    INSTRUCTION_POINTER = 28,
};

struct Symbol;

enum Addressing_Mode : u8 {
    ADDRESSING_MODE_IMMEDIATE,
    ADDRESSING_MODE_DIRECT,
    ADDRESSING_MODE_INDIRECT,
};

struct Move_Operand {
    Addressing_Mode mode;

    union {
        // ADDRESSING_MODE_IMMEDIATE
        u64 immediate;

        // INSTRUCTION_ADDRESSING_MODE_DIRECT, INSTRUCTION_ADDRESSING_MODE_INDIRECT
        struct {
            Register_Index base;
            Register_Index index;
            u32 scale;
            u32 displacement;
            u32 sign_extend; // 0 or 1
        };
    };

    u32 size;
};

struct Instruction {
    Instruction_Type type;

    union {
        struct {
            union {
                Register_Index a_register_index;
                Register_Index left_register_index;
            };

            union {
                Register_Index b_register_index;
                Register_Index right_register_index;
            };

            union {
                struct {
                    union {
                        Register_Index c_register_index;
                        Register_Index result_register_index;
                    };

                    Register_Index d_register_index;
                };

                union {
                    u64 immediate;
                    Symbol * symbol;
                };
            };
        };
    };

    struct {
        Move_Operand from;
        Move_Operand to;
    } move;
} __attribute__((packed));

// move a, b

// where a and b are:
//     #0 (only valid for b)
//     %x
//     %x + %y * c + d
//     [%x]
//     [%x + %y * c + d]
//     @symbol
// where b, c, d are registers or immediates
// or is scale (c) and displacement (d) immediates?
// and scale (c) is limited to 1, 2, 4 and 8?
