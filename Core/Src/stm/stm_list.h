/*
 * stm_list.h
 *
 *  Created on: Nov 6, 2023
 *      Author: Jay
 */

#ifndef INC_STM_LIST_H_
#define INC_STM_LIST_H_


/* Includes ------------------------------------------------------------------*/

typedef PACKED_STRUCT _tListNode {
    struct _tListNode * next;
    struct _tListNode * prev;
} tListNode;

void LST_init_head (tListNode * listHead);

uint8_t LST_is_empty (tListNode * listHead);

void LST_insert_head (tListNode * listHead, tListNode * node);

void LST_insert_tail (tListNode * listHead, tListNode * node);

void LST_remove_node (tListNode * node);

void LST_remove_head (tListNode * listHead, tListNode ** node );

void LST_remove_tail (tListNode * listHead, tListNode ** node );

void LST_insert_node_after (tListNode * node, tListNode * ref_node);

void LST_insert_node_before (tListNode * node, tListNode * ref_node);

int LST_get_size (tListNode * listHead);

void LST_get_next_node (tListNode * ref_node, tListNode ** node);

void LST_get_prev_node (tListNode * ref_node, tListNode ** node);


#endif /* INC_STM_LIST_H_ */
