#include <stdio.h>    /* for printf */
#include <stdlib.h>   /* for malloc */

//Reference: http://en.wikipedia.org/wiki/Linked_list
//		   : http://answers.google.com/answers/threadview/id/312972.html
 
typedef struct node {
	int data[4];
	struct node *next; /* pointer to next element in list */
} LLIST;

//Methods 
LLIST *list_add(LLIST **p, int *i);
void list_remove(LLIST **p);
LLIST **list_search(LLIST **n, int *i);
void list_print(LLIST *n);
void list_reverse(LLIST **p);
void ToArray(LLIST *n, int *array);
LLIST *head;




 
LLIST *list_add(LLIST **p, int *i)
{
	LLIST *cur;
	int x;
	if (p == NULL)
		return NULL;
 
	LLIST *tmp = malloc(sizeof(LLIST));
	if (tmp == NULL)
		return NULL;
	
	for(x = 0 ; x <4 ; x++)
		tmp->data[x] = i[x];
	
	tmp->next = NULL;
	
	
	cur = *p;
	
	if (cur == NULL) {
      *p = tmp;
   } else {
      while (cur->next != NULL)
        cur = cur->next;
      cur->next = tmp;
   }
	
	//n->next = *p; /* the previous element (*p) now becomes the "next" element */
	
	
    //*p = n;       /* add new empty element to the front (head) of the list */
   
	
	//for(x = 0 ; x <4 ; x++)
	//	n->data[x] = i[x];
		//head->data[x] = i[x];
		
 
	return *p;
}




void list_reverse(LLIST **p)
{ 
	LLIST *n = malloc(sizeof(LLIST));
		
	while(*p != NULL)
	{
	
		list_add(&n, (*p)->data);
		//list_print(n);
	
		*p = (*p)->next;
	}
	//n->next = NULL;
	//list_print(n);
	//n->next = *p; /* the previous element (*p) now becomes the "next" element */
	*p = n;       /* add new empty element to the front (head) of the list */
	
	//list_print(*p);
}
 
void list_remove(LLIST **p) /* remove head */
{
	if (p != NULL && *p != NULL)
	{
		LLIST *n = *p;
		*p = (*p)->next;
		free(n);
	}
}
 
LLIST **list_search(LLIST **n, int *i)
{
	if (n == NULL)
		return NULL;
 
	while (*n != NULL)
	{
		if ((*n)->data == i)
		{
			return n;
		}
		n = &(*n)->next;
	}
	return NULL;
}
 
void list_print(LLIST *n)
{
	int j = 0;
	if (n == NULL)
	{
		printf("list is empty\n");
	}
	while (n != NULL)
	{
		int i;
		
		printf("%d Node\n",j+1);
		for(i=0; i< 4; i++)
		{
			//printf("print %p %p %d\n", n, n->next, n->data[i]);
			printf("print %p %d\n", n, n->data[i]);
		}
		n = n->next;
		j++;
	}
}

void ToArray(LLIST *n, int *array)
{
	
    
	int j = 0;
	if (n == NULL)
	{
		printf("list is empty\n");
	}
	while (n != NULL)
	{
		int i;
		
		//printf("%d Node\n",j+1);
		int k = j * 4;
		
		for(i=0; i< 4; i++)
		{
			
			//printf("print %p %p %d\n", n, n->next, n->data[i]);
			//printf("print %p %d\n", n, n->data[i]);
			array[k+i] = n->data[i];
			//printf("%d\n",array[k+i]);
		}
		n = n->next;
		j++;
	}
	
	
	
}



//int main(void)
//{
//	LLIST *n = NULL;
 
//	list_add(&n, 0); /* list: 0 */
//	list_add(&n, 1); /* list: 1 0 */
//	list_add(&n, 2); /* list: 2 1 0 */
//	list_add(&n, 3); /* list: 3 2 1 0 */
//	list_add(&n, 4); /* list: 4 3 2 1 0 */
//	list_print(n);
//	list_remove(&n);                 /* remove first (4) */
//	list_remove(&n->next);           /* remove new second (2) */
//	list_remove(list_search(&n, 1)); /* remove cell containing 1 (first) */
//	list_remove(&n->next);           /* remove second to last node (0) */
//	list_remove(&n);                 /* remove last (3) */
//	list_print(n);
 
//	return 0;
//}

 
