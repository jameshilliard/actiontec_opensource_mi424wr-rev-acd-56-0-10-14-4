#ifdef DECL_FILE_BUFFER
char file1_txt[] = 
{
#include "file1.txt.val"
};
#endif
#ifdef WRITE_FILE
write_file("file1.txt", file1_txt, sizeof(file1_txt), 1, 0);
#endif
