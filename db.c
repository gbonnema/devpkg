/*
This program assists in adding lines to a file.

Copyright (C) 2014  Zed A. Shaw		
			  2014  Guus Bonnema	Copied program in Jan 2014 from Zed Shaw

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <unistd.h>
#include <stdio.h>
#include <apr_errno.h>
#include <apr_file_io.h>

#include "db.h"
#include "bstrlib.h"
#include "dbg.h"

static FILE *DB_open(const char *path, const char *mode)
{
	return fopen(path, mode);
}


static void DB_close(FILE *db)
{
	fclose(db);
}


static bstring DB_load()
{
	FILE *db = NULL;
	bstring data = NULL;
	
	db = DB_open(DB_FILE, "r");
	check(db, "Failed to open database: %s", DB_FILE);

	data = bread((bNread)fread, db);
	check(data, "Failed to read from db file: %s", DB_FILE);

	DB_close(db);
	return data;

error:
	if(db) DB_close(db);
	if(data) bdestroy(data);
	return NULL;
}


int DB_update(const char *url)
{
	if(DB_find(url)) {
		log_info("Already recorded as installed: %s", url);
	}

	FILE *db = DB_open(DB_FILE, "a+");
	check(db, "Failed to open DB file: %s", DB_FILE);

	bstring line = bfromcstr(url);
	bconchar(line, '\n');
	int rc = fwrite(line->data, blength(line), 1, db);
	check(rc == 1, "Failed to append to the db.");

	return 0;
error:
	if(db) DB_close(db);
	return -1;
}


int DB_find(const char *url)
{
	bstring data = NULL;
	bstring line = bfromcstr(url);
	int res = -1;

	data = DB_load();
	check(data, "Failed to load: %s", DB_FILE);

	if(binstr(data, 0, line) == BSTR_ERR) {
		res = 0;
	} else {
		res = 1;
	}

error: // fallthrough
	if(data) bdestroy(data);
	if(line) bdestroy(line);

	return res;
}


int DB_init()
{
	apr_pool_t *p = NULL;
	apr_pool_initialize();
	apr_pool_create(&p, NULL);

	if(access(DB_DIR, W_OK | X_OK) == -1) {
		apr_status_t rc = apr_dir_make_recursive(DB_DIR,
				APR_UREAD | APR_UWRITE | APR_UEXECUTE |
				APR_GREAD | APR_GWRITE | APR_GEXECUTE, p);
		check(rc == APR_SUCCESS, "Failed to make database dir: %s", DB_DIR);
	}

	if(access(DB_FILE, W_OK) == -1) {
		FILE *db = DB_open(DB_FILE, "w");
		check(db, "Cannot open database: %s", DB_FILE);
		DB_close(db);
	}

	apr_pool_destroy(p);
	return 0;

error:
	apr_pool_destroy(p);
	return -1;
}


int DB_list()
{
	bstring data = DB_load();
	check(data, "Failed to read load: %s", DB_FILE);

	printf("%s", bdata(data));
	bdestroy(data);
	return 0;

error:
	return -1;
}

