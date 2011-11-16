#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

#include <stdlib.h>

static void *file_mgr = NULL;

#ifdef __cplusplus
extern "C" {
#endif
  void clean_up_tmp_files(void);
  const char* fm_get_alt_file(const char *filename);
#ifdef __cplusplus
}
#endif

#endif // __FILE_MANAGER_H__
