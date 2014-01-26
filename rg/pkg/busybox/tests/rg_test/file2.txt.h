#ifdef DECL_FILE_BUFFER
char file2_txt[] = 
{
#include "file2.txt.val"
};
#endif
#ifdef WRITE_FILE
write_file("file2.txt", file2_txt, sizeof(file2_txt), 1, 0);
#endif
