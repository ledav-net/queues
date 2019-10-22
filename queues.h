/*
 * Copyright (C) 2009 David De Grave <david@ledav.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __QUEUES_H
#define __QUEUES_H

#include <stddef.h>

/*                        _______________________
 *                       /          Q01          \         Q = Queue (1)
 *                       \_______________________/         R = Record (3)
 *                        | R01 | | R02 | | R03 |          D = Data (2)
 *                        |_____| |-----| |-----|
 *                        |_____| | D02 | | D03 |
 *                                |     | |_____|
 *                       ^        |_____|        ^
 *                       |       ^   ^   ^       |
 *                       |        \  |  /        |
 * Chain Methods         |qAddBefore | qAddAfter |
 *              qReadFirst         qGet          qReadLast
 *               qReadNext ->      qDrop      <- qReadPrev
 * Stack Methods         |                       |
 *                   qPush                       qPushR
 *                    qPop                       qPopR
 */

struct _queueRecord	{	struct _queueRecord	*prev,
							*next;
				size_t			dataSize;
				char			data[0];
			};

struct _queue		{	struct _queueRecord	*start,
							*end;
				size_t			datasSize,
							queueSize;
				int			queueRecords;
			};

typedef struct _queue		QUEUE;
typedef struct _queueRecord	QUEUE_R;

#define qEmpty(q)		(!qRecords(q))
#define qRecords(q)		((q)->queueRecords)
#define qSize(q)		((q)->queueSize)
#define qDatasSize(q)		((q)->datasSize)

#define qRecData(r)		qRecDataType(r,void)
#define qRecDataType(r,t)	((t*)&(r)->data)
#define qRecDataSize(r)		((r)->dataSize)

extern QUEUE *		qCreate		(void);			/* Create a new empty queue					*/
extern void		qDestroy	(QUEUE *);		/* Destroy a queue						*/
extern void		qRelease	(QUEUE *);		/* Empty the queue						*/

/* STACK (FIFO/LIFO) FUNCTIONS */

extern QUEUE_R *	qPush		(QUEUE *		/* Add a new record with the specified data on top of the queue */
						, void *	/* Pointer to the data	 					*/
						, size_t);	/* Data	size							*/

extern QUEUE_R *	qPushR		(QUEUE *		/* Add a new record to the end					*/
						, void *	/* Pointer to the data							*/
						, size_t);	/* Data size							*/

extern int		qPop		(QUEUE *		/* Get and remove the top element				*/
						, void *);	/* Destination buffer						*/

extern int		qPopR		(QUEUE *		/* Get and remove the last element				*/
						, void *);	/* Destination buffer						*/

/* CHAINED LIST FUNCTIONS */

#define qReadNext(r)	((r)->next)
#define qReadPrev(r)	((r)->prev)
#define qReadFirst(q)	((q)->start)
#define qReadLast(q)	((q)->end)

#define qAddNext	qAddAfter
extern QUEUE_R *	qAddAfter	(QUEUE *		/* Add a new record after the one specified.			*/
						, QUEUE_R *	/* Queue record reference to add after				*/
						, void *	/* Data								*/
						, size_t);	/* Data size							*/
#define qAddPrev	qAddBefore
extern QUEUE_R *	qAddBefore	(QUEUE *		/* Add a new record before the one specified.			*/
						, QUEUE_R *	/* Queue record reference to add before				*/
						, void *	/* Data								*/
						, size_t);	/* Data size							*/
#define qPopThis	qGet
extern int		qGet		(QUEUE *		/* Get the specified data out of the record/queue (pop).	*/
						, QUEUE_R *	/* Record							*/
						, void *);	/* where to store the data					*/

extern void		qDrop		(QUEUE *		/* Drop the specified record out of the queue (pop/free).	*/
						, QUEUE_R *);	/* Record							*/

#endif
