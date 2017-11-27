#include "test3.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include<minix/drivers.h>

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup();
	sys_enable_iop(SELF);

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"scan <decimal no.- 0=>C, !=0=>ASM>\"\n"
			"\t service run %s -args \"leds <decimal no. array - 0=>ScrLk, 1=>NumLk, 2=>CapsLk\"\n"
			"\t service run %s -args \"timed_scan <decimal no. - time>\"\n",										//still to be modified
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if (argc != 3) {
			printf("kbd: wrong no. of arguments for kbd_test_scan()\n");
			return 1;
		}
		unsigned long lang= parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (lang == ULONG_MAX)
			return 1;
		printf("kbd::kbd_test_scan(%lu)\n", lang);
		return kbd_test_scan(lang);
	}
	else if (strncmp(argv[1], "leds", strlen("leds")) == 0) {
		if (argc < 3) {
			printf("kbd: wrong no. of arguments for kbd_test_leds()\n");
			return 1;
		}
		unsigned short *a = malloc(sizeof(unsigned short)*(argc-2));
		int i;
		for(i=0; i<(argc-2);i++)
		{
			a[i] = parse_ulong(argv[2+i], 10);
			if (a[i] == ULONG_MAX)
			return 1;				/* Parses string to unsigned long */
		}
		printf("kdb::kbd_test_leds(%lu, %s)\n", argc-2, "received array");
		return kbd_test_leds((argc-2), a);
	}
	else if (strncmp(argv[1], "timed_scan", strlen("timed_scan")) == 0) {
		if (argc != 3) {
			printf("kbd: wrong no of arguments for kbd_test_timed_scan()\n");
			return 1;
		}
		unsigned long time = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (time == ULONG_MAX)
			return 1;
		printf("kbd::kbd_test_timed_scan(%lu)\n", time);
		return kbd_test_timed_scan(time);									//Don't forget changing this!
	}
	else {
		printf("kbd: %s - no valid function!\n", argv[1]);
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
		printf("kdb: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
