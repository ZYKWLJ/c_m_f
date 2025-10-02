#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// 哈希表结构体
typedef struct hash_table hash_table_t;

// 哈希函数类型
// 这里使用的是函数指针，指向一个哈希函数，用于计算键的哈希值，参数是键的指针和键的大小
typedef size_t (*hash_func_t)(const void *key, size_t key_size);
// 键比较函数类型
// 这里使用的是函数指针，指向一个键比较函数，用于比较两个键是否相等，参数是两个键的指针和键的大小
typedef bool (*key_compare_func_t)(const void *key1, const void *key2, size_t key_size);
// 键释放函数类型
// 这里使用的是函数指针，指向一个键释放函数，用于释放键的内存，参数是键的指针
typedef void (*key_free_func_t)(void *key);
// 值释放函数类型
// 这里使用的是函数指针，指向一个值释放函数，用于释放值的内存，参数是值的指针
typedef void (*value_free_func_t)(void *value);

// 这里使用的是函数指针，指向一个键释放函数，用于释放键的内存，参数是键的指针
typedef void (*key_print_func_t)(void *key,size_t key_size);
// 值打印函数类型
// 这里使用的是函数指针，指向一个值打印函数，用于打印值的内存，参数是值的指针
typedef void (*value_print_func_t)(void *value,size_t value_size);
void hash_table_print_stats(const hash_table_t *table);
void hash_table_print_all_strings(const hash_table_t *table);
void hash_table_print_all(const hash_table_t *table, 
                         key_print_func_t key_print_func,
                         value_print_func_t value_print_func);
// 创建哈希表
// 这里返回哈希表指针，用于创建一个哈希表，参数是哈希表的容量、哈希函数、键比较函数、键释放函数和值释放函数，注意都是这些函数的指针！
hash_table_t *hash_table_create(
    size_t capacity,                     /*哈希表的容量，即桶的数量*/
    hash_func_t hash_func,               /*哈希函数，用于计算键的哈希值*/
    key_compare_func_t key_compare_func, /*键比较函数，用于比较两个键是否相等*/
    key_free_func_t key_free_func,       /*键释放函数，用于释放键的内存*/
    value_free_func_t value_free_func    /*值释放函数，用于释放值的内存*/
);

// 销毁哈希表1
void hash_table_destroy(hash_table_t *table);

// 插入键值对
/*这里插入的是键值对，参数是哈希表指针、键的指针、键的大小、值的指针和值的大小*/
bool hash_table_insert(
    hash_table_t *table,
    void *key,
    size_t key_size,
    void *value,
    size_t value_size);

// 查找值
/*这里查找的是值，参数是哈希表指针、键的指针和键的大小*/
void *hash_table_lookup(
    hash_table_t *table,
    const void *key,
    size_t key_size);

// 删除键值对
/*这里删除的是键值对，参数是哈希表指针、键的指针和键的大小*/
bool hash_table_remove(
    hash_table_t *table,
    const void *key,
    size_t key_size);

// 获取哈希表大小
/*这里获取的是哈希表中的键值对数量，参数是哈希表指针*/
size_t hash_table_size(const hash_table_t *table);

// 获取哈希表容量
/*这里获取的是哈希表的容量，即桶的数量，参数是哈希表指针*/
size_t hash_table_capacity(const hash_table_t *table);

// 清空哈希表
/*这里清空的是哈希表中的所有键值对，参数是哈希表指针*/
void hash_table_clear(hash_table_t *table);

// 内置哈希函数
/*这里是一些内置的哈希函数，用于计算键的哈希值，参数是键的指针和键的大小*/
size_t hash_func_string(const void *key, size_t key_size);
size_t hash_func_int(const void *key, size_t key_size);
size_t hash_func_uint64(const void *key, size_t key_size);

// 内置比较函数
/*这里是一些内置的比较函数，用于比较两个键是否相等，参数是两个键的指针和键的大小*/
bool key_compare_string(const void *key1, const void *key2, size_t key_size);
bool key_compare_int(const void *key1, const void *key2, size_t key_size);
bool key_compare_memory(const void *key1, const void *key2, size_t key_size);

#endif // HASH_TABLE_H