#include "bcgl_internal.h"

// str

char * __strdup(const char *str)
{
    int len = strlen(str);
    char *result = malloc(len + 1);
    memcpy(result, str, len);
    result[len] = 0;
    return result;
}

// clist

clist_node_t * clist_new_node(clist_t *list)
{
    clist_node_t *item = NEW_OBJECT(clist_node_t);
    if (!list->head)
    {
        list->head = item;
        list->tail = item;
    }
    else
    {
        list->tail->next = item;
        list->tail = item;
    }
    return item;
}

clist_node_t * clist_add_node(clist_t *list, void *data)
{
    clist_node_t *node = clist_new_node(list);
    node->data = data;
    return node;
}

void clist_delete_node(clist_t *list, void *data)
{
    clist_node_t *prev = NULL;
    for (clist_node_t *item = list->head; item; item = item->next)
    {
        if (item->data == data)
        {
            if (item == list->head)
            {
                list->head = item->next;
            }
            if (item == list->tail)
            {
                list->tail = prev;
            }
            if (prev)
            {
                prev->next = item->next;
            }
            free(item);
            break;
        }
        prev = item;
    }
}

void clist_clear(clist_t *list)
{
    for (clist_node_t *item = list->head; item; item = item->next)
    {
        free(item);
    }
    list->head = list->tail = NULL;
}
