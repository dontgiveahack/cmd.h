/* See LICENSE file for copyright and license details. */

#include <stdio.h>

#include "cmd.h"

void
cmd_foo(int argc, char **argv)
{
	CMD_Opt opts[] = {
		{ .sname = 'f', .lname = "flag",   .type = CMD_OPT_FLAG },
		{ .sname = 's', .lname = "string", .type = CMD_OPT_STR  },
		{ .sname = 'n', .lname = "number", .type = CMD_OPT_INT  },
	};

	CMD_ParseOut out = cmd_parse_options(argc, argv, opts, 3);
	if (out.res != CMD_PARSE_OK) {
		switch (out.res) {
		case CMD_PARSE_UNKNOWN_OPT: printf("Error: Unknown option\n"); break;
		case CMD_PARSE_MISSING_VAL: printf("Error: Missing option value\n"); break;
		case CMD_PARSE_INVALID_VAL: printf("Error: Invalid option value\n"); break;
		default:                    printf("Unknown error\n"); break;
		}
		return;
	}

	printf("Executing foo command\n");

	// flag
	if (opts[0].is_provided) {
		printf("Flag is set!\n");
	}

	// string
	const char *str_val = opts[1].str_val ? opts[1].str_val : "default";
	printf("String value: %s\n", str_val);

	// number
	if (opts[2].is_provided) {
		int num_val = opts[2].int_val;
		printf("Number value: %d\n", num_val);
	}

	// positionals
	if (out.positionalc > 0) {
		printf("Positional arguments:\n");
		for (int i = 0; i < out.positionalc; i++) {
			printf("\t[%d] %s\n", i, out.positionals[i]);
		}
	}
}

void
cmd_help()
{
	printf("Usage: program <command> [options]\n\n");
	printf("Commands:\n");
	printf("  foo   Example command with various options and positionals\n");
	printf("  help  Show this message\n");
}

CMD_Cmd commands[] = {
	{ "foo",  cmd_foo  },
	{ "help", cmd_help },
	{  NULL,  NULL     },
};

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		cmd_help();
		return 1;
	}

	if (!cmd_dispatch(argc, argv, commands)) {
		printf("Unknown command: %s\n", argv[1]);
		cmd_help();
		return 1;
	}

	return 0;
}
