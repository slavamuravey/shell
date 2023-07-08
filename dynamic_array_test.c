#include <stdio.h>
#include <stdlib.h>
#include "dynamic_array.h"

struct item {
    char *str;
    int number;
};

void test_dynamic_array_struct_item()
{
    struct dynamic_array *da = dynamic_array_create(3, sizeof(struct item));
    struct item *data;
    char *strs[] = {
        "str1",
        "str2",
        "str3",
        "str4",
        "str5",
        "str6",
        "str7",
        "str8",
        "str9",
        "str10",
    };
    int i;
    int items_count = 10;
    
    for (i = 0; i < items_count; i++) {
        struct item item;
        item.str = strs[i];
        item.number = (i + 1) * 10;
        dynamic_array_append(da, &item);
    }

    printf("cap: %ld, len: %ld, width: %ld\n", da->cap, da->len, da->width);

    data = da->ptr;
    for (i = 0; i < items_count; i++) {
        struct item item = data[i];
        printf("str: %s, number: %d\n", item.str, item.number);
    }
    free(data);
    free(da);
}

int main()
{
    test_dynamic_array_struct_item();

    return 0;
}
