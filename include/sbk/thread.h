#ifndef __SBK_THREAD_H
#define __SBK_THREAD_H


#include <pthread.h>


// TBD, may be removed

typedef struct {
	pthread_t   t;
	SbkMsgQueue *msgQueue;
} SbkThread;


void sbk_create_thread(void * (*func)(void *), SbkThread * t);
void sbk_handle_next_msg(SbkMsgQueue *msgQueue);
void sbk_put_msg(SbkThread *t, );


#endif // !__SBK_THREAD_H
