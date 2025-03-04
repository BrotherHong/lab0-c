#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));

    if (head) {
        INIT_LIST_HEAD(head);
    }

    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        list_del(&entry->list);
        free(entry->value);
        free(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;

    e->value = strdup(s);
    if (!e->value) {
        free(e);
        return false;
    }

    list_add(&e->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;

    e->value = strdup(s);
    if (!e->value) {
        free(e);
        return false;
    }

    list_add_tail(&e->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *e = list_first_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del_init(&e->list);

    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *e = list_last_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del_init(&e->list);

    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;

    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    if (!head || list_empty(head))
        return false;

    struct list_head *front = head->next, *back = head->prev;
    while (front != back && front->next != back) {
        front = front->next;
        back = back->prev;
    }
    list_del(back);

    element_t *e = list_entry(back, element_t, list);
    free(e->value);
    free(e);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (!head || list_empty(head))
        return false;

    struct list_head tmp_head;

    INIT_LIST_HEAD(&tmp_head);

    while (!list_empty(head)) {
        struct list_head *node = head->next, *last = head->next;
        const element_t *entry_node = list_entry(node, element_t, list);
        const element_t *entry_last;

        do {
            last = last->next;
            if (last == head)
                break;

            entry_last = list_entry(last, element_t, list);
        } while (!strcmp(entry_node->value, entry_last->value));

        if (node->next == last)
            list_move_tail(node, &tmp_head);
        else {
            while (head->next != last) {
                element_t *entry_del = list_entry(head->next, element_t, list);
                list_del(head->next);
                free(entry_del->value);
                free(entry_del);
            }
        }
    }

    list_splice(&tmp_head, head);
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/

    struct list_head **node = &head->next;
    struct list_head *node1, *node2, *next;

    while (*node != head && (*node)->next != head) {
        node1 = *node;
        node2 = (*node)->next;
        next = node2->next;

        list_del(node1);
        node2->next = node1;
        node1->prev = node2;
        node1->next = next;
        next->prev = node1;

        node = &next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *node, *safe;
    struct list_head *prev, *next, *tmp;
    list_for_each_safe (node, safe, head) {
        prev = node->prev;
        next = node->next;
        node->next = prev;
        node->prev = next;
    }
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/

    if (!head || list_empty(head) || list_is_singular(head))
        return;

    const struct list_head *last = head->prev;
    struct list_head *node, *tmp;
    int totalCount = q_size(head);

    while (totalCount) {
        int count = 1;
        node = head->next;
        while (count < k && node != last) {
            node = node->next;
            count++;
        }

        if (count < k) {
            while (head->prev != last) {
                list_move_tail(head->next, head);
            }
        } else {
            while (node != head) {
                tmp = node;
                node = node->prev;
                list_move_tail(tmp, head);
            }
        }

        totalCount -= count;
    }
}

void q_merge_two(struct list_head *head1, struct list_head *head2, bool descend)
{
    struct list_head tmp_head;
    element_t *entry1, *entry2;
    const int desc = (descend ? -1 : 1);

    INIT_LIST_HEAD(&tmp_head);

    while (!list_empty(head1) && !list_empty(head2)) {
        entry1 = list_first_entry(head1, element_t, list);
        entry2 = list_first_entry(head2, element_t, list);

        if (strcmp(entry1->value, entry2->value) * desc < 0)
            list_move_tail(&entry1->list, &tmp_head);
        else
            list_move_tail(&entry2->list, &tmp_head);
    }

    list_splice_tail_init(head2, &tmp_head);
    list_splice(&tmp_head, head1);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    struct list_head head_left;
    const struct list_head *fast;
    struct list_head *slow = head;

    if (!head || list_empty(head) || list_is_singular(head))
        return;

    for (fast = head->next; fast != head && fast->next != head;
         fast = fast->next->next)
        slow = slow->next;

    list_cut_position(&head_left, head, slow);
    q_sort(&head_left, descend);
    q_sort(head, descend);
    q_merge_two(head, &head_left, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/

    struct list_head *node = head->next;
    const element_t *entry_node;
    element_t *entry_prev;

    while (node != head) {
        if (node->prev == head) {
            node = node->next;
            continue;
        }

        entry_node = list_entry(node, element_t, list);
        entry_prev = list_entry(node->prev, element_t, list);

        if (strcmp(entry_prev->value, entry_node->value) > 0) {
            list_del(&entry_prev->list);
            free(entry_prev->value);
            free(entry_prev);
        } else
            node = node->next;
    }

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/

    struct list_head *node = head->next;
    const element_t *entry_node;
    element_t *entry_prev;

    while (node != head) {
        if (node->prev == head) {
            node = node->next;
            continue;
        }

        entry_node = list_entry(node, element_t, list);
        entry_prev = list_entry(node->prev, element_t, list);

        if (strcmp(entry_prev->value, entry_node->value) < 0) {
            list_del(&entry_prev->list);
            free(entry_prev->value);
            free(entry_prev);
        } else
            node = node->next;
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/

    queue_contex_t *context_first =
        list_first_entry(head, queue_contex_t, chain);
    queue_contex_t *context_last = NULL;
    struct list_head *last = head->prev;

    while (last != head->next) {
        context_last = list_entry(last, queue_contex_t, chain);

        q_merge_two(context_first->q, context_last->q, descend);
        context_first->size = q_size(context_first->q);
        context_last->size = 0;

        last = last->prev;
    }

    return context_first->size;
}
