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
int avgPrice = 35000;
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

//매물 생성
Product* createProduct() 
{
	Product* prod = (Product*)malloc(sizeof(Product));
	unsigned int price = 0;

	unsigned int randVal;
	if (rand_s(&randVal) == 0) {
		price = randVal;
	}

	prod->name = getRandomName();
	prod->amount = rand() % 100;
	prod->price = 3000 + (price % 47000);
	prod->orderCount = 0;
	prod->dealTryCount = 0;
	prod->isDealed = FALSE;
	pthread_mutex_init(&(prod->lock), NULL);
	printf("========================\n 이름 : %s\n 가격 : %d\n========================\n", prod->name, prod->price);

	return prod;
}

//매물큐에 매물 추가
int addProduct()
{
	Product* prod = createProduct();

	sem_wait(&prodLock);
	enqueueProduct(productQueue, prod);
	sem_post(&prodLock);

	return TRUE;
}

//구매자==================================================================================================================================================================

//주문 생성
Order* createOrder() 
{
	Order *order = (Order*)malloc(sizeof(Order));

	sem_wait(&idLock);
	order->id = orderId++;
	sem_post(&idLock);


	do {
		sem_wait(&prodLock);	
		order->product = dequeueProduct(productQueue);
		sem_post(&prodLock);
	} while (order->product == NULL);

	printf("[ %08d ] = [ %s ]\n", order->id, order->product->name);

	return order;
}

//주문큐에 주문 추가
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

//평균가 반영
void setAverage(int price)
{
	avgPrice = (int)((avgPrice + price) / 2);
}

//거래 행위
int dealing()
{
	Order* order;

	//주문 큐에서 주문 꺼내오기
	do {
		sem_wait(&orderLock);
		order = dequeueOrder(orderQueue);
		sem_post(&orderLock);
	} while (order == NULL);

	unsigned int percent = 100;

	unsigned int randVal;
	if (rand_s(&randVal) == 0) {
		percent = randVal % 100;
	}

	pthread_mutex_lock(&order->product->lock);
	if (order->product->price < avgPrice) {
		//평균가가 높으면 = 가격이 싸면
		percent = 50 + (randVal % 50);
	}
	else {
		//평균가가 낮으면 = 비싸면
		percent = 50 - (randVal % 50);
	}
	
	//확률에 따라 구매 or 안구매
	if (randVal % 100 < percent) {
		order->product->isDealed = TRUE;
	}
	else {
		order->product->orderCount--;
		order->product->dealTryCount++;
	}

	sem_wait(&avgLock);
	setAverage(order->product->price);
	sem_post(&avgLock);
	pthread_mutex_unlock(&order->product->lock);

	printf("거래완 : [ %d ] | 평균가 : %d\n\n", order->id, avgPrice);

	return TRUE;
}

//쓰레기 수집가============================================================================================================================================================



//메인====================================================================================================================================================================
int main() 
{
	srand(time(NULL));
	
	init();

	for (int i = 0; i < 100000; i++) 
	{
		//구매자 쓰레드 생성
		pthread_t t2;
		pthread_create(&t2, NULL, addOrder, NULL);

		//거래 실행 쓰레드 생성
		pthread_t t3;
		pthread_create(&t3, NULL, dealing, NULL);

		//판매자 쓰레드 생성
		pthread_t t;
		pthread_create(&t, NULL, addProduct, NULL);
		

		pthread_join(t, NULL);
		pthread_join(t2, NULL);
		pthread_join(t3, NULL);
	}

	printf("\n\n");

	printOrderQueue(orderQueue);

	return 0;
}