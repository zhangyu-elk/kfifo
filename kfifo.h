#ifndef _KFIFO_H
#define _KFIFO_H

/*Linux内核中的kfifo无锁缓冲队列, 一个线程读一个线程写是不会出现问题的
* 如果多个线程同时读或者同时写, 需要自行定义扩充锁结构
* 如果是想存储结构体队列, 需要进行一定的修改
*/

#include <string.h>
#include <stdio.h>
#include <limits.h>

unsigned int min(unsigned int a, unsigned int b)
{
	return (a < b) ? a : b;
}

//判断数是不是2的幂次方
int is_power_of_2(unsigned int num)
{
	return (num != 0 && ((num & (num - 1)) == 0)) ? 1 : 0;
}
//将数扩展为2的幂次方
long roundup_pow_of_two(unsigned long num)
{
	unsigned long ret = 1;
	unsigned long num_bk = num;
	while(num >>= 1)
	{
		ret <<= 1;
	}
	return (ret < num_bk) ? (ret << 1) : ret;
}

typedef struct kfifo kfifo_t;
struct kfifo
{
	unsigned char 	*buffer;                //队列内存区间
	unsigned int 	size;                   //队列大小
	unsigned int	in;                     //向队列中存数据时的位置
	unsigned int 	out;                    //从队列中取数据时的位置
};

//初始化kfifo结构体，会把buffer大小扩充到2的倍数
kfifo_t *kfifo_init(unsigned char *buffer, unsigned int size)
{
	kfifo_t *ret = NULL;
	
	if(!is_power_of_2(size))
	{
		return ret;
	}
	
	ret = (kfifo_t*)malloc(sizeof(kfifo_t));
	if(ret == NULL)
	{
		return ret;
	}
	
	ret->buffer		= buffer;
	ret->size		= size;
	ret->in			= 0;
	ret->out		= 0;
	return ret;
}

kfifo_t *kfifo_alloc(unsigned int size)
{
    unsigned char *buffer = NULL;
    kfifo_t *ret = NULL;
	
	if(!is_power_of_2(size))
	{
		size = roundup_pow_of_two(size);
	}
	
    buffer = (unsigned char *)malloc(size);
    if (!buffer)
	{
        return ret;
    }
	
	memset(buffer, 0, sizeof(size));

    ret = kfifo_init(buffer, size);

    return ret;
}

void kfifo_free(kfifo_t *queue)
{
	if(queue)
	{
		free(queue->buffer);
		free(queue);
	}
}

void kfifo_reset(kfifo_t *queue)
{
	if(queue)
	{
		queue->in = queue->out = 0;
	}
}

unsigned int kfifo_cap(kfifo_t *queue)
{
    if (NULL == queue) {
        return 0;
    }
    return queue->size;
}

unsigned int kfifo_len(kfifo_t *queue)
{
    if (NULL == queue) {
        return 0;
    }
	return (queue->in - queue->out);
}

unsigned int kfifo_put(kfifo_t *queue,
             unsigned char *buffer, unsigned int len)
{
    if (NULL == queue) {
        return -1;
    }

    unsigned int l;

    //queue中剩余的空间和len进行比较，得出可以放到buffer中的最长大小
	len = min(len, queue->size - queue->in + queue->out);

	//计算出从in到buffer缓冲区结束剩余的空间
	l = min(len, queue->size - (queue->in & (queue->size - 1)));

	//将插入的buffer数据中一部分复制到in后面
	memcpy(queue->buffer + (queue->in & (queue->size - 1)), buffer, l);

	//将插入的buffer数据中剩余的一部分复制到queue的开头
	memcpy(queue->buffer, buffer + l, len - l);

	//长度增长，in会不断增长直到超过unsigned int可表示的数然后变为0
    queue->in += len;

    return len;
}

unsigned int kfifo_get(kfifo_t *queue, unsigned char *buffer, unsigned int len)
{
	unsigned int l = 0;

	//queue中总的数据长度和len比较，得出的就是此次可取出的最大数据
	len = min(len, queue->in - queue->out);

	//计算出out后面有多少数据
	l = min(len, queue->size - (queue->out & (queue->size - 1)));

	//将kfifo中out后面的数据复制到buffer中
	memcpy(buffer, queue->buffer + (queue->out & (queue->size - 1)), l);

	//如果kfifo头部还有一部分数据，同样复制到buffer中
	memcpy(buffer + l, queue->buffer, len - l);

	//out同样是在不停的增长
	queue->out += len;
	
	return len;
}

unsigned int kfifo_empty(kfifo_t *queue)
{
	return (queue->in == queue->out);
}

#endif


