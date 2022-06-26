struct Expression;

struct Symbol_Function_Parameter {
    utf8 const * name;

    Expression * default_value;
};

void format(Formatter & formatter, Format const &, Symbol_Function_Parameter & argument) {
    format(formatter, "Symbol_Function_Parameter\\{\n");
    format(formatter, "    name = {}\n", argument.name);
    format(formatter, "    default_value = {}\n", argument.default_value);
    format(formatter, "\\}\n");
}

struct Argument {
    Expression * name;
    Expression * value;
};

void format(Formatter & formatter, Format const &, Argument & argument) {
    format(formatter, "{} = {}", argument.name, argument.value);
}

enum Expression_Type : u32 {
    // integer:   '0b' [0-1]*
    //          | [0-9]+
    //          | '0x' [0-9A-Fa-f]*
    EXPRESSION_INTEGER,

    // EXPRESSION_FLOAT,

    // string: '"' [^"]* '"'
    EXPRESSION_STRING,

    // identifier: (_ | Ll | Lm | Lo | Lt | Lu) (_ | Ll | Lm | Lo | Lt | Lu | Nd)*
    EXPRESSION_IDENTIFIER,

    // unary: (not | - | ~ | *) integer
    EXPRESSION_UNARY_BOOLEAN_NOT,
    EXPRESSION_UNARY_MINUS,
    EXPRESSION_UNARY_BITWISE_NOT,
    // EXPRESSION_UNARY_POINTER,

    // binary: integer ('+' | '-' | '|' | '^' | '<' | '>' | '<=' | '>=' | '==' | '!=' | '*' | '/' | '&' | '<<' | '>>' | 'and' | 'or')
    EXPRESSION_BINARY_ADDITION,
    EXPRESSION_BINARY_SUBTRACTION,
    EXPRESSION_BINARY_BITWISE_OR,
    EXPRESSION_BINARY_BITWISE_XOR,
    EXPRESSION_BINARY_COMPARISON_LESS_THAN,
    EXPRESSION_BINARY_COMPARISON_GREATER_THAN,
    EXPRESSION_BINARY_COMPARISON_LESS_THAN_OR_EQUAL_TO,
    EXPRESSION_BINARY_COMPARISON_GREATER_THAN_OR_EQUAL_TO,
    EXPRESSION_BINARY_COMPARISON_EQUALS,
    EXPRESSION_BINARY_COMPARISON_NOT_EQUALS,
    EXPRESSION_BINARY_MULTIPLICATION,
    EXPRESSION_BINARY_DIVISION,
    EXPRESSION_BINARY_BITWISE_AND,
    EXPRESSION_BINARY_SHIFT_LEFT,
    EXPRESSION_BINARY_SHIFT_RIGHT,
    EXPRESSION_BINARY_BOOLEAN_AND,
    EXPRESSION_BINARY_BOOLEAN_OR,

    // expression '(' ')'
    EXPRESSION_INVOCATION,

    // expression '.' expression
    EXPRESSION_FIELD,

    // expression '[' expression ']'
    EXPRESSION_INDEX,

    EXPRESSION_TRUE,
    EXPRESSION_FALSE,

    // '(' expression ')' [ ':' expression ] '{'  '}' [ 'else' expression ]
    EXPRESSION_FUNCTION,

    // struct '{' ... '}'
    EXPRESSION_STRUCTURE,

    // 'if' expression '{'  '}' [ 'else' expression ]
    EXPRESSION_IF,

    // 'for' identifier [ ',' identifier ] 'in' expression '{' expression* '}'
    EXPRESSION_FOR,

    // 'while' expression '{' expression* '}'
    EXPRESSION_WHILE,

    // expression '..' expression
    EXPRESSION_RANGE,

    // '{' expression* '}'
    EXPRESSION_BLOCK,

    EXPRESSION_ASSIGNMENT,

    // 'return' expression
    EXPRESSION_RETURN,

    // expression 'as' expression
    EXPRESSION_AS,

    // '[' expression* ']'
    EXPRESSION_ARRAY,

    // '&' expression
    EXPRESSION_ADDRESS_OF,
    // '*' expression
    EXPRESSION_DEREFERENCE,

