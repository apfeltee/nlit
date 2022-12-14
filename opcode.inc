OPCODE(POP, -1)
OPCODE(RETURN, 0)
OPCODE(CONSTANT, 1)
OPCODE(CONSTANT_LONG, 1)
OPCODE(TRUE, 1)
OPCODE(FALSE, 1)
OPCODE(NULL, 1)
OPCODE(ARRAY, 1)
OPCODE(OBJECT, 1)
OPCODE(RANGE, -1)
OPCODE(NEGATE, 0)
OPCODE(NOT, 0)
OPCODE(ADD, -1)
OPCODE(SUBTRACT, -1)
OPCODE(MULTIPLY, -1)
OPCODE(POWER, -1)
OPCODE(DIVIDE, -1)
OPCODE(FLOOR_DIVIDE, -1)
OPCODE(MOD, -1)
OPCODE(BAND, -1)
OPCODE(BOR, -1)
OPCODE(BXOR, -1)
OPCODE(LSHIFT, -1)
OPCODE(RSHIFT, -1)
OPCODE(BNOT, 0)
OPCODE(EQUAL, -1)
OPCODE(GREATER, -1)
OPCODE(GREATER_EQUAL, -1)
OPCODE(LESS, -1)
OPCODE(LESS_EQUAL, -1)
OPCODE(SET_GLOBAL, 0)
OPCODE(GET_GLOBAL, 1)
OPCODE(SET_LOCAL, 0)
OPCODE(GET_LOCAL, 1)
OPCODE(SET_LOCAL_LONG, 0)
OPCODE(GET_LOCAL_LONG, 1)
OPCODE(SET_PRIVATE, 0)
OPCODE(GET_PRIVATE, 1)
OPCODE(SET_PRIVATE_LONG, 0)
OPCODE(GET_PRIVATE_LONG, 1)
OPCODE(SET_UPVALUE, 0)
OPCODE(GET_UPVALUE, 1)
OPCODE(JUMP_IF_FALSE, -1)
OPCODE(JUMP_IF_NULL, 0)
OPCODE(JUMP_IF_NULL_POPPING, -1)
OPCODE(JUMP, 0)
OPCODE(JUMP_BACK, 0)
OPCODE(AND, -1)
OPCODE(OR, -1)
OPCODE(NULL_OR, -1)
OPCODE(CLOSURE, 1)
OPCODE(CLOSE_UPVALUE, -1)
OPCODE(CLASS, 1)
OPCODE(GET_FIELD, -1)
OPCODE(SET_FIELD, -2)
// [array] [index] -> [value]
OPCODE(SUBSCRIPT_GET, -1)
// [array] [index] [value] -> [value]
OPCODE(SUBSCRIPT_SET, -2)
// [array] [value] -> [array]
OPCODE(PUSH_ARRAY_ELEMENT, -1)
// [map] [slot] [value] -> [map]
OPCODE(PUSH_OBJECT_FIELD, -2)
// [class] [method] -> [class]
OPCODE(METHOD, -1)
// [class] [method] -> [class]
OPCODE(STATIC_FIELD, -1)
OPCODE(DEFINE_FIELD, -1)
OPCODE(INHERIT, 0)
// [instance] [class] -> [bool]
OPCODE(IS, -1)
OPCODE(GET_SUPER_METHOD, 0)
// Varying stack effect
OPCODE(CALL, 0)
OPCODE(INVOKE, 0)
OPCODE(INVOKE_SUPER, 0)
OPCODE(INVOKE_IGNORING, 0)
OPCODE(INVOKE_SUPER_IGNORING, 0)
OPCODE(POP_LOCALS, 0)
OPCODE(VARARG, 0)
OPCODE(REFERENCE_GLOBAL, 1)
OPCODE(REFERENCE_PRIVATE, 0)
OPCODE(REFERENCE_LOCAL, 1)
OPCODE(REFERENCE_UPVALUE, 1)
OPCODE(REFERENCE_FIELD, -1)
OPCODE(SET_REFERENCE, -1)