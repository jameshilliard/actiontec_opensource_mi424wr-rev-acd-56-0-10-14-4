#ifdef TEST_SOURCE
#include <unistd.h>
#include "test_tool.h"


const char hostname_hosts[] = "192.168.1.2 ab.cd.ef";

int fn_hostname(void)
{
#ifndef BB_HOSTNAME
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;
    static char orig_name[255];

    if (gethostname(orig_name, sizeof(orig_name)))
	end_program(1, "could not gethostname");
    
    activate("hostname abcd", NULL);
    RUN_COMPARE("hostname", "abcd\n");
    RUN_COMPARE("hostname -s", "\n");
    RUN_COMPARE("hostname -d", "\n");
    
    activate("hostname ab.cd.ef", NULL);
    RUN_COMPARE("hostname", "ab.cd.ef\n");
    RUN_COMPARE("hostname -s", "ab\n");
    RUN_COMPARE("hostname -d", "cd.ef\n");

    copy_file("/etc/hosts", "tmp");
    write_file("/etc/hosts", hostname_hosts, sizeof(hostname_hosts), 1, 0);
    RUN_COMPARE("hostname -i", "192.168.1.2\n");
    copy_file("tmp",  "/etc/hosts");

    if (sethostname(orig_name, strlen(orig_name)))
	end_program(1, "could not sethostname");

    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}
#endif
#ifdef SET_ENTRIES
register_test("hostname", fn_hostname, 1);
#endif
