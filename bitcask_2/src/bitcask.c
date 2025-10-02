#include "../include/bitcask.h"
#include "../../ht/ht.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define MAIN

// 创建哈希表
hash_table_t *index_hash_table = NULL;

// 释放pos_len内存
static void free_pos_len(void *ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}
// ddd
// 释放条目内存
static void free_entry(void *entry)
{
    if (entry != NULL)
    {
        c_bitcask_log_entry log_entry = (c_bitcask_log_entry)entry;
        free(log_entry->key);
        free(log_entry->value);
        free(log_entry);
    }
}

void print_entry(c_bitcask_log_entry entry)
{
    LOG("key: %s, value: %s, key_size: %d, value_size: %d\n", entry->key, entry->value, entry->key_size, entry->value_size);
}
// 创建日志条目
static c_bitcask_log_entry make_entry(string key, string value)
{
    if (key == NULL || value == NULL)
        return NULL;

    c_bitcask_log_entry entry = (c_bitcask_log_entry)malloc(sizeof(struct c_bitcask_log_entry_));
    if (entry == NULL)
        return NULL;

    entry->key_size = strlen(key);
    entry->value_size = strlen(value);

    entry->key = (string)malloc(entry->key_size + 1);
    entry->value = (string)malloc(entry->value_size + 1);
    if (entry->key == NULL || entry->value == NULL)
    {
        free(entry->key);
        free(entry->value);
        free(entry);
        return NULL;
    }

    strcpy(entry->key, key);
    strcpy(entry->value, value);
    print_entry(entry);
    return entry;
}

// 索引的值结构体
pos_len make_pos_len(int value_position, int value_len)
{
    pos_len ret = (pos_len)malloc(sizeof(struct pos_len_));
    if (ret == NULL)
        return NULL;
    ret->value_position = value_position;
    ret->value_len = value_len;
    return ret;
}

// 写入条目到文件
static int append(FILE *file, c_bitcask_log_entry entry)
{
    if (file == NULL || entry == NULL)
        return -1;

    // 记录当前写入位置
    int pos = ftell(file);

    // 如果键已存在，先删除旧索引
    if (hash_table_lookup(index_hash_table, entry->key, entry->key_size) != NULL)
    {
        hash_table_remove(index_hash_table, entry->key, entry->key_size);
    }

    // 创建新的位置信息
    pos_len pl = make_pos_len(pos, entry->value_size);
    if (pl == NULL)
    {
        return -1;
    }

    // // 写入哈希索引 - 注意：哈希表会复制键和值，所以我们不需要担心内存所有权
    // if (hash_table_insert(index_hash_table, entry->key, entry->key_size, pl, sizeof(struct pos_len_)) == false)
    // {
    //     fLOG(stderr, "Failed to insert key %s into hash table\n", entry->key);
    //     free_pos_len(pl);
    //     return -1;
    // }

    // 这里间已存在时会自动更新
    hash_table_insert(index_hash_table, entry->key, entry->key_size, pl, sizeof(struct pos_len_));
    // 写入文件
    if (fwrite(&entry->key_size, sizeof(int), 1, file) != 1)
        return -1;
    if (fwrite(&entry->value_size, sizeof(int), 1, file) != 1)
        return -1;
    if (fwrite(entry->key, sizeof(char), entry->key_size, file) != entry->key_size)
        return -1;
    if (fwrite(entry->value, sizeof(char), entry->value_size, file) != entry->value_size)
        return -1;

    fflush(file);
    return 0;
}

// 从文件读取条目
static int read_entry(FILE *file, c_bitcask_log_entry *entry_out, string key)
{
    if (file == NULL || entry_out == NULL || key == NULL)
        return -1;

    *entry_out = NULL;

    // 从哈希索引中查找位置信息
    pos_len value_pos_len = (pos_len)hash_table_lookup(index_hash_table, key, strlen(key));
    if (value_pos_len == NULL)
    {
        LOG("Key %s not found in index hash table\n", key);
        return -1;
    }

    // 移动到文件位置
    if (fseek(file, value_pos_len->value_position, SEEK_SET) != 0)
        return -1;

    // 读取条目
    int key_len, val_len;
    if (fread(&key_len, sizeof(int), 1, file) != 1)
        return -1;
    if (fread(&val_len, sizeof(int), 1, file) != 1)
        return -1;

    c_bitcask_log_entry entry = (c_bitcask_log_entry)malloc(sizeof(struct c_bitcask_log_entry_));
    if (entry == NULL)
        return -1;

    entry->key_size = key_len;
    entry->value_size = val_len;
    entry->key = (string)malloc(key_len + 1);
    entry->value = (string)malloc(val_len + 1);

    if (entry->key == NULL || entry->value == NULL)
    {
        free_entry(entry);
        return -1;
    }

    if (fread(entry->key, sizeof(char), key_len, file) != key_len)
    {
        free_entry(entry);
        return -1;
    }
    entry->key[key_len] = '\0';

    if (fread(entry->value, sizeof(char), val_len, file) != val_len)
    {
        free_entry(entry);
        return -1;
    }
    entry->value[val_len] = '\0';

    *entry_out = entry;
    return 1;
}

