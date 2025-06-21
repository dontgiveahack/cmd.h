# cmd.h

A tiny, single-header, zero-allocation command line parser in C99.

## Features

- **Single header**: Just include `cmd.h` and you're ready to go
- **Zero allocations**: No malloc/free, everything uses stack memory
- **C99 compatible**: Works with any C99 compliant compiler
- **Git-like interface**: Support for subcommands and options
- **Multiple option types**: Flags, strings, and integers
- **Short and long options**: `-f` and `--flag` both supported
- **Error handling**: Clear error reporting for invalid usage

## Quick Start

```c
#include "cmd.h"

void cmd_hello(int argc, char **argv) {
    CMD_Opt opts[] = {
        { .sname = 'n', .lname = "name",    .type = CMD_OPT_STR  },
        { .sname = 'v', .lname = "verbose", .type = CMD_OPT_FLAG },
    };
    
    if (cmd_parse_options(argc, argv, opts, 2) != CMD_PARSE_OK) {
        printf("Invalid options\n");
        return;
    }
    
    const char *name = opts[0].str_val ? opts[0].str_val : "Default";
    printf("Hello, %s!\n", name);
    
    if (opts[1].is_provided) {
        printf("Verbose mode enabled\n");
    }
}

CMD_Cmd commands[] = {
    { "hello", cmd_hello },
    { NULL, NULL },
};

int main(int argc, char *argv[]) {
    if (argc < 2 || !cmd_dispatch(argc, argv, commands)) {
        printf("Usage: %s <command> [options]\n", argv[0]);
        return 1;
    }
    return 0;
}
```

## Usage Examples

```bash
# Flag option
./program hello --verbose
./program hello -v

# String option
./program hello --name="Alice"
./program hello --name Alice
./program hello -n Alice

# Integer option
./program foo --number=42
./program foo -n 42
```

## API Reference

### Option Types

```c
typedef enum {
    CMD_OPT_FLAG,  // Boolean flag (--verbose, -v)
    CMD_OPT_STR,   // String value (--name="value", -n value)
    CMD_OPT_INT,   // Integer value (--count=5, -c 5)
} CMD_OptType;
```

### Option Structure

```c
typedef struct {
    char sname;          // Short option name ('f' for -f)
    const char *lname;   // Long option name ("flag" for --flag)
    CMD_OptType type;    // Option type
    const char *help;    // Help description (currently unused)
    int is_provided;     // Set to 1 if option was provided
    int int_val;         // Integer value (for CMD_OPT_INT)
    const char *str_val; // String value (for CMD_OPT_STR)
} CMD_Opt;
```

### Command Structure

```c
typedef struct {
    const char *name;                  // Command name
    void (*fn)(int argc, char **argv); // Command function
} CMD_Cmd;
```

### Parse Results

```c
typedef enum {
    CMD_PARSE_OK,           // Parsing successful
    CMD_PARSE_UNKNOWN_OPT,  // Unknown option provided
    CMD_PARSE_MISSING_VAL,  // Option requires value but none provided
    CMD_PARSE_INVALID_VAL,  // Invalid value for integer option
} CMD_ParseResult;
```

### Core Functions

#### `cmd_parse_options(int argc, char **argv, CMD_Opt *opts, int optc)`

Parse command line options and populate the options array.

- `argc, argv`: Command line arguments
- `opts`: Array of option definitions
- `optc`: Number of options in the array
- Returns: `CMD_ParseResult` indicating success or error type

#### `cmd_dispatch(int argc, char **argv, const CMD_Cmd *commands)`

Find and execute the appropriate command.

- `argc, argv`: Command line arguments
- `commands`: Null-terminated array of commands
- Returns: 1 if command found and executed, 0 otherwise

## Configuration

### Maximum Options

By default, the library supports up to 64 options. You can change this by defining `CMD_MAX_OPTIONS` before including the header:

```c
#define CMD_MAX_OPTIONS 32
#include "cmd.h"
```

## Error Handling

The parser provides detailed error information:

```c
CMD_ParseResult res = cmd_parse_options(argc, argv, opts, optc);
switch (res) {
case CMD_PARSE_OK:           /* Success */ break;
case CMD_PARSE_UNKNOWN_OPT:  printf("Unknown option\n"); break;
case CMD_PARSE_MISSING_VAL:  printf("Missing value\n"); break;
case CMD_PARSE_INVALID_VAL:  printf("Invalid value\n"); break;
}
```

## Design Philosophy

- **Simplicity**: Minimal API surface with clear semantics
- **Efficiency**: Zero dynamic allocations, minimal memory footprint
- **Portability**: Pure C99, no external dependencies
- **Readability**: Clean, well-commented code that's easy to understand

## Limitations

- No support for combined short options (e.g., `-abc` for `-a -b -c`)
- No built-in help generation (you implement your own)
- No support for optional arguments
- No support for variadic arguments
- String values point to original argv memory (no copying)

## License

`cmd.h` is published under a ISC License. See [LICENSE](LICENSE) file for copyright and license details.

## Contributing

This is meant to be a simple, focused library. If you need more features, consider forking or using a more full-featured argument parsing library.
