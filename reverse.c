// reverse.c (OSTEP warm-up project)
//
// Reverses the order of input lines.
// Usage:
//   ./reverse
//   ./reverse input.txt
//   ./reverse input.txt output.txt
//
// Implementation notes:
// - Uses getline() to support arbitrarily long lines.
// - Stores each line in dynamically allocated memory so it can print in reverse.
// - On errors prints exact required messages to stderr and exits with code 1.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static void die_usage(void) {
    fprintf(stderr, "usage: reverse <input> <output>\n");
    exit(1);
}

static void die_malloc(void) {
    fprintf(stderr, "malloc failed\n");
    exit(1);
}

static void die_cannot_open(const char *fname) {
    fprintf(stderr, "error: cannot open file '%s'\n", fname);
    exit(1);
}

int main(int argc, char *argv[]) {
    // Argument handling:
    //  - no args: read from stdin, write to stdout
    //  - one arg: read from input file, write to stdout
    //  - two args: read input file, write to output file (must differ)
    //  - more args: print usage error

    FILE *in = NULL;
    FILE *out = NULL;

    const char *in_name = NULL;
    const char *out_name = NULL;

    if (argc == 1) {
        in = stdin;
        out = stdout;
    } else if (argc == 2) {
        in_name = argv[1];
        in = fopen(in_name, "r");
        if (!in) die_cannot_open(in_name);
        out = stdout;
    } else if (argc == 3) {
        in_name = argv[1];
        out_name = argv[2];

        if (strcmp(in_name, out_name) == 0) {
            fprintf(stderr, "Input and output file must differ\n");
            exit(1);
        }

        in = fopen(in_name, "r");
        if (!in) die_cannot_open(in_name);

        out = fopen(out_name, "w");
        if (!out) die_cannot_open(out_name);
    } else {
        die_usage();
    }

    size_t cap = 1024;
    size_t n = 0;
    char **lines = (char **)malloc(cap * sizeof(char *));
    if (!lines) die_malloc();

    char *buf = NULL;
    size_t buflen = 0;

    while (1) {
        errno = 0;
        ssize_t len = getline(&buf, &buflen, in);
        if (len == -1) {
            // EOF or error. For this project, we just stop on EOF.
            break;
        }

        char *copy = (char *)malloc((size_t)len + 1);
        if (!copy) die_malloc();
        memcpy(copy, buf, (size_t)len);
        copy[len] = '\0';

        if (n == cap) {
            size_t new_cap = cap * 2;
        // Grow the lines array when needed (file can be very long).
        // realloc may fail; handle it as required.

            char **tmp = (char **)realloc(lines, new_cap * sizeof(char *));
            if (!tmp) die_malloc();
            lines = tmp;
            cap = new_cap;
        }

        lines[n++] = copy;
    }

    free(buf);
    if (in != stdin) fclose(in);

    for (size_t i = n; i > 0; i--) {
        fprintf(out, "%s", lines[i - 1]);
        free(lines[i - 1]);
    }

    free(lines);
    if (out != stdout) fclose(out);

    return 0;
}
