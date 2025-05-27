#pragma once
#include <pthread.h>

#define TRUE 1
#define FALSE 0

typedef struct Product {
	const char* name;
	int amount;
	int price;
	int orderCount;
	int dealTryCount;
	int isDealed;
	pthread_mutex_t lock;
}Product;

typedef struct Order {
	int id;
	Product* product;
}Order;

typedef struct ProductNode {
	Product* data;
	struct ProductNode* link;
}ProductNode;

typedef struct OrderNode {
	Order* data;
	struct OrderNode* link;
}OrderNode;

typedef struct ProductQueue {
	ProductNode *front, *rear;
}ProductQueue;

typedef struct OrderQueue {
	OrderNode *front, *rear;
}OrderQueue;

//매물 큐 초기화 함수
void initProductQueue(ProductQueue* q);

//매물 큐 공백 확인 함수
int isEmptyProductQueue(ProductQueue* q);

//매물 큐 요소 추가 함수
void enqueueProduct(ProductQueue* q, Product* data);

//매물 큐 요소 빼기 함수
Product* dequeueProduct(ProductQueue* q);

//매물 큐 출력 함수
void printProductQueue(ProductQueue* q);


//주문 큐 초기화 함수
void initOrderQueue(OrderQueue* q);

//주문 큐 공백 확인 함수
int isEmptyOrderQueue(OrderQueue* q);

//주문 큐 요소 추가 함수
void enqueueOrder(OrderQueue* q, Order* data);

//주문 큐 요소 빼기 함수
Order* dequeueOrder(OrderQueue* q);

//주문 큐 출력 함수
void printOrderQueue(OrderQueue* q);