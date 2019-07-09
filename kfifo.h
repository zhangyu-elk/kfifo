#ifndef _KFIFO_H
#define _KFIFO_H

/*Linux内核中的kfifo无锁缓冲队列, 一个线程读一个线程写是不会出现问题的
* 如果多个线程同时读或者同时写, 需要自行定义扩充锁结构
* 如果是想存储结构体队列, 需要进行一定的修改
*/

#include <string.h>
#include <stdio.h>
#include <limits.h>

#define min(a, b)	(((a) < (b)) ? (a) : (b))

//判断数是不是2的幂次方
inline int is_power_of_2(unsigned int num)
{
	return (num != 0 && ((num & (num - 1)) == 0)) ? 1 : 0;
}
//将数扩展为2的幂次方
inline long roundup_pow_of_two(unsigned long num)
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
	unsigned char 	*buffer;
	unsigned int 	size;
	unsigned int	in;
	unsigned int 	out;
};

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

unsigned int kfifo_len(kfifo_t *queue)
{
	return (queue->in - queue->out);
	//如果走到了末尾是由可能, in小于out的
}

unsigned int kfifo_put(kfifo_t *queue,
             unsigned char *buffer, unsigned int len)
{
    unsigned int l;
	
	len = min(len, queue->size - queue->in + queue->out);
	//可复制的长度, 如果最后in跑到out前面去了, 由于unsigned int 循环所以取得值还是正确的
	l = min(len, queue->size - (queue->in & (queue->size - 1)));
	//in后续的空间
	memcpy(queue->buffer + (queue->in & (queue->size - 1)), buffer, l);
	//复制到in后面
	memcpy(queue->buffer, buffer + l, len - l);
	//如果还有, 复制到buffer开头

    queue->in += len;

    return len;
}

unsigned int kfifo_get(kfifo_t *queue, unsigned char *buffer, unsigned int len)
{
	unsigned int l = 0;
	
	len = min(len, queue->in - queue->out);
	//可复制的长度, unsigned int循环特性
	l = min(len, queue->size - (queue->out & (queue->size - 1)));
	//out到buffer末尾后面数据的长度
	memcpy(buffer, queue->buffer + (queue->out & (queue->size - 1)), l);
	
	memcpy(buffer + l, queue->buffer, len - l);
	
	queue->out += len;
	
	return len;
}

unsigned int kfifo_empty(kfifo_t *queue)
{
	return (queue->in == queue->out);
}

#endif


