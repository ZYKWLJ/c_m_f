#include <stdio.h>
#include <time.h>

#define LOG_
#ifdef LOG_
// #define LOG(msg) printf("[LOG %s %s %s:%d] %s\n" __DATE__, __TIME__, __FILE__, __LINE__, msg)
#define LOG(...)                                                         \
    printf("[LOG ][%s %s %s:%d] ", __DATE__, __TIME__, __FILE__, __LINE__); \
    printf(__VA_ARGS__);                                                 \
    printf("\n");
#else
#define LOG(...) \
    do           \
    {            \
    } while (0);
#endif

#define TODO_
#ifdef TODO_
#define TODO(...)                                                         \
    printf("[TODO][%s %s %s:%d] ", __DATE__, __TIME__, __FILE__, __LINE__); \
    printf(__VA_ARGS__);                                                  \
    printf("\n");
#else
#define TODO(...) \
    do            \
    {             \
    } while (0);
#endif

// #include <stdio.h>

// int main(void) {
//     LOG("hello world")
//     return 0;
// }
