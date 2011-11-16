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

const char* FileManager::GetTmpFile(const std::string &filename) {
  std::map<std::string, std::string>::const_iterator file_iter =
    tmp_file_map_.find(filename);
  // If the filename already exists in the map, return the
  // tmp file that we have already created.
  if (file_iter != tmp_file_map_.end()) {
    return file_iter->second.c_str();
  }

  // Add the tmp file to the map and return the reference to the same.
  std::string tmp_file = filename + ".tmp";
  tmp_file_map_[filename] = tmp_file;
  std::cout << "Inserting: " << filename << std::endl;

  // Check if the filename exists and if it exists, copy the file to the
  // tmp file.

  // TODO:rohith

  return tmp_file_map_[filename].c_str();
}

// Delete all tmp file that we have in the map which no longer exists.
void FileManager::DeleteTmpFiles() {
  std::map<std::string, std::string>::const_iterator file_iter = tmp_file_map_.begin();
  for (; file_iter != tmp_file_map_.end(); ++file_iter) {
    // Check if the file exists, if it does, then delete.
    const std::string& tmp_filename = file_iter->second;
    std::cout << "Deleting " + tmp_filename << std::endl;
  }
}
