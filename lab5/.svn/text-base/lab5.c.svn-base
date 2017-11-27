#include "test5.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "pixmap.h"
#include <minix/drivers.h>



#include <stdio.h>

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);
static unsigned short parse_ushort(char *str, int base);

int main(int argc, char **argv)
{
	sef_startup();

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char *argv[]) {
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"init <mode, delay>\" \n"
			"\t service run %s -args \"square <x, y, size, color>\" \n"
			"\t service run %s -args \"line <xi,yi,xf,yf,color>\" \n"
			"\t service run %s -args \"xpm <xi,yi, xpm [0..4]>\" \n"
			"\t service run %s -args \"move <xi, yi,  xpm [0..4], hor, delta, time>\" \n"
			"\t service run %s -args \"controller\" \n" ,
			argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if (argc != 4) {
			printf("video: wrong no. of arguments for test_init\n");
			return 1;
		}
		unsigned long mode= parse_ulong(argv[2], 16);
		unsigned long delay= parse_ulong(argv[3], 10);
		if (mode == USHRT_MAX||delay==USHRT_MAX)
			return 1;
		printf("video::test_init(%d,%d)\n", mode,delay);
		test_init(mode,delay);
	}
	else if (strncmp(argv[1], "square", strlen("square")) == 0) {
		if (argc != 6) {
			printf("video: wrong no. of arguments for test_square()\n");
			return 1;
		}
		unsigned long x= parse_ulong(argv[2], 10);
		unsigned long y= parse_ulong(argv[3], 10);
		unsigned long size= parse_ulong(argv[4], 10);
		unsigned long color= parse_ulong(argv[5], 10);
		if (x == USHRT_MAX||y == USHRT_MAX||
				size == USHRT_MAX||color == USHRT_MAX)
			return 1;
		printf("video::test_square(%d,%d,%d,%d)\n",
				x,y,size,color);
		return test_square(x,y,size,color);
	}
	else if (strncmp(argv[1], "line", strlen("line")) == 0) {
		if (argc != 7) {
			printf("video: wrong no of arguments for test_line()\n");
			return 1;
		}
		unsigned long xi= parse_ulong(argv[2], 10);
		unsigned long yi= parse_ulong(argv[3], 10);
		unsigned long xf= parse_ulong(argv[4], 10);
		unsigned long yf= parse_ulong(argv[5], 10);
		unsigned long color= parse_ulong(argv[6],
				16);
		if (xi == USHRT_MAX||yi == USHRT_MAX||
				xf == USHRT_MAX||yf == USHRT_MAX
				|| color == USHRT_MAX)
			printf("video::test_line(%d,%d,%,d,%d,"
					"%d)\n",xi,yi,xf,yf,color);
		return test_line(xi,yi,xf,yf,color);
	}
	else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0) {
		if (argc !=5) {
			printf("video: wrong no of arguments for"
					" test_xpm()\n");
			return 1;
		}
		unsigned long xi = parse_ulong(argv[2], 10);
		unsigned long yi = parse_ulong(argv[3], 10);
		unsigned long xpm = parse_ushort(argv[4], 10);
		if (xi == USHRT_MAX||yi==USHRT_MAX
						||xpm==USHRT_MAX)
					return 1;
		char** fig;
		fig=getFig(xpm);
		printf("video:: test_xpm(%d, %d, pixmap[%d])\n",
				xi, yi, xpm);
		return test_xpm(xi, yi, fig);
	}
	else if (strncmp(argv[1], "move", strlen("move")) == 0) {
		if (argc !=8) {
			printf("video: wrong no of arguments for test_move()\n");
			return 1;
		}
		unsigned long xi = parse_ulong(argv[2], 10);
		unsigned long yi = parse_ulong(argv[3], 10);
		unsigned long xpm = parse_ushort(argv[4], 10);
		if (xi == USHRT_MAX||yi==USHRT_MAX
				||xpm==USHRT_MAX)
			return 1;
		char** fig;
				fig=getFig(xpm);
		unsigned long hor = parse_ulong(argv[5], 10);
		unsigned long delta = parse_ulong(argv[6], 10);
		unsigned long time = parse_ushort(argv[7], 10);

		if(hor == USHRT_MAX || delta==USHRT_MAX
				|| time==USHRT_MAX)
				return 1;

		printf("video:: test_move(%d, %d, pixmap[%d], %d, %d, %d)\n",
				xi, yi, xpm, hor, delta, time);
		return test_move(xi, yi, fig, hor, delta, time);
	}
	else if (strncmp(argv[1], "controller", strlen("controller")) == 0) {
			if( argc != 2 ) {
				printf("video: wrong no of arguments for test of test_controller() \n");
				return 1;
			}
			printf("video:: test_controller()\n");
			return test_controller();
	}
	else {
		printf("video: %s - no valid function!\n", argv[1]);
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
		printf("video: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

static unsigned short parse_ushort(char *str, int base)
{
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == USHRT_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return USHRT_MAX;
	}

	if (endptr == str) {
		printf("video: parse_ulong: no digits were found in %s\n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}