    EXPRESSION_VARIABLE_DECLARATION,
    EXPRESSION_FUNCTION_DECLARATION,
    EXPRESSION_STRUCTURE_DECLARATION,
    EXPRESSION_OPAQUE_DECLARATION,

    EXPRESSION_TYPE_POINTER,
    EXPRESSION_TYPE_ARRAY,
    EXPRESSION_TYPE_FUNCTION,

    EXPRESSION_IMPORT,
    EXPRESSION_LINKER,
};

void format(Formatter & formatter, Format const &, Expression_Type argument) {
    switch (argument) {
        case EXPRESSION_INTEGER: {
            return format(formatter, "{}", "EXPRESSION_INTEGER");
        } break;

        case EXPRESSION_STRING: {
            return format(formatter, "{}", "EXPRESSION_STRING");
        } break;

        case EXPRESSION_IDENTIFIER: {
            return format(formatter, "{}", "EXPRESSION_IDENTIFIER");
        } break;

        case EXPRESSION_UNARY_BOOLEAN_NOT: {
            return format(formatter, "{}", "EXPRESSION_UNARY_BOOLEAN_NOT");
        } break;

        case EXPRESSION_UNARY_MINUS: {
            return format(formatter, "{}", "EXPRESSION_UNARY_MINUS");
        } break;

        case EXPRESSION_UNARY_BITWISE_NOT: {
            return format(formatter, "{}", "EXPRESSION_UNARY_BITWISE_NOT");
        } break;

        case EXPRESSION_BINARY_ADDITION: {
            return format(formatter, "{}", "EXPRESSION_BINARY_ADDITION");
        } break;

        case EXPRESSION_BINARY_SUBTRACTION: {
            return format(formatter, "{}", "EXPRESSION_BINARY_SUBTRACTION");
        } break;

        case EXPRESSION_BINARY_BITWISE_OR: {
            return format(formatter, "{}", "EXPRESSION_BINARY_BITWISE_OR");
        } break;

        case EXPRESSION_BINARY_BITWISE_XOR: {
            return format(formatter, "{}", "EXPRESSION_BINARY_BITWISE_XOR");
        } break;

        case EXPRESSION_BINARY_COMPARISON_LESS_THAN: {
            return format(formatter, "{}", "EXPRESSION_BINARY_COMPARISON_LESS_THAN");
        } break;

        case EXPRESSION_BINARY_COMPARISON_GREATER_THAN: {
            return format(formatter, "{}", "EXPRESSION_BINARY_COMPARISON_GREATER_THAN");
        } break;

        case EXPRESSION_BINARY_COMPARISON_LESS_THAN_OR_EQUAL_TO: {
            return format(formatter, "{}", "EXPRESSION_BINARY_COMPARISON_LESS_THAN_OR_EQUAL_TO");
        } break;

        case EXPRESSION_BINARY_COMPARISON_GREATER_THAN_OR_EQUAL_TO: {
            return format(formatter, "{}", "EXPRESSION_BINARY_COMPARISON_GREATER_THAN_OR_EQUAL_TO");
        } break;

        case EXPRESSION_BINARY_COMPARISON_EQUALS: {
            return format(formatter, "{}", "EXPRESSION_BINARY_COMPARISON_EQUALS");
        } break;

        case EXPRESSION_BINARY_COMPARISON_NOT_EQUALS: {
            return format(formatter, "{}", "EXPRESSION_BINARY_COMPARISON_NOT_EQUALS");
        } break;

        case EXPRESSION_BINARY_MULTIPLICATION: {
            return format(formatter, "{}", "EXPRESSION_BINARY_MULTIPLICATION");
        } break;

        case EXPRESSION_BINARY_DIVISION: {
            return format(formatter, "{}", "EXPRESSION_BINARY_DIVISION");
        } break;

        case EXPRESSION_BINARY_BITWISE_AND: {
            return format(formatter, "{}", "EXPRESSION_BINARY_BITWISE_AND");
        } break;

        case EXPRESSION_BINARY_SHIFT_LEFT: {
            return format(formatter, "{}", "EXPRESSION_BINARY_SHIFT_LEFT");
        } break;

        case EXPRESSION_BINARY_SHIFT_RIGHT: {
            return format(formatter, "{}", "EXPRESSION_BINARY_SHIFT_RIGHT");
        } break;

        case EXPRESSION_BINARY_BOOLEAN_AND: {
            return format(formatter, "{}", "EXPRESSION_BINARY_BOOLEAN_AND");
        } break;

        case EXPRESSION_BINARY_BOOLEAN_OR: {
            return format(formatter, "{}", "EXPRESSION_BINARY_BOOLEAN_OR");
        } break;

        case EXPRESSION_INVOCATION: {
            return format(formatter, "{}", "EXPRESSION_INVOCATION");
        } break;

        case EXPRESSION_FIELD: {
            return format(formatter, "{}", "EXPRESSION_FIELD");
        } break;

        case EXPRESSION_INDEX: {
            return format(formatter, "{}", "EXPRESSION_INDEX");
        } break;

        case EXPRESSION_TRUE: {
            return format(formatter, "{}", "EXPRESSION_TRUE");
        } break;

        case EXPRESSION_FALSE: {
            return format(formatter, "{}", "EXPRESSION_FALSE");
        } break;

        case EXPRESSION_FUNCTION: {
            return format(formatter, "{}", "EXPRESSION_FUNCTION");
        } break;

        case EXPRESSION_STRUCTURE: {
            return format(formatter, "{}", "EXPRESSION_STRUCTURE");
        } break;

        case EXPRESSION_IF: {
            return format(formatter, "{}", "EXPRESSION_IF");
        } break;

        case EXPRESSION_FOR: {
            return format(formatter, "{}", "EXPRESSION_FOR");
        } break;

        case EXPRESSION_WHILE: {
            return format(formatter, "{}", "EXPRESSION_WHILE");
        } break;

        case EXPRESSION_RANGE: {
            return format(formatter, "{}", "EXPRESSION_RANGE");
        } break;

        case EXPRESSION_BLOCK: {
            return format(formatter, "{}", "EXPRESSION_BLOCK");
        } break;

        case EXPRESSION_ASSIGNMENT: {
            return format(formatter, "{}", "EXPRESSION_ASSIGNMENT");
        } break;

        case EXPRESSION_RETURN: {
            return format(formatter, "{}", "EXPRESSION_RETURN");
        } break;

        case EXPRESSION_AS: {
            return format(formatter, "{}", "EXPRESSION_AS");
        } break;

        case EXPRESSION_ARRAY: {
            return format(formatter, "{}", "EXPRESSION_ARRAY");
        } break;

        case EXPRESSION_ADDRESS_OF: {
            return format(formatter, "{}", "EXPRESSION_ADDRESS_OF");
        } break;

        case EXPRESSION_DEREFERENCE: {
            return format(formatter, "{}", "EXPRESSION_DEREFERENCE");
        } break;

        case EXPRESSION_VARIABLE_DECLARATION: {
            return format(formatter, "{}", "EXPRESSION_VARIABLE_DECLARATION");
        } break;

        case EXPRESSION_FUNCTION_DECLARATION: {
            return format(formatter, "{}", "EXPRESSION_FUNCTION_DECLARATION");
        } break;

        case EXPRESSION_STRUCTURE_DECLARATION: {
            return format(formatter, "{}", "EXPRESSION_STRUCTURE_DECLARATION");
        } break;

        case EXPRESSION_OPAQUE_DECLARATION: {
            return format(formatter, "{}", "EXPRESSION_OPAQUE_DECLARATION");
        } break;

        case EXPRESSION_TYPE_POINTER: {
            return format(formatter, "{}", "EXPRESSION_TYPE_POINTER");
        } break;

        case EXPRESSION_TYPE_ARRAY: {
            return format(formatter, "{}", "EXPRESSION_TYPE_ARRAY");
        } break;

        case EXPRESSION_TYPE_FUNCTION: {
            return format(formatter, "{}", "EXPRESSION_TYPE_FUNCTION");
        } break;

        case EXPRESSION_IMPORT: {
            return format(formatter, "{}", "EXPRESSION_IMPORT");
        } break;

        case EXPRESSION_LINKER: {
            return format(formatter, "{}", "EXPRESSION_LINKER");
        } break;

    }

    ASSERT_NOT_REACHED();
}

