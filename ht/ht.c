#include "ht.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../bitcask_2/include/utils.h"

// 哈希表节点
typedef struct hash_node
{
    void *key;              // 键指针
    size_t key_size;        // 键大小
    void *value;            // 值指针
    size_t value_size;      // 值大小
    struct hash_node *next; // 下一个节点（解决哈希冲突）
} hash_node_t;

// 哈希表结构
/*这里是哈希表的结构，包含桶数组、容量、当前大小、哈希函数、键比较函数、键释放函数和值释放函数*/
struct hash_table
{
    hash_node_t **buckets;               // 桶数组
    size_t capacity;                     // 容量
    size_t size;                         // 当前大小
    hash_func_t hash_func;               // 哈希函数
    key_compare_func_t key_compare_func; // 键比较函数
    key_free_func_t key_free_func;       // 键释放函数
    value_free_func_t value_free_func;   // 值释放函数
};


// 打印哈希表统计信息
void hash_table_print_stats(const hash_table_t *table)
{
    if (!table)
    {
        printf("Hash table is NULL\n");
        return;
    }

    printf("=== Hash Table Statistics ===\n");
    printf("Capacity: %zu\n", table->capacity);
    printf("Size: %zu\n", table->size);
    printf("Load factor: %.2f%%\n", (float)table->size / table->capacity * 100);

    // 计算桶的分布情况
    size_t empty_buckets = 0;
    size_t max_chain_length = 0;
    size_t total_chain_length = 0;

    for (size_t i = 0; i < table->capacity; i++)
    {
        size_t chain_length = 0;
        hash_node_t *current = table->buckets[i];
        
        while (current)
        {
            chain_length++;
            current = current->next;
        }

        if (chain_length == 0)
            empty_buckets++;
        
        if (chain_length > max_chain_length)
            max_chain_length = chain_length;
        
        total_chain_length += chain_length;
    }

    printf("Empty buckets: %zu (%.2f%%)\n", empty_buckets, 
           (float)empty_buckets / table->capacity * 100);
    printf("Max chain length: %zu\n", max_chain_length);
    printf("Average chain length: %.2f\n", 
           table->size > 0 ? (float)total_chain_length / table->size : 0);
    printf("=============================\n");
}


// 打印所有键值对（通用版本）
void hash_table_print_all(const hash_table_t *table, 
                         key_print_func_t key_print_func,
                         value_print_func_t value_print_func)
{
    if (!table)
    {
        printf("Hash table is NULL\n");
        return;
    }

    printf("=== Hash Table Contents (%zu items) ===\n", table->size);
    
    size_t item_count = 0;
    for (size_t i = 0; i < table->capacity; i++)
    {
        hash_node_t *current = table->buckets[i];
        
        if (current)
        {
            printf("Bucket[%zu]: ", i);
            
            while (current)
            {
                printf("[");
                if (key_print_func)
                    key_print_func(current->key, current->key_size);
                else
                    printf("<key@%p>", current->key);
                
                printf(" -> ");
                
                if (value_print_func)
                    value_print_func(current->value, current->value_size);
                else
                    printf("<value@%p>", current->value);
                
                printf("]");
                
                if (current->next)
                    printf(" -> ");
                
                current = current->next;
                item_count++;
            }
            printf("\n");
        }
    }
    
    if (item_count == 0)
    {
        printf("Hash table is empty\n");
    }
    printf("===================================\n");
}

// 专门用于字符串键值对的打印函数
void hash_table_print_all_strings(const hash_table_t *table)
{
    if (!table)
    {
        printf("Hash table is NULL\n");
        return;
    }

    printf("=== Hash Table String Contents (%zu items) ===\n", table->size);
    
    size_t item_count = 0;
    for (size_t i = 0; i < table->capacity; i++)
    {
        hash_node_t *current = table->buckets[i];
        
        while (current)
        {
            // 假设键和值都是字符串
            const char *key_str = (const char *)current->key;
            const char *value_str = (const char *)current->value;
            
            printf("Bucket[%zu] Item[%zu]: key=\"", i, item_count);
            
            // 安全地打印键（防止非字符串数据）
            if (current->key_size > 0 && key_str)
            {
                // 只打印可打印字符，非打印字符用转义序列
                for (size_t j = 0; j < current->key_size && j < 256; j++)
                {
                    char c = key_str[j];
                    if (c >= 32 && c <= 126) // 可打印ASCII字符
                        putchar(c);
                    else
                        printf("\\x%02x", (unsigned char)c);
                }
            }
            else
            {
                printf("<invalid_key>");
            }
            
            printf("\" -> value=\"");
            
            // 安全地打印值（防止非字符串数据）
            if (current->value_size > 0 && value_str)
            {
                for (size_t j = 0; j < current->value_size && j < 256; j++)
                {
                    char c = value_str[j];
                    if (c >= 32 && c <= 126) // 可打印ASCII字符
                        putchar(c);
                    else
                        printf("\\x%02x", (unsigned char)c);
                }
            }
            else if (current->value == NULL)
            {
                printf("<NULL>");
            }
            else
            {
                printf("<binary_data_%zu_bytes>", current->value_size);
            }
            
            printf("\" (key_size=%zu, value_size=%zu)\n", 
                   current->key_size, current->value_size);
            
            current = current->next;
            item_count++;
        }
    }
    
    if (item_count == 0)
    {
        printf("Hash table is empty\n");
    }
    printf("==========================================\n");
}
 


