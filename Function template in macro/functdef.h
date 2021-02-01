#ifndef FUNCDEF_H_AVSOFT
#define FUNCDEF_H_AVSOFT

#define STRUCT_INSTANCE_NAME strinstance // How function parameter (a.k.a pointer to struct) is named in function definition
#define STRUCT_NAME a // the name of struct. Later should be pt_regs
#define CAST // if defined, works as if the pointer to struct is passed instead of arguments themself

#define STRUCTURE_FIELD1 c
#define STRUCTURE_FIELD2 d
#define STRUCTURE_FIELD3 y
#define STRUCTURE_FIELD4 x
#define STRUCTURE_FIELD5 t
#define STRUCTURE_FIELD6 g



// DEEP LOGIC OF HOW IT WORKS.

#define STRINGIZE(arg)  STRINGIZE1(arg)
#define STRINGIZE1(arg) STRINGIZE2(arg)
#define STRINGIZE2(arg) #arg

#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

#define SET_ARG_1(decl1, ...) \
    decl1 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD1;
#define SET_ARG_2(decl1, decl2, ...)\
    decl1 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD1;\
    decl2 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD2;
#define SET_ARG_3(decl1, decl2, decl3, ...)\
    decl1 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD1;\
    decl2 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD2;\
    decl3 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD3;
#define SET_ARG_4(decl1, decl2, decl3, decl4, ...)\
    decl1 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD1;\
    decl2 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD2;\
    decl3 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD3;\
    decl4 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD4;
#define SET_ARG_5(decl1, decl2, decl3, decl4, decl5, ...)\
    decl1 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD1;\
    decl2 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD2;\
    decl3 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD3;\
    decl4 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD4;\
    decl5 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD5;
#define SET_ARG_6(decl1, decl2, decl3, decl4, decl5, decl6, ...)\
    decl1 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD1;\
    decl2 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD2;\
    decl3 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD3;\
    decl4 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD4;\
    decl5 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD5;\
    decl6 = STRUCT_INSTANCE_NAME->STRUCTURE_FIELD6;

#define FUN_START_1(ret_val, function_name, arg1) ret_val function_name(arg1)
#define FUN_START_2(ret_val, function_name, arg1, arg2) ret_val function_name(arg1, arg2)
#define FUN_START_3(ret_val, function_name, arg1, arg2, arg3) ret_val function_name(arg1, arg2, arg3)
#define FUN_START_4(ret_val, function_name, arg1, arg2, arg3, arg4) ret_val function_name(arg1, arg2, arg3, arg4)
#define FUN_START_5(ret_val, function_name, arg1, arg2, arg3, arg4, arg5) ret_val function_name(arg1, arg2, arg3, arg4, arg5)
#define FUN_START_6(ret_val, function_name, arg1, arg2, arg3, arg4, arg5, arg6) ret_val function_name(arg1, arg2, arg3, arg4, arg5, arg6)

#define FOR_EACH_NARG(...) FOR_EACH_NARG_(__VA_ARGS__, FOR_EACH_RSEQ_N())
#define FOR_EACH_NARG_(...) FOR_EACH_ARG_N(__VA_ARGS__) 
#define FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N 
#define FOR_EACH_RSEQ_N() 16, 15,14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define FOR_EACH_(N, ...) CONCATENATE(SET_ARG_, N)(__VA_ARGS__)
#define FOR_EACH(...) FOR_EACH_(FOR_EACH_NARG(__VA_ARGS__), __VA_ARGS__)

#define FOR_EACHF_(ret_val, function_name, N, ...) CONCATENATE(FUN_START_, N)(ret_val, function_name, __VA_ARGS__)
#define FOR_EACH_FUNCTIONAL(ret_val, function_name, ...) FOR_EACHF_(ret_val, function_name, FOR_EACH_NARG(__VA_ARGS__), __VA_ARGS__)

// DEEP LOGIC END




// FUNCTION DEFINITION LOGIC

#ifdef CAST
#define FUNCTION_START(ret_val, function_name, ...)\
    ret_val function_name(struct STRUCT_NAME *STRUCT_INSTANCE_NAME) { \
    FOR_EACH(__VA_ARGS__)
#else
#define FUNCTION_START(ret_val, function_name, ...)\
    FOR_EACH_FUNCTIONAL(ret_val, function_name, __VA_ARGS__) {
#endif

#ifdef CAST
#define FUNCTION_START_HEADER(ret_val, function_name, ...)\
    ret_val function_name(struct STRUCT_NAME *STRUCT_INSTANCE_NAME)
#else
#define FUNCTION_START_HEADER(ret_val, function_name, ...)\
    FOR_EACH_FUNCTIONAL(ret_val, function_name, __VA_ARGS__)
#endif

#endif