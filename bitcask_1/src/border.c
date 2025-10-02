#include <stdio.h>
#include "../include/border.h"
/*border column 1 & 5*/
void border_1_5(const char *str, int i, const char *ch_border)
{
    int j = i > strlen(str) + 5 ? i : strlen(str) + 5;
    while (j-- > 0)
        printf("%s", ch_border);
    printf("\n");
}
/*border column 2 & 4*/
void border_2_4(const char *str, int i, const char *ch_border)
{
    i = i > strlen(str) + 5 ? i : strlen(str) + 5;
    int j = i;
    while (j-- > 0)
        printf("%s", j == i - 1 || j == 0 ? ch_border : " ");
    printf("\n");
}
/*border column 3*/
void border_3(const char *str, int i, const char *ch_border)
{
    int j = i > strlen(str) + 5 ? i : strlen(str) + 5;
    printf("%s  %s", ch_border, str);
    j -= strlen(str) + 5;
    while (j-- > 0)
        printf(" ");
    printf(" #\n");
}
void border_print_5_columns(const char *str, int i, const char *ch_border)
{
    border_1_5(str, i, ch_border);
    border_2_4(str, i, ch_border);
    border_3(str, i, ch_border);
    border_2_4(str, i, ch_border);
    border_1_5(str, i, ch_border);
}

void border_print_3_columns(const char *str, int i, const char *ch_border)
{
    border_1_5(str, i, ch_border);
    // border_2_4(str, i);
    border_3(str, i, ch_border);
    // border_2_4(str, i);
    border_1_5(str, i, ch_border);
}