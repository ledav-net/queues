
#include "queues.h"

#include <stdio.h>
#include <string.h>

int	fill_queue(QUEUE *q, int (*fct)(QUEUE *, void *, size_t), const char *doing)
{
	int	i;

	const char *tests[] = { "55555", "4444", "333", "22", "1", "", NULL };

	printf("** Filling Queue\n");

	for ( i=0 ; tests[i] ; i++ ) {
		printf("%s: %s (%dc)\n", doing, tests[i], strlen(tests[i]));
		fct(q, (void *)tests[i], strlen(tests[i])+1);
	}
	return 0;
}

int	main(void)
{
	QUEUE		*q;
	QUEUE_R		*r, *next;
	char		buf[256];
	
	q = qCreate();

	fill_queue(q, qPush, "PUSH");

	printf("** qPop\n");
	while ( qPop(q, buf) >= 0 )
		printf("POP: %s (%dc)\n", buf, strlen(buf));

	fill_queue(q, qPushR, "PUSHR");

	printf("** qRead\n");
	for ( r = qRead(q, NULL) ; r ; r = qRead(q, r) ) {
		printf("READ: %s (%dc)\n", (char *)qRecData(r), strlen((char *)qRecData(r)));
	}

	printf("** qReadR\n");
	for ( r = qReadR(q, NULL) ; r ; r = qReadR(q, r) ) {
		printf("READR: %s (%dc)\n", (char *)qRecData(r), strlen((char *)qRecData(r)));
	}

	printf("** qPopR\n");
	while ( qPopR(q, buf) >= 0 )
		printf("POPR: %s (%dc)\n", buf, strlen(buf));

	fill_queue(q, qPush,  "PUSHING");

	printf("** qGet\n");
	for ( r = qRead(q, NULL) ; r ; r = next ) {
		next = qRead(q, r);
		qGet(q, r, buf);
		printf("GET: %s (%dc)\n", buf, strlen(buf));
	}
	
        fill_queue(q, qPushR, "PUSHR");

	printf("** qDrop\n");
	for ( r = qRead(q, NULL) ; r ; r = next ) {
		next = qRead(q, r);
		printf("DROP: %s (%dc)\n", (char *)qRecData(r), strlen((char *)qRecData(r)));
		qDrop(q, r);
	}

	qDestroy(q);

	return 0;
}
