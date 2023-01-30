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
