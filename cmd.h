/* cmd.h
 * Single-header, zero-allocation command and arguments parsing library
 * See LICENSE file for copyright and license details
 */

#ifndef CMD_H
#define CMD_H

#include <string.h>
#include <stdlib.h>

/* Maximum number of options that can be parsed */
#ifndef CMD_MAX_OPTIONS
#define CMD_MAX_OPTIONS 64
#endif

/* Option types */
typedef enum {
	CMD_OPT_FLAG, /* Flag option */
	CMD_OPT_STR,  /* String option */
	CMD_OPT_INT,  /* Integer option */
} CMD_OptType;

/* Option structure */
typedef struct {
	char sname;          /* Short option name (e.g: 'f' for -f) */
	const char *lname;   /* Long option name (e.g: "flag" for --flag) */
	CMD_OptType type;    /* Option type */
	const char *help;    /* Help description text */
	int is_provided;     /* Whether option was provided */
	int int_val;         /* Integer value (for CMD_OPT_INT) */
	const char *str_val; /* String value (for CMD_OPT_STR) */
} CMD_Opt;

/* Command structure */
typedef struct {
	const char *name;                  /* Command name */
	void (*fn)(int argc, char **argv); /* Command function pointer */
} CMD_Cmd;

/* Parser result for error handling */
typedef enum {
	CMD_PARSE_OK,
	CMD_PARSE_UNKNOWN_OPT,
	CMD_PARSE_MISSING_VAL,
	CMD_PARSE_INVALID_VAL,
} CMD_ParseResult;

/* find option by short name */
static CMD_Opt *
cmd_find_short_opt(char sname, CMD_Opt *opts, int optc)
{
	for (int i = 0; i < optc; i++) {
		if (opts[i].sname == sname) {
			return &opts[i];
		}
	}
	return NULL;
}

/* find option by long name */
static CMD_Opt *
cmd_find_long_opt(const char *lname, CMD_Opt *opts, int optc)
{
	for (int i = 0; i < optc; i++) {
		if (opts[i].lname && strcmp(opts[i].lname, lname) == 0) {
			return &opts[i];
		}
	}
	return NULL;
}

/* Check if string is a valid integer */
static int
cmd_is_valid_int(const char *str)
{
	if (!str || *str == '\0') return 0;

	const char *p = str;
	if (*p == '-' || *p == '+') p++; // skip sign
	while (*p) {
		if (*p < '0' || *p > '9') return 0;
		p++;
	}

	return 1;
}

/* Parse comamnd line options
 * Modifies the options array in-place, setting present and values
 * Returns: parse result status
 */
static CMD_ParseResult
cmd_parse_options(int argc, char **argv, CMD_Opt *opts, int optc)
{
	// Reset all options
	for (int i = 0; i < optc; i++) {
		opts[i].is_provided = 0;
		opts[i].str_val = NULL;
		opts[i].int_val = 0;
	}

	// note: skip program name and command name
	for (int i = 2; i < argc; i++) {
		char *arg = argv[i];
		CMD_Opt *opt = NULL;
		const char *val = NULL;

		// Long option
		if (strncmp(arg, "--", 2) == 0) {
			char *eq_pos = strchr(arg + 2, '=');
			if (eq_pos) {
				*eq_pos = '\0';
				val = eq_pos + 1;
			}

			if (!(opt = cmd_find_long_opt(arg + 2, opts, optc)))
				return CMD_PARSE_UNKNOWN_OPT;

			if (eq_pos)
				*eq_pos = '='; // restore original string

			// if no value found with =, check next argument
			if (!val && opt->type != CMD_OPT_FLAG) {
				if (i + 1 < argc && argv[i + 1][0] != '-') {
					val = argv[++i];
				} else {
					return CMD_PARSE_MISSING_VAL;
				}
			}

		// Short option
		} else if (arg[0] == '-' && arg[1] != '\0') {
			char short_opt = arg[1];
			if (!(opt = cmd_find_short_opt(short_opt, opts, optc)))
				return CMD_PARSE_UNKNOWN_OPT;

			if (opt->type != CMD_OPT_FLAG) {
				// value attached: -svalue
				if (arg[2] != '\0') {
					val = arg + 2;

				// value in next argument: -s value
				} else if (i + 1 < argc && argv[i + 1][0] != '-') {
					val = argv[++i];

				// missing value
				} else {
					return CMD_PARSE_MISSING_VAL;
				}
			}
		}

		opt->is_provided = 1;
		switch (opt->type) {
		case CMD_OPT_FLAG: break;
		case CMD_OPT_STR:
			opt->str_val = val;
			break;
		case CMD_OPT_INT:
			if (!val || !cmd_is_valid_int(val)) {
				return CMD_PARSE_INVALID_VAL;
			}
			opt->int_val = atoi(val);
			break;
		}
	}

	return CMD_PARSE_OK;
}

/* Find command by name */
static const CMD_Cmd *
cmd_find_command(const char *name, const CMD_Cmd *commands)
{
	for (int i = 0; commands[i].name != NULL; i++) {
		if (strcmp(commands[i].name, name) == 0) {
			return &commands[i];
		}
	}
	return NULL;
}

/* Dispatch command based on name */
static int
cmd_dispatch(int argc, char **argv, const CMD_Cmd *commands)
{
	const CMD_Cmd *cmd = cmd_find_command(argv[1], commands);
	if (!cmd) return 0;

	cmd->fn(argc, argv);
	return 1;
}

#endif /* CMD_H */
