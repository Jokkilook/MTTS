#include <stdio.h>
#include <stdlib.h>
#include "Struct.h"

void initProductQueue(ProductQueue* q)
{
	q->front = NULL;
	q->rear = NULL;
}

int isEmptyProductQueue(ProductQueue* q)
{
	if (q->front == NULL && q->rear == NULL) return 1;

	return 0;
}

void enqueueProduct(ProductQueue* q, Product* data)
{
	ProductNode* temp = (ProductNode*)malloc(sizeof(ProductNode));
	temp->data = data;
	temp->link = NULL;

	if (isEmptyProductQueue(q)) {
		q->front = temp;
		q->rear = temp;
	}
	else {
		q->rear->link = temp;
		q->rear = temp;
	}
}

Product* dequeueProduct(ProductQueue* q)
{
	ProductNode* temp = q->front;
	Product* data;
	if (isEmptyProductQueue(q)) {
		return NULL;
	}
	else {
		data = temp->data;
		q->front = q->front->link;
		if (q->front == NULL)
			q->rear = NULL;
		free(temp);
		return data;
	}
}

void printProductQueue(ProductQueue* q)
{
	if (isEmptyProductQueue(q)) {
		printf("");
		return;
	}

	ProductNode* curr = q->front;
	int index = 1;

	while (curr != NULL) {
		const char* p = curr->data->name;
		printf("%s", p);
		curr = curr->link;
	}
}

void initOrderQueue(OrderQueue* q)
{
	q->front = NULL;
	q->rear = NULL;
}

int isEmptyOrderQueue(OrderQueue* q)
{
	if (q->front == NULL && q->rear == NULL) return 1;

	return 0;
}

void enqueueOrder(OrderQueue* q, Order* data)
{
	OrderNode* temp = (OrderNode*)malloc(sizeof(OrderNode));
	temp->data = data;
	temp->link = NULL;

	if (isEmptyOrderQueue(q)) {
		q->front = temp;
		q->rear = temp;
	}
	else {
		q->rear->link = temp;
		q->rear = temp;
	}
}

Order* dequeueOrder(OrderQueue* q)
{
	OrderNode* temp = q->front;
	Order* data;
	if (isEmptyOrderQueue(q)) {
		return NULL;
	}
	else {
		data = temp->data;
		q->front = q->front->link;
		if (q->front == NULL)
			q->rear = NULL;
		free(temp);
		return data;
	}
}

void printOrderQueue(OrderQueue* q)
{
	if (isEmptyOrderQueue(q)) {
		printf("");
		return;
	}

	OrderNode* curr = q->front;
	int index = 1;

	while (curr != NULL) {
		int p = curr->data->id;
		printf("%d", p);
		curr = curr->link;
	}
}
