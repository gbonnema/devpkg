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

#include "shell.h"
#include "dbg.h"
#include "bstrlib.h"
#include <stdarg.h>
#include <stdlib.h>

int Shell_exec(Shell template, ...)
{
	apr_pool_t *p = NULL;
	int rc = -1;
	apr_status_t rv = APR_SUCCESS;
	va_list argp;
	const char *key = NULL;
	const char *arg = NULL;
	char *dest = NULL;
	char *tofree = NULL;
	int i = 0;

	rv = apr_pool_create(&p, NULL);
	check(rv == APR_SUCCESS, "Failed to create pool.");

	va_start(argp, template);

	for(key = va_arg(argp, const char *);
		key != NULL;
		key = va_arg(argp, const char *))
	{
		arg = va_arg(argp, const char *);

		if(template.exe != NULL && template.exe[0] == '!') {
			if(strcmp(template.exe, key) == 0) {
				bstring script = bfromcstr(arg);
				template.exe = tofree = bstr2cstr(script, ' ');
				bdestroy(script);
			}
		}

		for(i = 0; template.args[i] != NULL; i++) {
			if(strcmp(template.args[i], key) == 0) {
				template.args[i] = arg;
				break;	// found it
			}
		}
	}

	rc = Shell_run(p, &template);
	if (dest) free(dest);
	if (tofree) {
		bcstrfree(tofree);
		template.exe = tofree = NULL;
	}
	apr_pool_destroy(p);
	va_end(argp);
	return rc;

error:
	if(dest) free(dest);
	if (tofree) {
		bcstrfree(tofree);
		template.exe = tofree = NULL;
	}
	if(p) {
		apr_pool_destroy(p);
	}
	return rc;
}

int Shell_run(apr_pool_t *p, Shell *cmd)
{
	apr_procattr_t *attr;
	apr_status_t rv;
	apr_proc_t newproc;

	rv = apr_procattr_create(&attr, p);
	check(rv == APR_SUCCESS, "Failed to create proc attr.");

	rv = apr_procattr_io_set(attr, APR_NO_PIPE, APR_NO_PIPE, 
			APR_NO_PIPE);
	check(rv == APR_SUCCESS, "Failed to set IO of command.");

	rv = apr_procattr_dir_set(attr, cmd->dir);
	check(rv == APR_SUCCESS, "Failed to set root to %s", cmd->dir);

	rv = apr_procattr_cmdtype_set(attr, APR_PROGRAM_PATH);
	check(rv == APR_SUCCESS, "Failed to set cmd type.");

	rv = apr_proc_create(&newproc, cmd->exe, cmd->args, NULL, attr, p);
	check(rv == APR_SUCCESS, "Failed to run command.");

	rv = apr_proc_wait(&newproc, &cmd->exit_code, &cmd->exit_why, APR_WAIT);
	check(rv == APR_CHILD_DONE, "Failed to wait.");

	check(cmd->exit_code == 0, "%s exited badly.", cmd->exe);
	check(cmd->exit_why == APR_PROC_EXIT, "%s was killed or crashed", cmd->exe); 

	return 0;

error:
	return -1;
}

char *Shell_filename(Shell shell, const char *fname) {
	char *result = NULL;
	int rc = 0;
	bstring bdir_fname = bfromcstr(PREBUILD_SH.dir);
	bstring bfname = bfromcstr(fname);
	rc = bcatcstr(bdir_fname, "/");
	check(rc == BSTR_OK, "Error during concat of path and / for %s", PREBUILD_SH.dir);
	rc = bconcat(bdir_fname, bfname);
	check(rc == 0, "Allocating pathname from dir %s and file %s failed.", PREBUILD_SH.dir, fname);
	result = bstr2cstr(bdir_fname, ' ');
	// fallthrough
error:
	bdestroy(bfname);
	bdestroy(bdir_fname);
	return result;
}

Shell PREBUILD_SH = {
	.exe = "!NAME",					// !variables are replaced by a specified value
	.dir = "/tmp/pkg-build",
	.args = {"!NAME", NULL}			// !variables are replaced by a specified value
};

Shell CLEANUP_SH = {
	.exe = "rm",
	.dir = "/tmp",
	.args = {"rm", "-rf", "/tmp/pkg-build", "/tmp/pkg-src.tar.gz", 
		"/tmp/pkg-src.tar.bz2", "/tmp/DEPENDS", NULL}
};

Shell GIT_SH = {
	.exe = "git",
	.dir = "/tmp",
	.args = {"git", "clone", "URL", "pkg-build", NULL}
};

Shell TAR_SH = {
	.exe = "tar",
	.dir = "/tmp/pkg-build",
	.args = {"tar", "-xzf", "FILE", "--strip-components", "1", NULL}
};

Shell CURL_SH = {
	.exe = "curl",
	.dir = "/tmp",
	.args = {"curl", "-L", "-o", "TARGET", "URL", NULL}
};

Shell CONFIGURE_SH = {
	.exe = "./configure",
	.dir = "/tmp/pkg-build",
	.args = {"configure", "OPTS", NULL}
};

Shell MAKE_SH = {
	.exe = "make",
	.dir = "/tmp/pkg-build",
	.args = {"make", "OPTS", NULL}
};

Shell INSTALL_SH = {
	.exe = "sudo",
	.dir = "/tmp/pkg-build",
	.args = {"sudo", "make", "TARGET", NULL}
};