struct Expression;

struct Function_Parameter {
    Expression * name;
    Expression * type;
    Expression * value;
};

struct Field {
    Expression * name;
    Expression * type;
};

void format(Formatter & formatter, Format const &, Field const & argument) {
    format(formatter, "{}: {}", argument.name, argument.type);
}

void format(Formatter & formatter, Format const &, Function_Parameter & argument) {
    format(formatter, "{}: {}", argument.name, argument.type);
}

enum Expression_Flags : u32 {
    EXPRESSION_FLAGS_NONE = 0,
    // TODO: Rename these to EXPRESSION_FLAG_
    EXPRESSION_FLAGS_STATEMENT = 1 << 0,
    EXPRESSION_FLAGS_EXPLICIT_STATEMENT = 1 << 1, // TODO: This maybe should be renamed "yeilds" or something

    EXPRESSION_FLAGS_BINARY = 1 << 2,
    EXPRESSION_FLAGS_DECIMAL = 1 << 3,
    EXPRESSION_FLAGS_HEXADECIMAL = 1 << 4,
    EXPRESSION_FLAGS_NEGATIVE = 1 << 5,

    EXPRESSION_FLAGS_DECLARATION = 1 << 6,
};

// Christ almighty...
inline Expression_Flags operator|(Expression_Flags a, Expression_Flags b) {
    return (Expression_Flags)((u32)a | (u32)b);
}

