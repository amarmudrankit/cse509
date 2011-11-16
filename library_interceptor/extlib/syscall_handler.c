#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syscall.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include "../file_manager/file_manager.h"


#include "syscall_handler.h"

#define LOG 1

#define MAX_LOG_MSG_LEN 1000
__thread char debug_msg_buf[MAX_LOG_MSG_LEN];

void syscall_handler_pre(unsigned int *eax_ptr, unsigned int *ebx_ptr, unsigned int *ecx_ptr, 
		unsigned int *edx_ptr, unsigned int *esi_ptr, unsigned int *edi_ptr) {
	unsigned int syscall_no = *eax_ptr;

#ifdef LOG
	int pFile = syscall(SYS_open, "/tmp/test_library_interceptor", O_WRONLY|O_APPEND|O_CREAT, S_IRUSR|S_IWUSR);
	if (pFile > 0) {
		int byteCount = snprintf(debug_msg_buf, MAX_LOG_MSG_LEN, "Pre:System call %d is called\n", syscall_no);
		syscall(SYS_write, pFile, debug_msg_buf, byteCount);
	} 
#endif

	if(syscall_no == SYS_open) {
		char *old_ptr = (char*)(*ebx_ptr);
		const char *alt_ptr = fm_get_alt_file(old_ptr);
		*ebx_ptr = (unsigned int) alt_ptr;

#ifdef LOG
		if(pFile > 0) {
			int byteCount = snprintf(debug_msg_buf, MAX_LOG_MSG_LEN, "File name: %s swapped %s\n", old_ptr, alt_ptr);
			syscall(SYS_write, pFile, debug_msg_buf, byteCount);
		}
#endif
	}

#ifdef LOG
	if(pFile > 0) {
		syscall(SYS_close, pFile);
	}
#endif
}


void syscall_handler_post(unsigned int syscall_no, unsigned int *eax_ptr, unsigned int *ebx_ptr, 
		unsigned int *ecx_ptr, unsigned int *edx_ptr, unsigned int *esi_ptr, unsigned int *edi_ptr) {

#ifdef LOG
	int pFile = syscall(SYS_open, "/tmp/test_library_interceptor", O_WRONLY|O_APPEND|O_CREAT, S_IRUSR|S_IWUSR);
	if (pFile > 0) {
		int byteCount = snprintf(debug_msg_buf, MAX_LOG_MSG_LEN, "Post:System call %d is called, result: %d\n", syscall_no, (int)*eax_ptr);
		syscall(SYS_write, pFile, debug_msg_buf, byteCount);
		syscall(SYS_close, pFile);
	}
#endif
}



