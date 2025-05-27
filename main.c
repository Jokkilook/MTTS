#define HAVE_STRUCT_TIMESPEC
#include "Struct.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <Windows.h>

//매물 큐
ProductQueue* productQueue;
sem_t prodLock;

//주문 큐
OrderQueue* orderQueue;
sem_t orderLock;

//주문번호 카운터
int orderId = 0;
sem_t idLock;

//평균가
int avgPrice = 4000;
sem_t avgLock;

//초기화
void init() 
{
	productQueue = (ProductQueue*)malloc(sizeof(ProductQueue));
	orderQueue = (OrderQueue*)malloc(sizeof(OrderQueue));

	initProductQueue(productQueue);
	initOrderQueue(orderQueue);

	sem_init(&prodLock, 0, 1);
	sem_init(&orderLock, 0, 1);
	sem_init(&idLock, 0, 1);
	sem_init(&avgPrice, 0, 1);

}

//판매자==================================================================================================================================================================
Product* createProduct() 
{
	Product* prod = (Product*)malloc(sizeof(Product));

	prod->name = getRandomName();
	prod->amount = rand() % 100;
	prod->price = 3000 + rand() % 47000;
	prod->orderCount = 0;
	prod->dealTryCount = 0;
	prod->isDealed = FALSE;
	pthread_mutex_init(&(prod->lock), NULL);
	printf("========================\n 이름 : %s\n 가격 : %d\n========================\n", prod->name, prod->price);

	return prod;
}

int addProduct()
{
	Product* prod = createProduct();

	sem_wait(&prodLock);
	enqueueProduct(productQueue, prod);
	sem_post(&prodLock);

	return TRUE;
}

//구매자==================================================================================================================================================================
Order* createOrder() 
{
	Order *order = (Order*)malloc(sizeof(Order));

	sem_wait(&idLock);
	order->id = orderId++;
	printf("[ %08d ]\n", order->id);
	sem_post(&idLock);


	do {
		sem_wait(&prodLock);	
		order->product = dequeueProduct(productQueue);
		sem_post(&prodLock);
	} while (order->product == NULL);

	return order;
}

int addOrder()
{
	Order* order = createOrder();

	sem_wait(&orderLock);
	enqueueOrder(orderQueue, order);
	sem_post(&orderLock);

	return TRUE;
}

//거래 시스템==============================================================================================================================================================
//평균 거래가와 차이가 얼마나 나는 지 보고 결정.
//평균보다 비싸면 확률 DOWN, 싸면 UP
//거래가 성사되면 평균가에 거래가를 반영
//거래 성사 여부 를 true 로
//거래 성사 여부가 true거나
//구매 실패 시 주문자 수--, 거래 시도 횟수 ++ 후
//주문자 수가 0이면 매물큐로 push
//주문자 수가 0이고 거래 성사여부가 true면 삭제
//아니면 다음 구매자가 거래 시도.
void setAverage(int price)
{
	avgPrice = (int)((avgPrice + price) / 2);
}

int dealing()
{
	Order* order;

	//주문 큐에서 주문 꺼내오기
	do {
		sem_wait(&orderLock);
		order = dequeueOrder(orderQueue);
		sem_post(&orderLock);
	} while (order == NULL);

	int percent = 100;
	pthread_mutex_lock(&order->product->lock);
	if (order->product->price < avgPrice) {
		//평균가가 높으면 = 가격이 싸면
		percent = 50 + rand() % 50;
	}
	else {
		//평균가가 낮으면 = 비싸면
		percent = 50 - rand() % 50;
	}
	
	//확률에 따라 구매 or 안구매
	if (rand() % 100 < percent) {
		order->product->isDealed = TRUE;
	}
	else {
		order->product->orderCount--;
		order->product->dealTryCount++;
	}
	pthread_mutex_unlock(&order->product->lock);

	sem_wait(avgLock);
	setAverage(order->product->price);
	sem_post(avgLock);

	return TRUE;
}



//메인====================================================================================================================================================================
int main() 
{
	srand(time(NULL));
	
	init();

	for (int i = 0; i < 100; i++) 
	{
		pthread_t t;
		pthread_create(&t, NULL, addProduct, NULL);
		pthread_t t2;
		pthread_create(&t2, NULL, addOrder, NULL);

		pthread_join(t, NULL);
		pthread_join(t2, NULL);
	}

	printf("\n\n");

	printOrderQueue(orderQueue);

	return 0;
}