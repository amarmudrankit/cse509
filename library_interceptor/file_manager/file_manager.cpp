/* vim: set ts=2 sw=2 tw=78 et: */
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <map>
#include <string>
#include "file_manager.h"

struct FileManager {
  public:
    // Initialize with scratch space dir.
    FileManager();

    // Returns a tmp file name for the passed filename.
    // In addition it does the following function:
    // -- If the file already exists, it will create a copy of the
    // file apart from creating a tmp file.
    const char* GetTmpFile(const std::string &filename);

    // Delete all tmp file that we have in the map which no longer exists.
    void DeleteTmpFiles();
  private:
    void FileCopy(const char *src, const char *dest);
    std::map<std::string, std::string> tmp_file_map_;
};

FileManager::FileManager() {}

void clean_up_tmp_files(void) {
  if (file_mgr != NULL) {
    static_cast<FileManager*>(file_mgr)->DeleteTmpFiles();
  }
}

const char* fm_get_alt_file(const char *filename) {
  if (file_mgr == NULL) {
    file_mgr = static_cast<void*>(new FileManager());
    atexit(clean_up_tmp_files);
  }
  if (!filename) {
    return filename;
  }
  return static_cast<FileManager*>(file_mgr)->GetTmpFile(filename);
}

void FileManager::FileCopy(const char *src, const char *dest) {
  char buf;
  int src_file, dest_file, n;
  src_file = syscall(SYS_open, src, O_RDONLY);
  if(src_file == -1) {
    perror("Unable to read source file");
    exit(-1);
  }

  struct stat o_stat;

  // TODO: syscall(SYS_stat ... won't work
  int r = stat(src, &o_stat);
  if(r) { 
    perror("Unable to stat destination file.");
    exit(-1);
  }

  dest_file = syscall(SYS_open, dest, O_WRONLY|O_CREAT, (o_stat.st_mode & 0777));
  if (dest_file == -1) {
    perror("Unable to open destination file");
    exit(-1);
  }
  while ((n = syscall(SYS_read, src_file, &buf, 1)) > 0) {
    syscall(SYS_write, dest_file, &buf, 1);
  }
  syscall(SYS_close, src_file);
  syscall(SYS_close, dest_file);
}

const char* FileManager::GetTmpFile(const std::string &filename) {
  std::map<std::string, std::string>::const_iterator file_iter =
    tmp_file_map_.find(filename);
  // If the filename already exists in the map, return the
  // tmp file that we have already created.
  if (file_iter != tmp_file_map_.end()) {
    return file_iter->second.c_str();
  }

  // Add the tmp file to the map and return the reference to the same.
  std::string tmp_file = filename;
  for(int l = 0; l < tmp_file.length(); ++l) {
    if(tmp_file[l] == '/') tmp_file[l] = '_';
  }
  
  tmp_file = "/tmp/" + tmp_file + ".tmp";

  tmp_file_map_[filename] = tmp_file;
  std::cerr << "Inserting: " << filename << " into " << tmp_file << std::endl;

  // Check if the filename exists and if it exists, copy the file to the
  // tmp file.
	if (!syscall(SYS_access, filename.c_str(), F_OK)) {
    FileCopy(filename.c_str(), tmp_file.c_str());
	}

  const char *ret = tmp_file_map_[filename].c_str();

  return ret;
}

// Delete all tmp file that we have in the map which no longer exists.
void FileManager::DeleteTmpFiles() {
  std::map<std::string, std::string>::const_iterator file_iter = tmp_file_map_.begin();
  for (; file_iter != tmp_file_map_.end(); ++file_iter) {
    // Check if the file exists, if it does, then delete.
    const std::string& tmp_filename = file_iter->second;
    if (!syscall(SYS_access, tmp_filename.c_str(), F_OK)) {
      std::cout << "Deleting " + tmp_filename << std::endl;
			syscall(SYS_unlink, tmp_filename.c_str());
    }
  }
}