#ifdef MAIN

// 重建索引函数
static void rebuild_index(FILE *file)
{
    fseek(file, 0, SEEK_SET);

    LOG("Rebuilding index...\n");
    int entry_count = 0;

    while (!feof(file))
    {
        long current_pos = ftell(file);

        // 读取key长度
        int key_size;
        if (fread(&key_size, sizeof(int), 1, file) != 1)
        {
            if (feof(file))
                break;
            break;
        }

        // 读取value长度
        int value_size;
        if (fread(&value_size, sizeof(int), 1, file) != 1)
        {
            break;
        }

        // 读取key
        string key = (string)malloc(key_size + 1);
        if (key == NULL)
            break;
        if (fread(key, sizeof(char), key_size, file) != key_size)
        {
            free(key);
            break;
        }
        key[key_size] = '\0';
        LOG("rebuild_index: key_size: %d, value_size: %d, current_pos: %ld\n", key_size, value_size, current_pos);

        // 跳过value（只建立索引，不读取实际值）
        if (fseek(file, value_size, SEEK_CUR) != 0)
        {
            free(key);
            break;
        }

        // 创建位置信息
        pos_len pl = make_pos_len(current_pos, value_size);
        if (pl != NULL)
        {
            // 插入新索引
            if (hash_table_insert(index_hash_table, key, key_size, pl, sizeof(struct pos_len_)) == 0)
            {
                entry_count++;
            }
            else
            {
                free_pos_len(pl);
            }
        }

        free(key);
    }

    LOG("Index rebuilt with %d entries\n", entry_count);
}

int main(void)
{
    // 创建哈希表 - 使用正确的内存管理函数
    index_hash_table = hash_table_create(
        100,
        hash_func_string,
        key_compare_string,
        free,        // key是动态分配的，需要释放
        free_pos_len // value是pos_len结构，需要释放
    );

    if (index_hash_table == NULL)
    {
        LOG("Failed to create hash table\n");
        return 1;
    }

    // 打开文件
    FILE *fp = fopen("D:\\bitcask6.bin", "ab+");
    if (fp == NULL)
    {
        perror("Failed to open file");
        hash_table_destroy(index_hash_table);
        return 1;
    }

    // 重建索引
    rebuild_index(fp);
    hash_table_print_stats(index_hash_table);
    hash_table_print_all(index_hash_table, NULL, NULL);
    hash_table_print_all_strings(index_hash_table);

    // 写入测试数据
    LOG("Writing test data...\n");
    c_bitcask_log_entry entry1 = make_entry("Alice", "20");
    c_bitcask_log_entry entry2 = make_entry("Jack", "30");
    c_bitcask_log_entry entry3 = make_entry("Joe", "400");
    c_bitcask_log_entry entry4 = make_entry("Alice", "200000"); // 更新Alice的值

    if (entry1)
    {
        if (append(fp, entry1) == 0)
            LOG("Written: Alice->20\n");
    }
    if (entry2)
    {
        if (append(fp, entry2) == 0)
            LOG("Written: Jack->30\n");
    }
    if (entry3)
    {
        if (append(fp, entry3) == 0)
            LOG("Written: Joe->400\n");
    }
    if (entry4)
    {
        if (append(fp, entry4) == 0)
            LOG("Written: Alice->200000 (updated)\n");
    }

    // 释放条目内存（注意：键已被哈希表复制，可以安全释放）
    free_entry(entry1);
    free_entry(entry2);
    free_entry(entry3);
    free_entry(entry4);

    // 测试读取
    printf("\nReading back data...\n");
    c_bitcask_log_entry read_entry_ptr = NULL;

    printf("=============Alice=============\n");
    if (read_entry(fp, &read_entry_ptr, "Alice") == 1)
    {
        printf("[%d-%d-%s-%s]\n", read_entry_ptr->key_size, read_entry_ptr->value_size,
               read_entry_ptr->key, read_entry_ptr->value);
        free_entry(read_entry_ptr);
    }
    else
    {
        printf("Failed to read Alice\n");
    }

    printf("=============Jack=============\n");
    if (read_entry(fp, &read_entry_ptr, "Jack") == 1)
    {
        printf("[%d-%d-%s-%s]\n", read_entry_ptr->key_size, read_entry_ptr->value_size,
               read_entry_ptr->key, read_entry_ptr->value);
        free_entry(read_entry_ptr);
    }
    else
    {
        printf("Failed to read Jack\n");
    }

    printf("=============Joe=============\n");
    if (read_entry(fp, &read_entry_ptr, "Joe") == 1)
    {
        printf("[%d-%d-%s-%s]\n", read_entry_ptr->key_size, read_entry_ptr->value_size,
               read_entry_ptr->key, read_entry_ptr->value);
        free_entry(read_entry_ptr);
    }
    else
    {
        printf("Failed to read Joe\n");
    }

    fclose(fp);
    hash_table_destroy(index_hash_table);

    printf("\nFile operations completed successfully.\n");
    return 0;
}

#endif

