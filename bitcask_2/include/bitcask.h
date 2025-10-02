#ifndef BITCASK_H_
#define BITCASK_H_
typedef unsigned long UL;
typedef char *string;

/*this is a log_entry*/
typedef struct c_bitcask_log_entry_
{
    int key_size;
    int value_size;
    string key;
    string value;
} *c_bitcask_log_entry;

/*this is a index_hash_key_entry*/
typedef struct pos_len_
{
    int value_position;
    int value_len;
} *pos_len;


// typedef struct c_bitcask_index_
// {
//     string key;
//     int value_position;
//     int value_len;
// } *c_bitcask_index;

// typedef struct c_bitcask_
// {
//     c_bitcask_index index;
//     c_bitcask_log_entry log;
// } *c_bitcask;

// c_bitcask_log_entry init(c_bitcask_log_entry entry){
//     entry=(c_bitcask_log_entry)malloc(sizeof(struct c_bitcask_log_entry_));
//     entry->key_size=0;
//     entry->value_size=0;
//     entry

// }
#endif /* BITCASK_H_ */
