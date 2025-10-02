#include <stdio.h>
#include "../include/border.h"
#define READ_TIME 3
int count = 0;
void judge_exist_index()
{
    // printf("turn %d\t", count);
    /*fistly, judge whether the count is the first or not*/
    border_print_3_columns("==fistly, judge whether the count is the first or not==",100,"+");
    printf("%s\n", "fistly, judge whether the count is the first or not");
    printf("%s\n\n", !(count++) ? "system first read,create the index file structrue from the log file first" : "read the index info to find the position in the disk directly");
}
void search()
{
    /*test1 start read data by the data's key*/
    // printf("%s\n", );
    border_print_3_columns("==test1 start read data by the data's key==",100,"+");
    /*then read the exactly position of the data in the disk*/
    printf("%s\n", "then read the exactly position of the data in the disk");
    printf("%s\n\n", "read the position and value len in the disk of the data according the data's key");
}

void insert()
{
    /*test2 start read data by the data's key*/
    border_print_3_columns("==test2 start insert data by the data's key==",100,"+");
    /*find whether have the data in the disk, if have, then insert in the end and update the info in index structrue*/
    /*if not, then insert in the end and insert the info in index structrue*/
    printf("%s\n", "find whether have the data in the disk, if have, then insert in the end and update the info in index structrue");
    printf("%s\n\n", "if not, then insert in the end and insert the info in index structrue");
}

void update()
{
    border_print_3_columns("==test2 start insert data by the data's key==",100,"\\");
    /*first find whether exist the data has the same key,*/
    /*if exists,append the data in the end of disk,and update the index with the new position and len but the same key*/
    /*if not, append the data in the end of disk,and update the index with the new position and len and the new key*/
    printf("%s\n", "first find whether exist the data has the same key");
    printf("%s\n", "if exists, append the data in the end of disk,and update the index with the new position and len but the same key");
    printf("%s\n\n", "if not, append the data in the end of disk,and update the index with the new position and len and the new key");
}

void delete()
{
}

int main(void)
{

    while (count < READ_TIME)
    {
        char TURN[100];
        sprintf(TURN, "turn %d", count);
        border_print_5_columns(TURN, 50,"#");
        // printf("turn %d\t", count);
        /*fistly, judge whether the count is the first or not*/
        judge_exist_index();
        /*test1 start read data by the data's key*/
        search();
        /*test2 start insert data with key-value*/
        insert();
        /*test3 start update data with key-value*/
        update();
        /*test4 start delete data with key*/
        delete();
    }

#if 0

    border_print_5_columns("hello", 2); 
    border_print_5_columns("hello", 20);
    border_print_5_columns("hello", 0);
#endif
    return 0;
}