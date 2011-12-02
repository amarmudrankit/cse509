#include <stdio.h>
#include <stdlib.h>

#include <sqlite3.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

static sqlite3* db;
int main(void) {


  struct passwd *pw = getpwuid(getuid());

  const char *home_dir = pw->pw_dir;
	const char * const um_db_filename = "interceptor.db";

	int path_len = strlen(home_dir) + strlen(um_db_filename) + 2;
	char *um_db_filepath = (char *) malloc(path_len);
	memset(um_db_filepath, 0, path_len);
	strcat(um_db_filepath, home_dir);
	strcat(um_db_filepath, "/");
	strcat(um_db_filepath, um_db_filename);

	sqlite3_open(um_db_filepath, &db);

	sqlite3_close(db);
}

