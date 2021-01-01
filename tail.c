/* tail.c -- a program to show the last lines of a file.
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: Fri Jan  1 02:23:49 EET 2021
 * Copyright: (C) 2021 Luis Colorado.  All rights reserved.
 * License: BSD.
 */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_LINES         10
#define MAX_LINE            1024

/* tracing macros */
#define F(_fmt) __FILE__":%d:%s: "_fmt, __LINE__, __func__
#define LOG(_fmt, ...) do {\
        fprintf(stderr, _fmt, ##__VA_ARGS__);\
    } while (0)

void process(char *name, FILE *f, int num_lines)
{
    char line[MAX_LINE];

    LOG(F("[%s]:\n"), name);
    char **last_lines = calloc(num_lines, sizeof(*last_lines));
    assert(last_lines != NULL);

    int n = 0;
    while (fgets(line, sizeof line, f)) {

        if (last_lines[n])
            free(last_lines[n]);        /* if reused, first free it */

        char *p = strtok(line, "\n");   /* eliminate last \n */
        if (!p) {                       /* empty line */
            p = "";
        }

        last_lines[n++] = strdup(p);    /* allocate memory for this
                                         * string */
        if (n == num_lines)             /* update n (and wrap if */

            n = 0;                      /* needed) */
    }

    /* now printing */
    if (last_lines[n]) {                /* we filled the array */
        while (last_lines[n]) {
            char *l = last_lines[n];
            printf("%s\n", l);
            free(l);                    /* free */
            last_lines[n++] = NULL;     /* set to NULL */
            if (n == num_lines)         /* wrap if necessary */
                n = 0;
        }
    } else {                            /* not filled */
        int i;
        for (i = 0; i < n; i++) {
            printf("%s\n", last_lines[i]);
            free(last_lines[i]);
            /* not necessary to set the pointer to NULL in this
             * case */
        }
    }
    free(last_lines);                   /* free the array */
}

int main(int argc, char **argv)
{
    int num_lines = DEFAULT_LINES;

    if (argc > 1) {
        int i;
        int num_files_processed = 0;
        for(i = 1; i < argc; i++) {     /* process each argument */
            if (argv[i][0] == '-') {
                num_lines = -atoi(argv[i]);
                if (num_lines <= 0)
                    num_lines = DEFAULT_LINES;
                continue;               /* set the num_lines and
                                         * continue */
            }
            /* it's a file */
            FILE *f = fopen(argv[i], "r");
            if (!f) {
                LOG(F("ERROR: %s: %s\n"),
                    argv[i], strerror(errno));
                exit(EXIT_FAILURE);
            }
            process(argv[i], f, num_lines);
            fclose(f);
            num_files_processed++;
        }
        if (num_files_processed)
            exit(EXIT_SUCCESS);
    }
    process("stdin", stdin, num_lines);
    exit(EXIT_SUCCESS);
}
