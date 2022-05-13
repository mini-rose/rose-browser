#include "rose.h"

#define MSGBUFSZ 8
#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

guint xid;

void setatom(int a, const char *v)
{
	XChangeProperty(dpy, xid,
									atoms[a], atoms[AtomUTF8], 8, PropModeReplace,
									(unsigned char *)v, strlen(v) + 1);
	XSync(dpy, False);
}

const char* getatom(int a)
{
	static char buf[BUFSIZ];
	Atom adummy;
	int idummy;
	unsigned long ldummy;
	unsigned char *p = NULL;

	XSync(dpy, False);
	XGetWindowProperty(dpy, xid,
	                   atoms[a], 0L, BUFSIZ, False, atoms[AtomUTF8],
	                   &adummy, &idummy, &ldummy, &ldummy, &p);
	if (p)
		strncpy(buf, (char *)p, LENGTH(buf) - 1);
	else
		buf[0] = '\0';
	XFree(p);

	return buf;
}

static void setup()
{
	if (!(dpy = XOpenDisplay(NULL))) {
		puts("Can't open default display");
		exit(1);
	}

	atoms[AtomFind] = XInternAtom(dpy, "_ROSE_FIND", False);
	atoms[AtomGo] = XInternAtom(dpy, "_ROSE_GO", False);
	atoms[AtomUri] = XInternAtom(dpy, "_ROSE_URI", False);
}

static void run(GtkApplication *app)
{
	RoseWindow *window = rose_window_new(app);

	if (appearance[DARKMODE])
		g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", true, NULL);

	if (!options[HOMEPAGE])
		options[HOMEPAGE] = "https://duckduckgo.com";

	xid = rose_window_show(app, window, options[HOMEPAGE]);
}

int main(int argc, char **argv)
{
	if (argc == 2) {
		options[HOMEPAGE] = argv[1];
		argv++; argc--;
	}
	setup();
	GtkApplication *app = gtk_application_new("org.gtk.rose", G_APPLICATION_NON_UNIQUE);
	g_signal_connect(app, "activate", G_CALLBACK(run), NULL);
	g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
}
