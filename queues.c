
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "queues.h"

static QUEUE_R *	qNewRecord(void *data, size_t dataSize)
{
	QUEUE_R *e;

	if ( ! (e = malloc(sizeof(QUEUE_R) + dataSize)) ) return NULL;

	e->prev     =
	e->next     = NULL;
	e->dataSize = dataSize;
	memcpy(e->data, data, dataSize);

	return e;
}

#define qFreeRecord(r)	free(r)

#define MINUS		-1
#define PLUS		 1

static void	qUpdateCounters(QUEUE *q, QUEUE_R *r, int multi)
{
	q->queueRecords += (multi);
	q->queueSize    += ((r->dataSize + sizeof(QUEUE_R)) * multi);
	q->datasSize	+= (r->dataSize * multi);
}

static QUEUE_R *	qUnlink(QUEUE *q, QUEUE_R *r)
{
	if ( r->next ) r->next->prev = r->prev;
	else           q->end        = r->prev;

	if ( r->prev ) r->prev->next = r->next;
	else           q->start      = r->next;

	r->next =
	r->prev = NULL;

	qUpdateCounters(q, r, MINUS);

	return r;
}

QUEUE *	qCreate(void)
{
	register QUEUE	*q = calloc(1, sizeof(QUEUE));
	if ( q ) q->queueSize = sizeof(QUEUE);
	return q;
}

void	qDestroy(QUEUE *q)
{
	register QUEUE_R *e, *next;

	for ( e = q->start ; e ; e = next ) {
		next = e->next;
		qFreeRecord(e);
	}

	free(q);
}

void	qRelease(QUEUE *q)
{
	register QUEUE_R *e, *next;
	for ( e = q->start ; e ; e = next ) {
		next = e->next;
		qFreeRecord(e);
	}
	q->queueRecords =
	q->datasSize    = 0;
	q->queueSize    = sizeof(QUEUE);
}

/*
 *	STACK (FIFO/LIFO) Functions
 */

QUEUE_R *	qPush(QUEUE *q, void *data, size_t dataSize)
{
	QUEUE_R	*e;

	if ( ! (e = qNewRecord(data, dataSize)) ) return NULL;

	if ( ! q->start ) {
		q->end         = e;
	} else {
		e->next        = q->start;
		q->start->prev = e;
	}
	
	q->start = e;

	qUpdateCounters(q, e, PLUS);

	return e;
}

QUEUE_R *	qPushR(QUEUE *q, void *data, size_t dataSize)
{
	QUEUE_R	*e;
	
	if ( ! (e = qNewRecord(data, dataSize)) ) return NULL;

	if ( ! q->end ) {
		q->start = e;
	} else {
		e->prev      = q->end;
		q->end->next = e;
	}

	q->end = e;

	qUpdateCounters(q, e, PLUS);

	return e;
}

int	qPop(QUEUE *q, void *data)
{
	QUEUE_R	*e;

	if ( ! q->start ) {
		errno = ENODATA;
		return -1;
	}

	e = q->start;
	qUnlink(q, e);
	memcpy(data, e->data, e->dataSize);
	qFreeRecord(e);

	return 0;
}

int	qPopR(QUEUE *q, void *data)
{
	QUEUE_R	*e;

	if ( ! q->end ) {
		errno = ENODATA;
		return -1;
	}

	e = q->end;
	qUnlink(q, e = q->end);
	memcpy(data, e->data, e->dataSize);
	qFreeRecord(e);

	return 0;
}

/*
 *	Chained records functions
 */

QUEUE_R *	qAddAfter(QUEUE *q, QUEUE_R *ref, void *d, size_t s)
{
	QUEUE_R	*r;
	
	if ( ! (r = qNewRecord(d, s)) ) return NULL;
	
	r->next   = ref->next;
	r->prev   = ref;
	ref->next = r;

	if ( ! r->next ) q->end        = r;
	else		 r->next->prev = r;

	qUpdateCounters(q, r, PLUS);

	return r;
}

QUEUE_R *	qAddBefore(QUEUE *q, QUEUE_R *ref, void *d, size_t s)
{
	QUEUE_R	*r;

	if ( ! (r = qNewRecord(d, s)) ) return NULL;

	r->next   = ref;
	r->prev   = ref->prev;
	ref->prev = r;

	if ( ! r->prev ) q->start      = r;
	else             r->prev->next = r;

	qUpdateCounters(q, r, PLUS);

	return r;
}

void	qDrop(QUEUE *q, QUEUE_R *r)
{
	qFreeRecord(qUnlink(q, r));
}

int	qGet(QUEUE *q, QUEUE_R *r, void *d)
{
	qUnlink(q, r);
	memcpy(d, qRecData(r), qRecDataSize(r));
	qFreeRecord(r);
	return 0;
}
