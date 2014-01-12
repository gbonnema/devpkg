/*
This program provides functions to run shell commands using templates (provided in shell.c)

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


#ifndef _shell_h
#define _shell_h

#define MAX_COMMAND_ARGS 100

#include <apr_thread_proc.h>

typedef struct Shell {
	const char *dir;
	const char *exe;

	apr_procattr_t *attr;
	apr_proc_t proc;
	apr_exit_why_e exit_why;
	int exit_code;

	const char *args[MAX_COMMAND_ARGS];
} Shell;

int Shell_run(apr_pool_t *p, Shell *cmd);
int Shell_exec(Shell cmd, ...);
char *Shell_filename(Shell cmd, const char *fname);

extern Shell CLEANUP_SH;
extern Shell GIT_SH;
extern Shell TAR_SH;
extern Shell CURL_SH;
extern Shell CONFIGURE_SH;
extern Shell MAKE_SH;
extern Shell PREBUILD_SH;
extern Shell INSTALL_SH;

#endif
