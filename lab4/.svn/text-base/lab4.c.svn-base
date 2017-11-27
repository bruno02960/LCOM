#include "test4.h"
#include <limits.h>
#include <string.h>
#include <errno.h>

#include <minix/drivers.h>



#include <stdio.h>

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup();

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"packet <decimal no.- packets>\"\n"
			"\t service run %s -args \"async <decinal no. - time>\"\n"
			"\t service run %s -args \"config\"\n"
			"\t service run %s -args \"gesture <decinal no. - length>\"\n",
			argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	if (strncmp(argv[1], "packet", strlen("packet")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for test_packet\n");
			return 1;
		}
		unsigned long cnt= parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (cnt == ULONG_MAX)
			return 1;
		printf("mouse::test_packet(%lu)\n", cnt);
		return test_packet(cnt);
	}
	else if (strncmp(argv[1], "async", strlen("async")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for test_async()\n");
			return 1;
		}
		unsigned long idle_time= parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (idle_time == ULONG_MAX)
			return 1;
		printf("mouse::test_async(%lu)\n", idle_time);
		return test_async(idle_time);
	}
	else if (strncmp(argv[1], "config", strlen("config")) == 0) {
		if (argc != 2) {
			printf("mouse: wrong no of arguments for test_config()\n");
			return 1;
		}
		printf("mouse::test_config()\n");
		return test_config();															//Don't forget changing this!
	}
	else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no of arguments for test_gesture()\n");
			return 1;
		}
		unsigned long length = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (length == ULONG_MAX)
			return 1;
		printf("mouse::test_gesture(%lu)\n", length);
		return test_gesture(length);															//Don't forget changing this!
	}
	else {
		printf("mouse: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base)
{
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("mouse: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
