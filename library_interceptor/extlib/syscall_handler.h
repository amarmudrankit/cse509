#ifndef __SYSHANDLER_H__
#define __SYSHANDLER_H__

void syscall_handler_pre(unsigned int *eax_ptr, unsigned int *ebx_ptr, unsigned int *ecx_ptr, unsigned int *edx_ptr, unsigned int *esi_ptr, unsigned int *edi_ptr); 

void syscall_handler_post(unsigned int syscall_no, unsigned int *eax_ptr, unsigned int *ebx_ptr, unsigned int *ecx_ptr, unsigned int *edx_ptr, unsigned int *esi_ptr, unsigned int *edi_ptr);

#endif /* __SYSHANDLER_H__ */
 
