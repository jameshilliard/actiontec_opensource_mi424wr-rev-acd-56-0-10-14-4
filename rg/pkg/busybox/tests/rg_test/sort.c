#include <busybox.h>
const char usage_messages[] = "sort <filename>";
const char *full_version = "OpenRG 1.0";
const struct BB_applet applets[0];
const char *applet_name = "sort";
const char *name_too_long = "Name too long";
const char *memory_exhausted = "Out of memory";
const int NUM_APPLETS = 1;

int main(int argc, char **argv)
{
    sort_main(argc, argv);
}
