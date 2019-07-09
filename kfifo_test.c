#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "kfifo.h"

static void *funWrite(void *arg)
{
    kfifo_t* que = (kfifo_t*)arg;
	int i = 0;

    for(; i < 200; ++i)
    {
        kfifo_put(que, (unsigned char*)&i, sizeof(int));
    }
	
	for(; i < 400; ++i)
    {
        kfifo_put(que, (unsigned char*)&i, sizeof(int));
    }
    return NULL;
}

static void* funRead(void *arg)
{
    kfifo_t* que = (kfifo_t*)arg;
	int x = 0;
    while(1)
    {
		int ret = 0;
		if(kfifo_get(que, (char*)&ret, 4))
		{
			printf("%d\n", ret);
			x = 1;
		}
        else
        {
			if(x)
			{
				x = 0;
				printf("%s\n", "get fail");
			}
			
            continue;
        }
    }
    return NULL;
}

int main()
{
    kfifo_t *que = kfifo_alloc(500);
    pthread_t t1, t2;
	pthread_create(&t2, NULL, funRead, que);
    pthread_create(&t1, NULL, funWrite, que);

	pthread_join(t2, NULL);
    pthread_join(t1, NULL);
    return 0;   
}