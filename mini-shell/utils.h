/*******************************************************************************
* Author:
*   @coronapl
* Program:
*   mini-shell
*******************************************************************************/

#ifndef MINI_SHELL_UTILS_H
#define MINI_SHELL_UTILS_H

#include <ctype.h>

/**
 * Trim white spaces.
 * @param str
 */
void trim(char *str) {
    int i;
    int begin = 0;
    int end = strlen(str) - 1;

    while (isspace((unsigned char) str[begin]))
        begin++;

    while ((end >= begin) && isspace((unsigned char) str[end]))
        end--;

    for (i = begin; i <= end; i++)
        str[i - begin] = str[i];

    str[i - begin] = '\0';
}

/**
 * Separate string into array using a delimiter.
 * @param str
 * @param result
 * @param delim
 */
void parse_str(char *str, char **result, char *delim) {
    char *copy, *tok;
    int i = 0;

    copy = strdup(str);

    while ((tok = strsep(&copy, delim)) != NULL) {
        trim(tok);
        if (strlen(tok) != 0) {
            result[i] = tok;
            i++;
        }
    }

    result[i] = NULL;
    free(copy);
}

#endif //MINI_SHELL_UTILS_H
