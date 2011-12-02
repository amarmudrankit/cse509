#ifndef __INTERCEPTOR_UM_H__
#define __INTERCEPTOR_UM_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <sqlite3.h>

/* bails out if the result from an sqlite function is not what we want it to
 * be.
 */
#define SQLITE_ASSERT(stmt, expected_code) { \
	int sqlite__result__ = (stmt); \
	if(sqlite__result__ != expected_code) {\
		fprintf(stderr, "SQLITE_ERROR: %s.\n", sqlite3_errmsg(um_db_)); \
	}\
	assert(sqlite__result__ == expected_code); \
}

#define SQLITE_ASSERT_OK(stmt) { SQLITE_ASSERT(stmt, SQLITE_OK); }

static const char * const INIT_QUERIES[] = {
	// specifies what type of interception a user wants for a process. perhaps
	// they just want to log, perhaps they want "private browsing" ...
	"CREATE TABLE IF NOT EXISTS user_process(user VARCHAR, process VARCHAR, process_intercept_mode INT)",

	// specifies individual filepath interceptions
	"CREATE TABLE IF NOT EXISTS file_interceptions(filepath VARCHAR, file_intercept_mode INT)",

	"CREATE TABLE IF NOT EXISTS process_mapped_files(original_filepath VARChAR, mapped_filepath VARCHAR)",

	// logging table for all syscalls ever made
	"CREATE TABLE IF NOT EXISTS syscall_log(user INT, syscall_no INT, process VARCHAR, timestamp INT DEFAULT CURRENT_TIMESTAMP)"
};

static const int NUM_INIT_QUERIES = 4;

void shutdown_um(void);

static sqlite3* um_db_;

int UM_ERR_INIT = 1;

/* initializes the user manager
 *
 * this should be called exactly once during the lifetime of the program.
 * perhaps a check should be added for that.
 *
 * it creates a sqlite db in the home directory of the user, and creates the
 * tables. of course, things are done only if required (due to the CREATE
 * TABLE IF NOT EXIST queries).
 */
int initialize_um(void) {
	if(um_db_) return 0;

	struct passwd *pw = getpwuid(getuid());
	assert(pw);
	assert(pw->pw_dir);

	const char *home_dir = pw->pw_dir;
	const char *um_db_filename = ".interceptor.db";

	int path_len = strlen(home_dir) + strlen(um_db_filename) + 2;
	char *um_db_filepath = (char *) malloc(path_len);
	memset(um_db_filepath, 0, path_len);
	strcat(um_db_filepath, home_dir);
	strcat(um_db_filepath, "/");
	strcat(um_db_filepath, um_db_filename);

	if(sqlite3_open(um_db_filepath, &um_db_)) {
		printf("Could not open database %s.\n", um_db_filepath);
		return UM_ERR_INIT;
	}

	for(int i = 0; i < NUM_INIT_QUERIES; ++i) {
		char *err_msg;
		if(sqlite3_exec(um_db_, INIT_QUERIES[i], 0, 0, &err_msg)) {
			printf("Error while executing statement %s.\n", INIT_QUERIES[i]);
			// std::cerr << err_msg << std::endl;
			sqlite3_free(err_msg);
			return UM_ERR_INIT;
		}
	}

	free(um_db_filepath);

	atexit(shutdown_um);

	return 0;
}

static const char * const LOG_SYSCALL_QUERY = "INSERT INTO syscall_log(user, syscall_no, process) VALUES (@user, @syscall_no, @process)";

/* logs a syscall in the database.
 */
int um_log_syscall(int syscall_no, const char * process) {
	assert(!initialize_um());

	assert(process);

	uid_t uid = getuid();
	
	sqlite3_stmt *stmt;
	SQLITE_ASSERT_OK(sqlite3_prepare_v2(um_db_, LOG_SYSCALL_QUERY, -1, &stmt, NULL));

	SQLITE_ASSERT_OK(sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "@user"), uid));
	SQLITE_ASSERT_OK(sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "@syscall_no"), syscall_no));
	SQLITE_ASSERT_OK(sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@process"), process, -1, SQLITE_TRANSIENT));

	// keep doing this till the database is busy or locked.
	int result;
	do {
		result = sqlite3_step(stmt);
	} while ((result & SQLITE_DONE) != SQLITE_DONE && ((result & SQLITE_BUSY) != 0 || (result & SQLITE_LOCKED) != 0));

	SQLITE_ASSERT(result, SQLITE_DONE);

	SQLITE_ASSERT_OK(sqlite3_finalize(stmt));

	return 0;
}

/* clean up for the user manager 
 */
void shutdown_um(void) {
	if(um_db_) sqlite3_close(um_db_);
}

#endif // __INTERCEPTOR_UM_H__
