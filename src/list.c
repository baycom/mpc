/* the Music Player Daemon (MPD)
 * (c)2003-2004 by Warren Dukes (warren.dukes@gmail.com)
 * This project's homepage is: http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "list.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>

static void makeListNodesArray(List * list) {
	ListNode * node = list->firstNode;
	long i;

	list->nodesArray = malloc(sizeof(ListNode *)*list->numberOfNodes);

	for(i=0;i<list->numberOfNodes;i++) {
		list->nodesArray[i] = node;
		node = node->nextNode;
	}
}

static void freeListNodesArray(List * list) {
	free(list->nodesArray);
	list->nodesArray = NULL;
}

List * makeList(ListFreeDataFunc * freeDataFunc) {
	List * list = malloc(sizeof(List));

	assert(list!=NULL);

	list->firstNode = NULL;
	list->lastNode = NULL;
	list->freeDataFunc = freeDataFunc;
	list->numberOfNodes = 0;
	list->nodesArray = NULL;

	return list;
}

int insertInList(List * list,char * key,void * data) {
	ListNode * node;

	assert(list!=NULL);
	assert(key!=NULL);
	assert(data!=NULL);

	node = malloc(sizeof(ListNode));
	assert(node!=NULL);

	if(list->nodesArray) freeListNodesArray(list);

	if(list->firstNode==NULL) {
		assert(list->lastNode==NULL);
		list->firstNode = node;
	}
	else {
		assert(list->lastNode!=NULL);
		assert(list->lastNode->nextNode==NULL);
		list->lastNode->nextNode = node;
	}
	
	node->key = malloc((strlen(key)+1)*sizeof(char));
	assert(node->key!=NULL);
	strcpy(node->key,key);
	node->data = data;
	node->nextNode = NULL;
	node->prevNode = list->lastNode;

	list->lastNode = node;

	list->numberOfNodes++;
	
	return 1;
}

int insertInListWithoutKey(List * list, void * data) {
	ListNode * node;

	assert(list!=NULL);
	assert(data!=NULL);

	node = malloc(sizeof(ListNode));
	assert(node!=NULL);
	
	if(list->nodesArray) freeListNodesArray(list);

	if(list->firstNode==NULL) {
		assert(list->lastNode==NULL);
		list->firstNode = node;
	}
	else {
		assert(list->lastNode!=NULL);
		assert(list->lastNode->nextNode==NULL);
		list->lastNode->nextNode = node;
	}

	node->key = NULL;
	node->data = data;
	node->nextNode = NULL;
	node->prevNode = list->lastNode;

	list->lastNode = node;

	list->numberOfNodes++;
	
	return 1;
}

int findInList(List * list,char * key,void ** data) {
	static long high;
	static long low;
	static long cur;
	static ListNode * tmpNode;
	static int cmp;

	assert(list!=NULL);

	if(list->nodesArray) {
		high = list->numberOfNodes-1;
		low = 0;
		cur = high;

		while(high>low) {
			cur = (high+low)/2;
			tmpNode = list->nodesArray[cur];
			cmp = strcmp(tmpNode->key,key);
			if(cmp==0) {
				(*data) = tmpNode->data;
				return 1;
			}
			else if(cmp>0) high = cur;
			else {
				if(low==cur) break;
				low = cur;
			}
		}

		cur = high;
		if(cur>=0) {
			tmpNode = list->nodesArray[cur];
			if(strcmp(tmpNode->key,key)==0) {
				(*data) = tmpNode->data;
				return 1;
			}
		}
	}
	else {
		tmpNode = list->firstNode;
	
		while(tmpNode!=NULL && strcmp(tmpNode->key,key)!=0) {
			tmpNode = tmpNode->nextNode;
		}
	
		if(tmpNode!=NULL) {
			(*data) = tmpNode->data;
			return 1;
		}
	}

	return 0;
}

int deleteFromList(List * list,char * key) {
	ListNode * tmpNode;

	assert(list!=NULL);

	tmpNode = list->firstNode;

	while(tmpNode!=NULL && strcmp(tmpNode->key,key)!=0) {
		tmpNode = tmpNode->nextNode;
	}

	if(tmpNode!=NULL)
		deleteNodeFromList(list,tmpNode);
	else
		return 0;

	return 1;
}

void deleteNodeFromList(List * list,ListNode * node) {
	assert(list!=NULL);
	assert(node!=NULL);
	
	if(node->prevNode==NULL) {
		list->firstNode = node->nextNode;
	}
	else {
		node->prevNode->nextNode = node->nextNode;
	}
	if(node->nextNode==NULL) {
		list->lastNode = node->prevNode;
	}
	else {
		node->nextNode->prevNode = node->prevNode;
	}
	if(list->freeDataFunc) {
		list->freeDataFunc(node->data);
	}
	free(node->key);
	free(node);
	list->numberOfNodes--;

	if(list->nodesArray) {
		freeListNodesArray(list);
		makeListNodesArray(list);
	}

}
		
void freeList(void * list) {
	ListNode * tmpNode;
	ListNode * tmpNode2;

	assert(list!=NULL);

	tmpNode = ((List *)list)->firstNode;

	if(((List *)list)->nodesArray) free(((List *)list)->nodesArray);

	while(tmpNode!=NULL) {
		tmpNode2 = tmpNode->nextNode;
		free(tmpNode->key);
		if(((List *)list)->freeDataFunc) {
			((List *)list)->freeDataFunc(tmpNode->data);
		}
		free(tmpNode);
		tmpNode = tmpNode2;
	}

	free(list);
}

void clearList(List * list) {
	ListNode * tmpNode;
	ListNode * tmpNode2;

	assert(list!=NULL);

	tmpNode = ((List *)list)->firstNode;

	while(tmpNode!=NULL) {
		tmpNode2 = tmpNode->nextNode;
		free(tmpNode->key);
		if(((List *)list)->freeDataFunc) {
			((List *)list)->freeDataFunc(tmpNode->data);
		}
		free(tmpNode);
		tmpNode = tmpNode2;
	}

	if(list->nodesArray) freeListNodesArray(list);

	list->firstNode = NULL;
	list->lastNode = NULL;
	list->numberOfNodes = 0;
}
