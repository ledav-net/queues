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
#include "queues.h"

#include <stdio.h>
#include <string.h>

int	fill_queue(QUEUE *q, QUEUE_R * (*fct)(QUEUE *, const void *, size_t), const char *doing)
{
	int	i;

	const char *tests[] = { "55555", "4444", "333", "22", "1", "", NULL };

	printf("** Filling Queue\n");

	for ( i=0 ; tests[i] ; i++ ) {
		printf("%s: %s (%ldc)\n", doing, tests[i], strlen(tests[i]));
		fct(q, (void *)tests[i], strlen(tests[i])+1);
	}

	printf("** qRecords = %d, qSize = %ld, qDatasSize = %lu\n", qRecords(q), qSize(q), qDatasSize(q));

	return 0;
}

int	main(void)
{
	QUEUE		*q;
	QUEUE_R		*r, *next;
	char		buf[256];
	
	q = qCreate();

/**/	fill_queue(q, qPush, "PUSH");

	printf("** qPop\n");
	while ( qPop(q, buf) >= 0 ) printf("POP: %s (%ldc)\n", buf, strlen(buf));

	printf("** qRecords = %d, qSize = %ld, qDatasSize = %lu\n", qRecords(q), qSize(q), qDatasSize(q));

/**/	fill_queue(q, qPushR, "PUSHR");

	printf("** qReadFirst/Next\n");
	for ( r = qReadFirst(q) ; r ; r = qReadNext(r) )
		printf("READ: %s (%ldc)\n", (char *)qRecData(r), strlen((char *)qRecData(r)));

	printf("** qRecords = %d, qSize = %ld, qDatasSize = %lu\n", qRecords(q), qSize(q), qDatasSize(q));

/**/	printf("** qReadLast/Prev\n");
	for ( r = qReadLast(q) ; r ; r = qReadPrev(r) )
		printf("READR: %s (%ldc)\n", (char *)qRecData(r), strlen((char *)qRecData(r)));

	printf("** qRecords = %d, qSize = %ld, qDatasSize = %lu\n", qRecords(q), qSize(q), qDatasSize(q));

/**/	printf("** qPopR\n");
	while ( qPopR(q, buf) >= 0 ) printf("POPR: %s (%ldc)\n", buf, strlen(buf));

	printf("** qRecords = %d, qSize = %ld, qDatasSize = %lu\n", qRecords(q), qSize(q), qDatasSize(q));

/**/	fill_queue(q, qPush,  "PUSH");

	printf("** qReadFirst/Next + qGet\n");
	for ( r = qReadFirst(q) ; r ; r = next ) {
		next = qReadNext(r);
		qGet(q, r, buf);
		printf("GET: %s (%ldc)\n", buf, strlen(buf));
	}

	printf("** qRecords = %d, qSize = %ld, qDatasSize = %lu\n", qRecords(q), qSize(q), qDatasSize(q));

/**/	fill_queue(q, qPushR, "PUSHR");

	printf("** qReadFirst/Next + qDrop\n");
	for ( r = qReadFirst(q) ; r ; r = next ) {
		next = qReadNext(r);
		printf("DROP: %s (%ldc)\n", (char *)qRecData(r), strlen((char *)qRecData(r)));
		qDrop(q, r);
	}

	printf("** qRecords = %d, qSize = %ld, qDatasSize = %lu\n", qRecords(q), qSize(q), qDatasSize(q));

/**/	fill_queue(q, qPush,  "PUSH");

	printf("** qRelease\n");
	qRelease(q);

	printf("** qRecords = %d, qSize = %ld, qDatasSize = %lu\n", qRecords(q), qSize(q), qDatasSize(q));

	qDestroy(q);

	return 0;
}
