#ifndef __RT_SERVICE_H__
#define __RT_SERVICE_H__

/* 已知一个结构体里面的成员的地址，反推出该结构体的首地址 */
#define rt_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
			
#define rt_list_entry(node, type, member) \
    rt_container_of(node, type, member)
		
/*
*************************************************************************
*                           双向链表操作相关函数
*************************************************************************
*/
		
/* 初始化链表节点 */
rt_inline void rt_list_init(rt_list_t *l)
{
    l->next = l->prev = l;
}

/* 在双向链表指定节点后面插入一个节点 
*这个函数其实是把节点n插入节点l的后面，
*其插入的规律是：1指我，我指1，2指我，我指2.
*在这个函数中其中1代表l->next，我代表n，2代表l。
*很有规律性——你指我一下，我指你一下，他指我一下，我再指他一下，指回去。
*/
rt_inline void rt_list_insert_after(rt_list_t *l, rt_list_t *n)
{
    l->next->prev = n;
    n->next = l->next;

    l->next = n;
    n->prev = l;
}

/* 在双向链表指定节点前面插入一个节点 */
rt_inline void rt_list_insert_before(rt_list_t *l, rt_list_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

/* 从双向链表删除一个节点 */
rt_inline void rt_list_remove(rt_list_t *n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;

    n->next = n->prev = n;
}

/**
 * @brief 判断列表是否为空
 * @param l 需要判断的list
 */
rt_inline int rt_list_isempty(const rt_list_t *l)
{
    return l->next == l;
}

#endif /* __RT_SERVICE_H__ */
