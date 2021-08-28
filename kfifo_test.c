#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "kfifo.h"

static void *funWrite(void *arg)
{
    kfifo_t* que = (kfifo_t*)arg;

    for(int i = 1; i < 1000000; ++i)
    {
        kfifo_put(que, (unsigned char*)&i, sizeof(int));
    }

    return NULL;
}

static void* funRead(void *arg)
{
    kfifo_t* que = (kfifo_t*)arg;
	int val = 0;
    while(1)
    {
		int tmp = 0;
		if(kfifo_get(que, (unsigned char*)&tmp, 4))
		{
			if (tmp == val + 1) {
			    val = tmp;
			} else {
                printf("kfifo_get error value\n");
                exit(-1);
			}
		} else {
            break;
		}
    }
    return NULL;
}

//TODO：单元测试

int main()
{
    kfifo_t *que = kfifo_alloc(1000000);
    pthread_t t1, t2;

    pthread_create(&t1, NULL, funWrite, que);
	pthread_create(&t2, NULL, funRead, que);

	pthread_join(t2, NULL);
    pthread_join(t1, NULL);
    return 0;   
}