/* vi: set sw=4 ts=4: */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include "busybox.h"

#undef APPLET
#undef APPLET_NOUSAGE
#undef PROTOTYPES
#include "applets.h"

#define bb_need_full_version
#define BB_DECLARE_EXTERN
#include "messages.c"

static int been_there_done_that = 0;


const char *applet_name;

#ifdef BB_FEATURE_INSTALLER
/* 
 * directory table
 *		this should be consistent w/ the enum, busybox.h::Location,
 *		or else...
 */
static char* install_dir[] = {
	"/",
	"/bin",
	"/sbin",
	"/usr/bin",
	"/usr/sbin",
};

/* abstract link() */
typedef int (*__link_f)(const char *, const char *);

/* 
 * Where in the filesystem is this busybox?
 * [return]
 *		malloc'd string w/ full pathname of busybox's location
 *		NULL on failure
 */
static char *busybox_fullpath()
{
	pid_t pid;
#ifdef ACTION_TEC_DIFFSERV
	char path[1024];
#else
	char proc[256];
#endif //ACTION_TEC_DIFFSERV
	int len;

	pid = getpid();
	sprintf(proc, "/proc/%d/exe", pid);
#ifdef ACTION_TEC_DIFFSERV
	len = readlink(proc, path, 1024); 
#else
    len = readlink(proc, path, 256);
#endif// ACTION_TEC_DIFFSERV
	if (len != -1) {
		path[len] = 0;
	} else {
		perror_msg("%s", proc);
		return NULL;
	}
	return strdup(path);
}

/* create (sym)links for each applet */
static void install_links(const char *busybox, int use_symbolic_links)
{
	__link_f Link = link;

#ifdef ACTION_TEC_DIFFSERV
	char command[1024];
#else
    char command[256];
#endif //ACTION_TEC_DIFFSERV

	int i;
	int rc;

	if (use_symbolic_links) 
		Link = symlink;

	for (i = 0; applets[i].name != NULL; i++) {
		sprintf ( command, "%s/%s", 
				install_dir[applets[i].location], applets[i].name);
		rc = Link(busybox, command);

		if (rc) {
			perror_msg("%s", command);
		}
	}
}

#endif /* BB_FEATURE_INSTALLER */

int main(int argc, char **argv)
{
	const char *s;

	for (s = applet_name = argv[0]; *s != '\0';) {
		if (*s++ == '/')
			applet_name = s;
	}

#ifdef BB_SH
	/* Add in a special case hack -- whenever **argv == '-'
	 * (i.e. '-su' or '-sh') always invoke the shell */
	if (**argv == '-' && *(*argv+1)!= '-') {
		exit(((*(shell_main)) (argc, argv)));
	}
#endif

	run_applet_by_name(applet_name, argc, argv);
	error_msg_and_die("applet not found");
}


int busybox_main(int argc, char **argv)
{
	int col = 0;

#ifdef BB_FEATURE_INSTALLER	
	/* 
	 * This style of argument parsing doesn't scale well 
	 * in the event that busybox starts wanting more --options.
	 * If someone has a cleaner approach, by all means implement it.
	 */
	if (argc > 1 && (strcmp(argv[1], "--install") == 0)) {
		int use_symbolic_links = 0;
		int rc = 0;
		char *busybox;

		/* to use symlinks, or not to use symlinks... */
		if (argc > 2) {
			if ((strcmp(argv[2], "-s") == 0)) { 
				use_symbolic_links = 1; 
			}
		}

		/* link */
		busybox = busybox_fullpath();
		if (busybox) {
			install_links(busybox, use_symbolic_links);
			free(busybox);
		} else {
			rc = 1;
		}
		return rc;
	}
#endif /* BB_FEATURE_INSTALLER */

	argc--;

	/* If we've already been here once, exit now */
	if (been_there_done_that == 1 || argc < 1) {
		const struct BB_applet *a = applets;

		fprintf(stderr, "%s\n\n"
				"Usage: busybox [function] [arguments]...\n"
				"   or: [function] [arguments]...\n\n"
				"\tBusyBox is a multi-call binary that combines many common Unix\n"
				"\tutilities into a single executable.  Most people will create a\n"
				"\tlink to busybox for each function they wish to use, and BusyBox\n"
				"\twill act like whatever it was invoked as.\n" 
				"\nCurrently defined functions:\n", full_version);

		while (a->name != 0) {
			col +=
				fprintf(stderr, "%s%s", ((col == 0) ? "\t" : ", "),
						(a++)->name);
			if (col > 60 && a->name != 0) {
				fprintf(stderr, ",\n");
				col = 0;
			}
		}
		fprintf(stderr, "\n\n");
		exit(0);
	}

	/* Flag that we've been here already */
	been_there_done_that = 1;
	
	return (main(argc, argv+1));
}

/*
Local Variables:
c-file-style: "linux"
c-basic-offset: 4
tab-width: 4
End:
*/
