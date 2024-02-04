# Lexer

A simple lexer written in C that tokenizes input code into different categories such as keywords, operators, identifiers, strings, characters, and numeric values.

## Features

- Tokenizes input code into various categories.
- Recognizes keywords, operators, identifiers, strings, characters, and numeric values.
- Handles single-line and multi-line comments.

## Sample Input with C-Like Language

```c
/*
 * Hello world with args.
 */
proc main(argc: integer; argv: string_vector_type)
{
   printf("Hello, world\n");
   {:
      argc := argc - 1;
      exit when (argc = 0);
      printf("arg @ %d: %s\n", argc, argv @ argc);
   }:
};
```

### Produces
```
Comment: /*
Comment:  * Hello world with args.
Comment:  */
Keyword: proc
Identifier: main
Operator: (
Identifier: argc
Operator: :
Keyword: integer
Operator: ;
Identifier: argv
Operator: :
Identifier: string_vector_type
Operator: )
Operator: {
Identifier: printf
Operator: (
String: "Hello, world\n"
Operator: )
Operator: ;
Operator: {:
Identifier: argc
Operator: :=
Identifier: argc
Operator: -
Numeric: 1
Operator: ;
Keyword: exit
Keyword: when
Operator: (
Identifier: argc
Operator: =
Numeric: 0
Operator: )
Operator: ;
Identifier: printf
Operator: (
String: "arg @ %d: %s\n"
Operator: ,
Identifier: argc
Operator: ,
Identifier: argv
Operator: @
Identifier: argc
Operator: )
Operator: ;
Operator: }:
Operator: }
Operator: ;
```
