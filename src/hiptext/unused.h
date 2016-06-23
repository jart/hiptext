// Suppress unused variables in GCC and Clang.
//
// See: http://stackoverflow.com/a/12199209

#define MON_Internal_UnusedStringify(macro_arg_string_literal) #macro_arg_string_literal
#define MONUnusedParameter(macro_arg_parameter) _Pragma(MON_Internal_UnusedStringify(unused(macro_arg_parameter)))