inline Expression_Flags operator&(Expression_Flags a, Expression_Flags b) {
    return (Expression_Flags)((u32)a & (u32)b);
}

inline Expression_Flags & operator|=(Expression_Flags& a, Expression_Flags b) {
    return (Expression_Flags &)((u32 &)a |= (u32)b);
}

struct Symbol;
struct Environment;

struct Expression {
    Expression_Type type;
    Expression_Flags flags;

    Type * resolved_type;
    Symbol * resolved_symbol;

    Location location;

    Symbol * environment;

    union {
        struct {
            Expression * left;
            Expression * right;
        };

        Expression * expression;

        utf8 const * identifier;

        utf8 const * string;
        u64 integer;
    };
};

struct Expression_Invocation : Expression {
    Expression * invoked;
    Array<Argument> arguments;
};

struct Expression_Field : Expression {
    Expression * accessed;
    Expression * accessor;
};

struct Expression_Index : Expression {
    Expression * indexed;
    Expression * index;
};

struct Expression_Function : Expression {
    Array<Function_Parameter> parameters;
    Expression * return_type;

    Expression * body;

    b32 entry;
    b32 foreign;
};

struct Expression_Structure : Expression {
    Array<Field> fields;
};

struct Expression_If : Expression {
    Expression * condition;
    Expression * then_expression;
    Expression * else_expression;
};

struct Expression_For : Expression {
    Expression * index;
    Expression * index_type;
    Expression * value;
    Expression * value_type;
    Expression * expression;
    Expression * block;
};

struct Expression_While : Expression {
    Expression * condition;
    Expression * block;
};

struct Expression_Block : Expression {
    Array<Expression *> expressions;
};

struct Expression_Array : Expression {
    Array<Expression *> expressions;
};

struct Expression_Declaration : Expression {
    Expression * declared_name;
    Expression * declared_type;
    Expression * declared_value;

    Symbol * symbol;
};

struct Expression_Type_Array : Expression {
    Expression * type;
    Expression * length;
};

struct Expression_Type_Function : Expression {
    Array<Function_Parameter> parameters;
    Array<Symbol_Function_Parameter> resolved_parameters;
    Expression * return_type;
};

static Allocator expression_allocator;

Expression * make_expression(u64 size, Expression_Type type, Location const & location) {
    Expression * expression = (Expression *)allocate(expression_allocator, size);
    expression->type = type;
    expression->flags = EXPRESSION_FLAGS_NONE;
    expression->resolved_type = NULL;
    expression->location = location;

    return expression;
}