// 创建哈希表
/*这里是创建哈希表的函数，参数是容量、哈希函数、键比较函数、键释放函数和值释放函数*/
hash_table_t *hash_table_create(
    size_t capacity,
    hash_func_t hash_func,
    key_compare_func_t key_compare_func,
    key_free_func_t key_free_func,
    value_free_func_t value_free_func)
{
    if (capacity == 0 || !hash_func || !key_compare_func)
    {
        return NULL;
    }

    hash_table_t *table = malloc(sizeof(hash_table_t));
    if (!table)
        return NULL;

    table->buckets = calloc(capacity, sizeof(hash_node_t *));
    if (!table->buckets)
    {
        free(table);
        return NULL;
    }

    table->capacity = capacity;
    table->size = 0;
    table->hash_func = hash_func;
    table->key_compare_func = key_compare_func;
    table->key_free_func = key_free_func;
    table->value_free_func = value_free_func;

    return table;
}

// 销毁哈希表
/*这里是销毁哈希表的函数，参数是哈希表指针*/
void hash_table_destroy(hash_table_t *table)
{
    if (!table)
        return;

    hash_table_clear(table);
    free(table->buckets);
    free(table);
}

// 创建节点
/*这里是创建哈希表节点的函数，参数是键的指针、键的大小、值的指针和值的大小*/
static hash_node_t *create_node(
    const void *key, size_t key_size,
    const void *value, size_t value_size)
{
    hash_node_t *node = malloc(sizeof(hash_node_t));
    if (!node)
        return NULL;

    // 分配并复制键
    node->key = malloc(key_size);
    if (!node->key)
    {
        free(node);
        return NULL;
    }
    memcpy(node->key, key, key_size);
    node->key_size = key_size;

    // 分配并复制值
    if (value && value_size > 0)
    {
        node->value = malloc(value_size);
        if (!node->value)
        {
            free(node->key);
            free(node);
            return NULL;
        }
        memcpy(node->value, value, value_size);
        node->value_size = value_size;
    }
    else
    {
        node->value = NULL;
        node->value_size = 0;
    }

    node->next = NULL;
    return node;
}

// 释放节点
/*这里是释放哈希表节点的函数，参数是哈希表指针和节点指针*/
static void free_node(hash_table_t *table, hash_node_t *node)
{
    if (!node)
        return;

    if (table->key_free_func)
    {
        table->key_free_func(node->key);
    }
    else
    {
        free(node->key);
    }

    if (table->value_free_func)
    {
        table->value_free_func(node->value);
    }
    else
    {
        free(node->value);
    }

    free(node);
}
// 单独独立出来键是否存在
// 检查键是否已存在
bool hash_table_key_exists(
    hash_table_t *table,
    const void *key,
    size_t key_size)
{
    if (!table || !key || key_size == 0)
        return false;

    // 计算哈希值
    size_t index = table->hash_func(key, key_size) % table->capacity;

    // 检查键是否已存在
    hash_node_t *current = table->buckets[index];
    while (current)
    {
        if (table->key_compare_func(current->key, key, key_size))
        {
            // 键已存在
            return true;
        }
        current = current->next;
    }

    // 键不存在
    return false;
}

