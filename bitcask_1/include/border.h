#ifndef BORDER_H_
#define BORDER_H_
#include <string.h>

/*border column 1 & 5*/
void border_1_5(const char *str, int i, const char *ch_border);
/*border column 2 & 4*/
void border_2_4(const char *str, int i, const char *ch_border);
/*border column 3*/
void border_3(const char *str, int i, const char *ch_border);
void border_print_5_columns(const char *str, int i, const char *ch_border);
void border_print_3_columns(const char *str, int i, const char *ch_border);

#endif /* BORDER_H_ */