Expression * make_expression_integer(Location const & location, Token * token, u32 negative, u32 base, u64 value) {
    Expression * expression = make_expression(sizeof(Expression), EXPRESSION_INTEGER, location);

    expression->integer = value;

    if (base == 2) {
        expression->flags |= EXPRESSION_FLAGS_BINARY;
    } else if (base == 10) {
        expression->flags |= EXPRESSION_FLAGS_DECIMAL;
    } else if (base == 16) {
        expression->flags |= EXPRESSION_FLAGS_HEXADECIMAL;
    } else {
        ASSERT_NOT_REACHED();
    }

    if (negative) {
        expression->flags |= EXPRESSION_FLAGS_NEGATIVE;
    }

    return expression;
}

Expression * make_expression_string(Location const & location, Token * token) {
    Expression * expression = make_expression(sizeof(Expression), EXPRESSION_STRING, location);

    {
        int length = (int)(token->location.end.pointer - token->location.begin.pointer - 1);
        expression->string = utf8_copy(token->location.begin.pointer + 1, length);
    }

    return expression;
}

Expression * make_expression_identifier(Location const & location, Token * token) {
    Expression * expression = make_expression(sizeof(Expression), EXPRESSION_IDENTIFIER, location);

    {
        expression->identifier = utf8_copy(token->location.begin.pointer, token->location.end.pointer + 1);
    }

    return expression;
}

Expression * make_expression_unary(Location const & location, Expression_Type type, Expression * right) {
    Expression * expression = make_expression(sizeof(Expression), type, location);
    expression->right = right;

    return expression;
}

Expression * make_expression_binary(Location const & location, Expression_Type type, Expression * left, Expression * right) {
    Expression * expression = make_expression(sizeof(Expression), type, location);
    expression->left = left;
    expression->right = right;

    return expression;
}

Expression_Invocation * make_expression_invocation(Location const & location, Expression * invoked, Array<Argument> & arguments) {
    Expression_Invocation * expression = (Expression_Invocation *)make_expression(sizeof(Expression_Invocation), EXPRESSION_INVOCATION, location);
    expression->invoked = invoked;
    expression->arguments = arguments;

    return expression;
}

Expression_Field * make_expression_field(Location const & location, Expression * accessed, Expression * accessor) {
    Expression_Field * expression = (Expression_Field *)make_expression(sizeof(Expression_Field), EXPRESSION_FIELD, location);
    expression->accessed = accessed;
    expression->accessor = accessor;

    return expression;
}

Expression_Index * make_expression_index(Location const & location, Expression * indexed, Expression * index) {
    Expression_Index * expression = (Expression_Index *)make_expression(sizeof(Expression_Index), EXPRESSION_INDEX, location);
    expression->indexed = indexed;
    expression->index = index;

    return expression;
}

Expression * make_expression_true(Location const & location, Token * token) {
    Expression * expression = make_expression(sizeof(Expression), EXPRESSION_TRUE, location);

    return expression;
}

Expression * make_expression_false(Location const & location, Token * token) {
    Expression * expression = make_expression(sizeof(Expression), EXPRESSION_FALSE, location);

    return expression;
}

Expression_Function * make_expression_function(Location const & location, Array<Function_Parameter> & parameters, Expression * return_type, Expression * body) {
    Expression_Function * expression = (Expression_Function *)make_expression(sizeof(Expression_Function), EXPRESSION_FUNCTION, location);
    expression->parameters = parameters;
    expression->return_type = return_type;
    expression->body = body;
    expression->entry = FALSE;
    expression->foreign = FALSE;

    return expression;
}

Expression_Structure * make_expression_struct(Location const & location, Array<Field> & fields) {
    Expression_Structure * expression = (Expression_Structure *)make_expression(sizeof(Expression_Structure), EXPRESSION_STRUCTURE, location);
    expression->fields = fields;

    return expression;
}

Expression * make_expression_range(Location const & location, Expression * left, Expression * right) {
    Expression * expression = make_expression(sizeof(Expression), EXPRESSION_RANGE, location);
    expression->left = left;
    expression->right = right;

    return expression;
}

Expression_Block * make_expression_block(Location const & location, Array<Expression *> expressions) {
    Expression_Block * expression = (Expression_Block *)make_expression(sizeof(Expression_Block), EXPRESSION_BLOCK, location);
    expression->expressions = expressions;

    return expression;
}

