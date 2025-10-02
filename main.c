#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ht.h"

// string-
// Custom structure
typedef struct
{
    int x;
    int y;
} point_t;

// Custom free functions
void free_point(void *ptr)
{
    printf("Freeing point\n");
    free(ptr);
}

void free_string(void *ptr)
{
    printf("Freeing string: %s\n", (char *)ptr);
    free(ptr);
}

int main()
{
    hash_table_t *str_to_int_table = hash_table_create(
        100,
        hash_func_string,
        key_compare_string,
        free_string, // Auto-free keys
        NULL         // No special free needed for values
    );

#if 0
    printf("=== Generic Hash Table Example ===\n\n");

    // Example 1: String to integer mapping
    printf("1. String to integer mapping:\n");
    hash_table_t *str_to_int_table = hash_table_create(
        100,
        hash_func_string,
        key_compare_string,
        free_string,  // Auto-free keys
        NULL          // No special free needed for values
    );

    // Insert data
    char *key1 = strdup("apple");
    char *key2 = strdup("banana");
    char *key3 = strdup("cherry");
    
    int value1 = 100, value2 = 200, value3 = 300;
    
    hash_table_insert(str_to_int_table, key1, strlen(key1) + 1, &value1, sizeof(int));
    hash_table_insert(str_to_int_table, key2, strlen(key2) + 1, &value2, sizeof(int));
    hash_table_insert(str_to_int_table, key3, strlen(key3) + 1, &value3, sizeof(int));

    // Lookup data
    int *result = hash_table_lookup(str_to_int_table, "apple", strlen("apple") + 1);
    if (result) {
        printf("apple -> %d\n", *result);
    }

    result = hash_table_lookup(str_to_int_table, "banana", strlen("banana") + 1);
    if (result) {
        printf("banana -> %d\n", *result);
    }

    hash_table_destroy(str_to_int_table);
    printf("\n");

    // Example 2: Integer to string mapping
    printf("2. Integer to string mapping:\n");
    hash_table_t *int_to_str_table = hash_table_create(
        50,
        hash_func_int,
        key_compare_int,
        NULL,         // Integer keys don't need freeing
        free_string   // String values need freeing
    );

    int keys[] = {1, 2, 3};
    char *val1 = strdup("one");
    char *val2 = strdup("two");
    char *val3 = strdup("three");

    hash_table_insert(int_to_str_table, &keys[0], sizeof(int), val1, strlen(val1) + 1);
    hash_table_insert(int_to_str_table, &keys[1], sizeof(int), val2, strlen(val2) + 1);
    hash_table_insert(int_to_str_table, &keys[2], sizeof(int), val3, strlen(val3) + 1);

    char *str_result = hash_table_lookup(int_to_str_table, &keys[1], sizeof(int));
    if (str_result) {
        printf("2 -> %s\n", str_result);
    }

    hash_table_destroy(int_to_str_table);
    printf("\n");

    // Example 3: Custom structure as value
    printf("3. Custom structure as value:\n");
    hash_table_t *custom_table = hash_table_create(
        50,
        hash_func_string,
        key_compare_string,
        free_string,
        free_point
    );

    point_t *p1 = malloc(sizeof(point_t));
    p1->x = 10; p1->y = 20;
    
    point_t *p2 = malloc(sizeof(point_t));
    p2->x = 30; p2->y = 40;

    char *point_key1 = strdup("origin");
    char *point_key2 = strdup("target");

    hash_table_insert(custom_table, point_key1, strlen(point_key1) + 1, p1, sizeof(point_t));
    hash_table_insert(custom_table, point_key2, strlen(point_key2) + 1, p2, sizeof(point_t));

    point_t *point_result = hash_table_lookup(custom_table, "origin", strlen("origin") + 1);
    if (point_result) {
        printf("origin -> (%d, %d)\n", point_result->x, point_result->y);
    }

    hash_table_destroy(custom_table);
#endif
    return 0;
}