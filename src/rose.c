#include "client.h"
#include "lua.h"
#include <stdlib.h>
#include <signal.h>

void at_exit()
{
	rose_client_destroy_all();
}

void rose_setup_gtk()
{
	g_object_set(
		gtk_settings_get_default(),
		"gtk-enable-animations",
		rose_lua_value_boolean("rose.settings.animations"),
		"gtk-application-prefer-dark-theme",
		rose_lua_value_boolean("rose.settings.darkmode"),
	NULL);
}

int main()
{
	/* Setup */
#if GTK == 3
	gtk_init(0, NULL);
#elif GTK == 4
	gtk_init();
#endif
	srand(time(NULL));
	atexit(at_exit);

	signal(SIGINT, (void *)rose_client_destroy_all);
	signal(SIGTERM, (void *)rose_client_destroy_all);
	signal(SIGKILL, (void *)rose_client_destroy_all);

	rose_client_new();
	rose_setup_gtk();

	while (1)
		g_main_context_iteration(NULL, TRUE);

	return 0;
}