Expression_Array * make_expression_array(Location const & location, Array<Expression *> expressions) {
    Expression_Array * expression = (Expression_Array *)make_expression(sizeof(Expression_Array), EXPRESSION_ARRAY, location);
    expression->expressions = expressions;

    return expression;
}

Expression * make_expression_return(Location const & location, Expression * value) {
    Expression * expression = make_expression(sizeof(Expression), EXPRESSION_RETURN, location);
    expression->expression = value;

    return expression;
}

Expression * make_expression_as(Location const & location, Expression * left, Expression * right) {
    Expression * expression = make_expression(sizeof(Expression), EXPRESSION_AS, location);
    expression->left = left;
    expression->right = right;

    return expression;
}

Expression_For * make_expression_for(Location const & location, Expression * index, Expression * index_type, Expression * value, Expression * value_type, Expression * expression, Expression * block) {
    Expression_For * result = (Expression_For *)make_expression(sizeof(Expression_For), EXPRESSION_FOR, location);
    result->index = index;
    result->index_type = index_type;
    result->value = value;
    result->value_type = value_type;
    result->expression = expression;
    result->block = block;
    return result;
}

Expression_While * make_expression_while(Location const & location, Expression * condition, Expression * block) {
    Expression_While * result = (Expression_While *)make_expression(sizeof(Expression_While), EXPRESSION_WHILE, location);
    result->condition = condition;
    result->block = block;
    return result;
}

Expression_If * make_expression_if(Location const & location, Expression * condition, Expression * then_expression, Expression * else_expression) {
    Expression_If * expression = (Expression_If *)make_expression(sizeof(Expression_If), EXPRESSION_IF, location);
    expression->condition = condition;
    expression->then_expression = then_expression;
    expression->else_expression = else_expression;

    return expression;
}

Expression_Declaration * make_expression_variable_declaration(Location const & location, Expression * name, Expression * type, Expression * value) {
    Expression_Declaration * expression = (Expression_Declaration *)make_expression(sizeof(Expression_Declaration), EXPRESSION_VARIABLE_DECLARATION, location);
    expression->flags |= EXPRESSION_FLAGS_DECLARATION;
    expression->declared_name = name;
    expression->declared_type = type;
    expression->declared_value = value;

    return expression;
}

Expression_Declaration * make_expression_function_declaration(Location const & location, Expression * name, Expression * value) {
    Expression_Declaration * expression = (Expression_Declaration *)make_expression(sizeof(Expression_Declaration), EXPRESSION_FUNCTION_DECLARATION, location);
    expression->flags |= EXPRESSION_FLAGS_DECLARATION;
    expression->declared_name = name;
    expression->declared_type = NULL;
    expression->declared_value = value;

    return expression;
}

Expression_Declaration * make_expression_struct_declaration(Location const & location, Expression * name, Expression * value) {
    Expression_Declaration * expression = (Expression_Declaration *)make_expression(sizeof(Expression_Declaration), EXPRESSION_STRUCTURE_DECLARATION, location);
    expression->flags |= EXPRESSION_FLAGS_DECLARATION;
    expression->declared_name = name;
    expression->declared_type = NULL;
    expression->declared_value = value;

    return expression;
}

Expression_Declaration * make_expression_opaque_declaration(Location const & location, Expression * name) {
    Expression_Declaration * expression = (Expression_Declaration *)make_expression(sizeof(Expression_Declaration), EXPRESSION_OPAQUE_DECLARATION, location);
    expression->flags |= EXPRESSION_FLAGS_DECLARATION;
    expression->declared_name = name;
    expression->declared_type = NULL;
    expression->declared_value = NULL;

    return expression;
}

Expression * make_expression_type_pointer(Location const & location, Expression * value) {
    Expression * expression = make_expression(sizeof(Expression), EXPRESSION_TYPE_POINTER, location);
    expression->expression = value;

    return expression;
}

Expression * make_expression_type_array(Location const & location, Expression * type_expression, Expression * length_expression) {
    Expression_Type_Array * expression = (Expression_Type_Array *)make_expression(sizeof(Expression_Type_Array), EXPRESSION_TYPE_ARRAY, location);
    expression->type = type_expression;
    expression->length = length_expression;

    return expression;
}

