#include "rose.h"
#include <stdarg.h>

#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

Display *glob_dpy; /* defined in rose.h */

static guint glob_xid;
static Atom glob_atoms[AtomLast];

union atomdummies
{
	unsigned long l;
	char *str;
	Atom a;
	int i;
};

void setatom(int a, const char *v)
{
	XChangeProperty(glob_dpy, glob_xid, glob_atoms[a], glob_atoms[AtomUTF8], 8,
		PropModeReplace, (unsigned char *) v, strlen(v) + 1);
	XSync(glob_dpy, False);
}

const char *getatom(int a)
{
	union atomdummies dum;

	static char buf[2000];
	unsigned char *p = NULL;

	XSync(glob_dpy, False);
	XGetWindowProperty(glob_dpy, glob_xid, glob_atoms[a], 0L, 8, False,
			glob_atoms[AtomUTF8], &dum.a, &dum.i, &dum.l, &dum.l, &p);

	if (p)
		strncpy(buf, (char *)p, LENGTH(buf) - 1);
	else
		buf[0] = '\0';

	XFree(p);
	return buf;
}

static void setup()
{
	if (!(glob_dpy = XOpenDisplay(NULL))) {
		puts("Can't open default display");
		exit(1);
	}

	glob_atoms[AtomFind] = XInternAtom(glob_dpy, "_ROSE_FIND", False);
	glob_atoms[AtomGo] = XInternAtom(glob_dpy, "_ROSE_GO", False);
	glob_atoms[AtomUri] = XInternAtom(glob_dpy, "_ROSE_URI", False);
}

static void run(GtkApplication *a, char *url)
{
	va_list args;
	RoseWindow *window;

	/* We need to pass our instance of options, because it's a static variable
	   so each file gets its own instace. */
	window = rose_window_new(a, url);

	if (appearance[DARKMODE])
		g_object_set(gtk_settings_get_default(),
				"gtk-application-prefer-dark-theme", true, NULL);

	if (!appearance[ANIMATIONS])
		g_object_set(gtk_settings_get_default(), "gtk-enable-animations", false,
				NULL);

	glob_xid = rose_window_show(window);
}

int main(int argc, char **argv)
{
	/* Setup XAtoms */
	setup();

	GtkApplication *a = gtk_application_new("org.gtk.rose",
			G_APPLICATION_NON_UNIQUE);

	g_signal_connect(
		a, "activate", G_CALLBACK(run), (argc > 1) ? argv[1] : NULL);

	g_application_run(G_APPLICATION(a), 0, NULL);
	g_object_unref(a);
}