// 插入键值对
/*这里是插入哈希表键值对的函数，参数是哈希表指针、键的指针、键的大小、值的指针和值的大小*/
bool hash_table_insert(
    hash_table_t *table,
    void *key,
    size_t key_size,
    void *value,
    size_t value_size)
{
    if (!table || !key || key_size == 0)
        return false;

    // 计算哈希值
    size_t index = table->hash_func(key, key_size) % table->capacity;

    // 检查键是否已存在
    hash_node_t *current = table->buckets[index];
    while (current)
    {
        if (table->key_compare_func(current->key, key, key_size))
        {
            LOG("key_existed: key_size: %d,  index: %ld,now update\n", key_size, index);
            // 键已存在，更新值
            if (table->value_free_func)
            {
                table->value_free_func(current->value);
            }
            else
            {
                free(current->value);
            }

            if (value && value_size > 0)
            {
                current->value = malloc(value_size);
                if (!current->value)
                    return false;
                memcpy(current->value, value, value_size);
                current->value_size = value_size;
            }
            else
            {
                current->value = NULL;
                current->value_size = 0;
            }
            return true;
        }
        current = current->next;
    }

    // 创建新节点
    hash_node_t *new_node = create_node(key, key_size, value, value_size);
    if (!new_node)
        return false;

    // 插入到链表头部
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
    table->size++;

    return true;
}

// 查找值
/*这里是查找哈希表键对应的值的函数，参数是哈希表指针、键的指针和键的大小*/
void *hash_table_lookup(
    hash_table_t *table,
    const void *key,
    size_t key_size)
{
    if (!table || !key || key_size == 0)
        return NULL;

    size_t index = table->hash_func(key, key_size) % table->capacity;
    hash_node_t *current = table->buckets[index];

    while (current)
    {
        if (table->key_compare_func(current->key, key, key_size))
        {
            return current->value;
        }
        current = current->next;
    }

    return NULL;
}

// 删除键值对
/*这里是删除哈希表键值对的函数，参数是哈希表指针、键的指针和键的大小*/
bool hash_table_remove(
    hash_table_t *table,
    const void *key,
    size_t key_size)
{
    if (!table || !key || key_size == 0)
        return false;

    size_t index = table->hash_func(key, key_size) % table->capacity;
    hash_node_t *current = table->buckets[index];
    hash_node_t *prev = NULL;

    while (current)
    {
        if (table->key_compare_func(current->key, key, key_size))
        {
            if (prev)
            {
                prev->next = current->next;
            }
            else
            {
                table->buckets[index] = current->next;
            }

            free_node(table, current);
            table->size--;
            return true;
        }
        prev = current;
        current = current->next;
    }

    return false;
}

// 获取哈希表大小
/*这里是获取哈希表大小的函数，参数是哈希表指针*/
size_t hash_table_size(const hash_table_t *table)
{
    return table ? table->size : 0;
}

// 获取哈希表容量
size_t hash_table_capacity(const hash_table_t *table)
{
    return table ? table->capacity : 0;
}

// 清空哈希表
void hash_table_clear(hash_table_t *table)
{
    if (!table)
        return;

    for (size_t i = 0; i < table->capacity; i++)
    {
        hash_node_t *current = table->buckets[i];
        while (current)
        {
            hash_node_t *next = current->next;
            free_node(table, current);
            current = next;
        }
        table->buckets[i] = NULL;
    }
    table->size = 0;
}

// 字符串哈希函数
/*这里是字符串哈希函数的实现，参数是键的指针和键的大小*/
size_t hash_func_string(const void *key, size_t key_size)
{
    const char *str = (const char *)key;
    size_t hash = 5381;
    int c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}

// 整数哈希函数
/*这里是整数哈希函数的实现，参数是键的指针和键的大小*/
size_t hash_func_int(const void *key, size_t key_size)
{
    const int *p = (const int *)key;
    return (size_t)(*p);
}

// uint64哈希函数
/*这里是uint64哈希函数的实现，参数是键的指针和键的大小*/
size_t hash_func_uint64(const void *key, size_t key_size)
{
    const uint64_t *p = (const uint64_t *)key;
    return (size_t)(*p);
}

// 字符串比较函数
/*这里是字符串比较函数的实现，参数是两个键的指针和键的大小*/
bool key_compare_string(const void *key1, const void *key2, size_t key_size)
{
    const char *str1 = (const char *)key1;
    const char *str2 = (const char *)key2;
    return strcmp(str1, str2) == 0;
}

// 整数比较函数
/*这里是整数比较函数的实现，参数是两个键的指针和键的大小*/
bool key_compare_int(const void *key1, const void *key2, size_t key_size)
{
    const int *p1 = (const int *)key1;
    const int *p2 = (const int *)key2;
    return *p1 == *p2;
}

// 内存比较函数
/*这里是内存比较函数的实现，参数是两个键的指针和键的大小*/
bool key_compare_memory(const void *key1, const void *key2, size_t key_size)
{
    return memcmp(key1, key2, key_size) == 0;
}