Expression * make_expression_type_function(Location const & location, Array<Function_Parameter> const & parameters, Expression * return_type) {
    Expression_Type_Function * expression = (Expression_Type_Function *)make_expression(sizeof(Expression_Type_Function), EXPRESSION_TYPE_FUNCTION, location);
    expression->parameters = parameters;
    expression->return_type = return_type;

    return expression;
}

Expression * make_expression_import(Location const & location, Expression * value) {
    Expression * expression = make_expression(sizeof(Expression), EXPRESSION_IMPORT, location);
    expression->flags |= EXPRESSION_FLAGS_STATEMENT;
    expression->expression = value;

    return expression;
}

Expression * make_expression_linker(Location const & location, Expression * value) {
    Expression * expression = make_expression(sizeof(Expression), EXPRESSION_LINKER, location);
    expression->flags |= EXPRESSION_FLAGS_STATEMENT;
    expression->expression = value;

    return expression;
}

void format(Formatter & formatter, Format const & specifier, Expression const * expression) {
    if (expression->resolved_type) {
        format(formatter, "<{}>", expression->resolved_type);
    }

    switch (expression->type) {
        case EXPRESSION_INTEGER: {
            format(formatter, "{}", expression->integer);
        } break;

        case EXPRESSION_STRING: {
            format(formatter, "\"{}\"", expression->string);
        } break;

        case EXPRESSION_IDENTIFIER: {
            format(formatter, "{}", expression->identifier);
        } break;

        case EXPRESSION_UNARY_BOOLEAN_NOT: {
            format(formatter, "(not {})", expression->right);
        } break;

        case EXPRESSION_UNARY_MINUS: {
            format(formatter, "(- {})", expression->right);
        } break;

        case EXPRESSION_UNARY_BITWISE_NOT: {
            format(formatter, "(~ {})", expression->right);
        } break;

        case EXPRESSION_BINARY_ADDITION: {
            format(formatter, "(+ {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_SUBTRACTION: {
            format(formatter, "(- {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_BITWISE_OR: {
            format(formatter, "(| {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_BITWISE_XOR: {
            format(formatter, "(^ {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_COMPARISON_LESS_THAN: {
            format(formatter, "(< {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_COMPARISON_GREATER_THAN: {
            format(formatter, "(> {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_COMPARISON_LESS_THAN_OR_EQUAL_TO: {
            format(formatter, "(<= {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_COMPARISON_GREATER_THAN_OR_EQUAL_TO: {
            format(formatter, "(>= {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_COMPARISON_EQUALS: {
            format(formatter, "(== {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_COMPARISON_NOT_EQUALS: {
            format(formatter, "(!= {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_MULTIPLICATION: {
            format(formatter, "(* {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_DIVISION: {
            format(formatter, "(/ {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_BITWISE_AND: {
            format(formatter, "(& {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_SHIFT_LEFT: {
            format(formatter, "(<< {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_SHIFT_RIGHT: {
            format(formatter, "(>> {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_BOOLEAN_AND: {
            format(formatter, "(and {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BINARY_BOOLEAN_OR: {
            format(formatter, "(or {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_INVOCATION: {
            format(formatter, "(invocation {} {})", ((Expression_Invocation *)expression)->invoked, ((Expression_Invocation *)expression)->arguments);
        } break;

        case EXPRESSION_FIELD: {
            format(formatter, "(field {} {})", ((Expression_Field *)expression)->accessed, ((Expression_Field *)expression)->accessor);
        } break;

        case EXPRESSION_INDEX: {
            format(formatter, "(index {} {})", ((Expression_Index *)expression)->indexed, ((Expression_Index *)expression)->index);
        } break;

        case EXPRESSION_TRUE: {
            format(formatter, "true");
        } break;

        case EXPRESSION_FALSE: {
            format(formatter, "false");
        } break;

        case EXPRESSION_FUNCTION: {
            if (((Expression_Function *)expression)->return_type != NULL) {
                format(formatter, "(function {} {} {})", ((Expression_Function *)expression)->parameters, ((Expression_Function *)expression)->return_type, ((Expression_Function *)expression)->body);
            } else {
                format(formatter, "(function {} {})", ((Expression_Function *)expression)->parameters, ((Expression_Function *)expression)->body);
            }
        } break;

        case EXPRESSION_STRUCTURE: {
            format(formatter, "(structure {})", ((Expression_Structure *)expression)->fields);
        } break;

        case EXPRESSION_IF: {
            format(formatter, "(if {} ", ((Expression_If *)expression)->condition);
            format(formatter, " {}", ((Expression_If *)expression)->then_expression);
            if (((Expression_If *)expression)->else_expression) {
                format(formatter, " {}", ((Expression_If *)expression)->else_expression);
            }
            format(formatter, ")");
        } break;

        case EXPRESSION_FOR: {
            if (((Expression_For *)expression)->index != NULL) {
                format(formatter, "(for {} {} {} {})", ((Expression_For *)expression)->index, ((Expression_For *)expression)->value, ((Expression_For *)expression)->expression, ((Expression_For *)expression)->block);
            } else {
                format(formatter, "(for {} {} {})", ((Expression_For *)expression)->value, ((Expression_For *)expression)->expression, ((Expression_For *)expression)->block);
            }
        } break;

        case EXPRESSION_WHILE: {
            format(formatter, "(while {} {})", ((Expression_While *)expression)->condition, ((Expression_While *)expression)->block);
        } break;

        case EXPRESSION_RANGE: {
            format(formatter, "(.. {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_BLOCK: {
            format(formatter, "(block {})", ((Expression_Block *)expression)->expressions);
        } break;

        case EXPRESSION_ASSIGNMENT: {
            format(formatter, "(= {} {})", expression->left, expression->right);
        } break;

        case EXPRESSION_RETURN: {
            format(formatter, "(return {})", expression->expression);
        } break;

        case EXPRESSION_AS: {
            format(formatter, "({} as {})", expression->left, expression->right);
        } break;

        case EXPRESSION_ARRAY: {
            format(formatter, "(array {})", ((Expression_Array *)expression)->expressions);
        } break;

        case EXPRESSION_ADDRESS_OF: {
            format(formatter, "(& {})", expression->right);
        } break;

        case EXPRESSION_DEREFERENCE: {
            format(formatter, "(* {})", expression->right);
        } break;

        case EXPRESSION_VARIABLE_DECLARATION: {
            format(formatter, "({} : {} = {})", ((Expression_Declaration *)expression)->declared_name, ((Expression_Declaration *)expression)->declared_type, ((Expression_Declaration *)expression)->declared_value);
        } break;

        case EXPRESSION_FUNCTION_DECLARATION: {
            format(formatter, "({} :: {})", ((Expression_Declaration *)expression)->declared_name, ((Expression_Declaration *)expression)->declared_value);
        } break;

        case EXPRESSION_STRUCTURE_DECLARATION: {
            format(formatter, "({} :: {})", ((Expression_Declaration *)expression)->declared_name, ((Expression_Declaration *)expression)->declared_value);
        } break;

        case EXPRESSION_OPAQUE_DECLARATION: {
            format(formatter, "({} :: opaque)", ((Expression_Declaration *)expression)->declared_name);
        } break;

        case EXPRESSION_TYPE_POINTER: {
            format(formatter, "(& {})", expression->expression);
        } break;

        case EXPRESSION_TYPE_ARRAY: {
            format(formatter, "([] {})", expression->expression);
        } break;

        case EXPRESSION_TYPE_FUNCTION: {
            if (((Expression_Type_Function *)expression)->return_type != NULL) {
                format(formatter, "(function {} {})", ((Expression_Type_Function *)expression)->parameters, ((Expression_Type_Function *)expression)->return_type);
            } else {
                format(formatter, "(function {})", ((Expression_Type_Function *)expression)->parameters);
            }
        } break;

        case EXPRESSION_IMPORT: {
            format(formatter, "(import {})", expression->expression);
        } break;

        case EXPRESSION_LINKER: {
            format(formatter, "(linker {})", expression->expression);
        } break;
    }
}
