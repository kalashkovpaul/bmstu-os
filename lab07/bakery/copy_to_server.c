/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "bakery.h"

static int choosing[N];
static int num[N] = {0};
static char alpha = 'A';


int get_number()
{
	int max = 0;
	for (int i = 0; i < N; i++)
	{
		if (num[i] > max)
		{
			max = num[i];
		}
	}
	return max + 1;
}

struct BAKERY *
bakery_proc_1_svc(struct BAKERY *argp, struct svc_req *rqstp)
{
	static struct BAKERY result;

	int i = argp->id;
	result.id = argp->id;

	if (argp->number == -1) {
		choosing[i] = 1;
		num[i] = get_number();
		choosing[i] = 0;
		result.number = num[i];
		return &result;
	}


	for (int j = 0; j < N; j++)
	{
		while(choosing[j]) {/*nothing*/}
		while (num[j] != 0 && (num[j] < num[i] || (num[j] == num[i] && j < i))) {/*nothing*/}
		if (i == j) {
			result.letter = alpha;
			printf("Посетитель %d получил букву %c\n", i, alpha);
			alpha++;
			if (alpha > 'Z')
			{
				alpha = 'A';
			}
			num[i] = 0;
			break;
		}
	}
	return &result;
}
