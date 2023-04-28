#include "client.h"
#include <stdlib.h>

void at_exit()
{
	rose_client_destroy_all();
}

int main()
{
	/* Setup */
	gtk_init();
	srand(time(NULL));
	atexit(at_exit);

	rose_client_new();

	while (1)
		g_main_context_iteration(NULL, TRUE);

	return 0;
}
