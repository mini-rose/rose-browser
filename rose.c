#include "rose.h"

#define MSGBUFSZ 8
#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

guint xid;

void
sigchld(int unused)
{
	if (signal(SIGCHLD, sigchld) == SIG_ERR) {
		puts("Can't install SIGCHLD handler");
		exit(1);
	}
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
}

void
setatom(int a, const char *v)
{
	XChangeProperty(dpy, xid,
									atoms[a], atoms[AtomUTF8], 8, PropModeReplace,
									(unsigned char *)v, strlen(v) + 1);
	XSync(dpy, False);
}

static gboolean
readsock(GIOChannel *s, GIOCondition ioc, gpointer unused)
{
	static char msg[MSGBUFSZ];
	GError *gerr = NULL;
	gsize msgsz;

	if (g_io_channel_read_chars(s, msg, sizeof(msg), &msgsz, &gerr) !=
	    G_IO_STATUS_NORMAL) {
		if (gerr) {
			fprintf(stderr, "surf: error reading socket: %s\n",
			        gerr->message);
			g_error_free(gerr);
		}
		return TRUE;
	}

	if (msgsz < 2) {
		fprintf(stderr, "surf: message too short: %lu\n", msgsz);
		return TRUE;
	}

	return TRUE;
}

const char *
getatom(int a)
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
	GIOChannel *gchanin;

	sigchld(0);

	if (!(dpy = XOpenDisplay(NULL))) {
		puts("Can't open default display");
		exit(1);
	}

	atoms[AtomFind] = XInternAtom(dpy, "_ROSE_FIND", False);
	atoms[AtomGo] = XInternAtom(dpy, "_ROSE_GO", False);
	atoms[AtomUri] = XInternAtom(dpy, "_ROSE_URI", False);
	atoms[AtomUTF8] = XInternAtom(dpy, "UTF8_STRING", False);

	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, spair) < 0) {
		fputs("Unable to create sockets\n", stderr);
		spair[0] = spair[1] = -1;
	} else {
		gchanin = g_io_channel_unix_new(spair[0]);
		g_io_channel_set_encoding(gchanin, NULL, NULL);
		g_io_channel_set_flags(gchanin, g_io_channel_get_flags(gchanin)
		                       | G_IO_FLAG_NONBLOCK, NULL);
		g_io_channel_set_close_on_unref(gchanin, TRUE);
		g_io_add_watch(gchanin, G_IO_IN, readsock, NULL);
	}
}

static void run(GtkApplication *app)
{
	RoseWindow *window = rose_window_new(app);

	if (dark_mode) {
		g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", true, NULL);
	}

	xid = rose_window_show(app, window);
	setatom(AtomUri, homepage);
}

int main(int argc, char **argv)
{
	if (argc == 2) {
		homepage = argv[1];
		argv++; argc--;
	}

	setup();
	GtkApplication *app = gtk_application_new("org.gtk.rose", G_APPLICATION_NON_UNIQUE);
	g_signal_connect(app, "activate", G_CALLBACK(run), NULL);
	g_application_run(G_APPLICATION(app), argc, argv);
}
