#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syscall.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */


#include "syscall_handler.h"

#define MAX_LOG_MSG_LEN 1000
__thread char debug_msg_buf[MAX_LOG_MSG_LEN];

void syscall_handler_pre(unsigned int *eax_ptr, unsigned int *ebx_ptr, unsigned int *ecx_ptr, 
		unsigned int *edx_ptr, unsigned int *esi_ptr, unsigned int *edi_ptr) {


}


void syscall_handler_post(unsigned int syscall_no, unsigned int *eax_ptr, unsigned int *ebx_ptr, 
		unsigned int *ecx_ptr, unsigned int *edx_ptr, unsigned int *esi_ptr, unsigned int *edi_ptr) {

	int pFile = syscall(SYS_open, "/tmp/test_library_interceptor", O_WRONLY|O_APPEND|O_CREAT, S_IRUSR|S_IWUSR);
	if (pFile > 0) {
		int byteCount = snprintf(debug_msg_buf, MAX_LOG_MSG_LEN, "System call %d is called, result: %d\n", syscall_no, (int)*eax_ptr);
		syscall(SYS_write, pFile, debug_msg_buf, byteCount);
		syscall(SYS_close, pFile);
	}                           
}



