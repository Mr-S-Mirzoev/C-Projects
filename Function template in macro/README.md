# Reason 

The reason behind this was the difference in system call structure after 4.17 version compared to earlier versions

## Limitations, initial requirements:

- The name of the structure is hardcoded into the STRUCT_NAME macro. For example, if we want to use the "foo" function with the "struct pt_regs" argument, we must declare in the header

        #define STRUCT_NAME pt_regs

- The name of the parameter that is the object of the STRUCT_NAME structure is declared in the STRUCT_INSTANCE_NAME macro. Default:

      #define STRUCT_INSTANCE_NAME strinstance

- Whether structure is used or explicit parameters are used is determined as specified or not specified macro CAST

- The order in which elements are defined in struct is defined via STRUCTURE_FIELD1 .. STRUCTURE_FIELD6 
        
        // For example, for struct a:

        struct a
        {
            int c;
            long d;
            char y;
            unsigned x;
            long long t;
        };
        
        // we would define macros as:

        #define STRUCTURE_FIELD1 c
        #define STRUCTURE_FIELD2 d
        #define STRUCTURE_FIELD3 y
        #define STRUCTURE_FIELD4 x
        #define STRUCTURE_FIELD5 t
        #define STRUCTURE_FIELD6 g

## EXAMPLE

Let's say we have defined the following macros:

        #define STRUCT_INSTANCE_NAME strinstance
        #define STRUCT_NAME pt_regs
        #define CAST

And we defined the function like this:

        FUNCTION_START (void, helloworld, int a, int b)
            printf ("% d% d \ n", a, b);
            return;
        }

Then, the preprocessor will expand this code into the following:
        
        void helloworld (struct a * strinstance) {int a = strinstance-> c; int b = strinstance-> d;
