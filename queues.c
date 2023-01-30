/* SPDX-License-Identifier: MIT
 *
 * Copyright 2022 David De Grave <david@ledav.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "queues.h"

static QUEUE_R *	qNewRecord(const void *data, size_t dataSize)
{
	QUEUE_R *e;

	if ( ! (e = malloc(sizeof(QUEUE_R) + dataSize)) ) return NULL;

	e->prev     =
	e->next     = NULL;
	e->dataSize = dataSize;
	memcpy(e->data, data, dataSize);

	return e;
}

#define MINUS		-1
#define PLUS		 1

#define qFreeRecord(r)	free(r)

static void	qUpdateCounters(QUEUE *q, const QUEUE_R *r, int multi)
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
	memset(q, 0, sizeof(QUEUE));
	q->queueSize = sizeof(QUEUE);
}

/*
 *	STACK (FIFO/LIFO) Functions
 */

QUEUE_R *	qPush(QUEUE *q, const void *data, size_t dataSize)
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

QUEUE_R *	qPushR(QUEUE *q, const void *data, size_t dataSize)
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

QUEUE_R *	qAddAfter(QUEUE *q, QUEUE_R *ref, const void *d, size_t s)
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

QUEUE_R *	qAddBefore(QUEUE *q, QUEUE_R *ref, const void *d, size_t s)
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
