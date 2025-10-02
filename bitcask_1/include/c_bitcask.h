#ifndef C_BITCASK_H_
#define C_BITCASK_H_

typedef struct c_bitcask_log_
{
} *c_bitcask_log;

typedef struct c_bitcask_index_
{
} *c_bitcask_index;

typedef struct c_bitcask_
{
    c_bitcask_index index;
    c_bitcask_log log;
} *c_bitcask;

#endif /* C_BITCASK_H_ */