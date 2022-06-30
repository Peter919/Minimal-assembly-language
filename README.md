Thanks for opening this page :)

# Minimal assembly language
This is the C implementation for *minimal assembly language*, a low-level esoteric programming language I've designed and implemented in C. For more information about the language itself, visit this page: https://esolangs.org/wiki/Minimal_assembly_language

## Usage
This program will first translate all the .masm (the file extension for minimal assembly files) files in a directory to a .mexe file, which is easier for the computer to decode and run. Whether you want to translate a directory or run a .mexe file, you must run main.c with the first argument being the full path to a file within the directory. The second argument must be "c" if you want to translate a directory, "r" if you want to run a .mexe file and "cr" if you want to do both.

## How it works
The program has two largely independent parts: The "compiler" (probably doesn't quite fit the definition) and the runner. The compiler translates a directory of .masm files to a .mexe file, and the runner runs a .mexe file.

### Compiler
The compiler goes through four different steps to translate a directory to a .mexe file. First, it finds all the MASM files in a directory and initializes a list of ```struct MasmFile```s. These includes all the information needed about the different minimal assembly files, like their name, labels, and contents. Most of the compilation time is spent adding information to them.

The second step is the lexical analysis. Here, the contents of each MASM file is converted to a list of tokens. These tokens are the fundemental building blocks of a file. All of them have types (like comment, whitespace or identifier), and the ones that need it also have values. For example,
```
cmt sets "tmp"
set tmp
```
is converted to
```
{COMMENT, NEWLINE, INSTRUCTION (value = 1), WHITESPACE, IDENTIFIER (value = "tmp")}
```
The value of ```INSTRUCTION``` equals ```1``` because the instruction ```set``` has ID 1.

The third step is the parsing. Here, the ```struct MasmFile```s get finished. The order of the tokens are checked for syntax errors (like having two commands in the same line), labels imported from other files are found and identifiers are converted to indices in a *symbol table* (array of variables).

The fourth and final step of the compilation is the translation. Here, all ```struct MasmFile```s are converted to a single buffer with all the information needed to run a file. The buffer has the following structure:
```
repeat as many times as there is MASM files in the directory (
        repeat as many times as there are imports in the current MASM file (
                (import file index -- 2 bytes)
                (import label index -- 2 bytes)
                (import local symbol table index -- 1 byte)
        )
        (0xffff -- 2 bytes, shows that there are no more imports in the current MASM file)
        
        repeat as many times as there are labels in the current MASM file (
                (label length -- 2 bytes)
                (label contents)
        )
        (0xffff --2 bytes, shows that there are no more labels in the current MASM file)
)
```
This might be a little difficult to read, but to simplify, each masm file store the position of the labels it has imported first, and then its own labels. Each imported label stores a file index (the index of the file that the label is imported from), a label index (the index of the label within the file) and a local symbol table index (the ID of the variable that is going to hold the value of the label). A file index of 0xffff (65535) marks the end of the imports in a file. After the end of the imports, the file's own labels are declared. First, two bytes are used to store the length of the label. The actual contents of the label are stored afterwards. A label length of 0xffff marks the end of a file and the beginning of the next file's imports. The MASM file list is then deleted, the entire buffer will be written to a .mexe file, and the compilation is completed.

### Runner
The runner does three different things. First, it converts a .mexe file to a buffer and converts that buffer to a list of ```struct MexeFilePart```s (the translated equivalent to a single MASM file). These files stores their own local symbol table as well as a list of their labels and their contents. Imported labels will be pointed to by elements in these symbol tables.

Second, it creates an array of local symbol tables as well as a single global one. The local symbol tables are an array of memory, copied from each ```struct MexeFilePart``` in the mexe file part list. Memory is allocated for the global symbol table too, and most of the built-in variables in the global symbol table (like ```ipt``` and ```out```) will be initialized.

Finally, execution starts in the main label and uses the global and local symbol tables to execute code. This is done in a single file, ```execute_main.c``` and is pretty straight-forward.
