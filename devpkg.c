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


#include <stdio.h>
#include <apr_general.h>
#include <apr_getopt.h>
#include <apr_strings.h>
#include <apr_lib.h>

#include "dbg.h"
#include "db.h"
#include "commands.h"

int main(int argc, const char const *argv[])
{
	apr_pool_t *p = NULL;
	apr_pool_initialize();
	apr_pool_create(&p, NULL);

	apr_getopt_t *opt;
	apr_status_t rv;

	char ch = '\0';
	const char *optarg = NULL;
	const char *config_opts = NULL;
	const char *install_opts = NULL;
	const char *make_opts = NULL;
	const char *url = NULL;
	enum CommandType request = COMMAND_NONE;


	rv = apr_getopt_init(&opt, p, argc, argv);

	while(apr_getopt(opt, "I:Lc:m:i:d:SF:B:", &ch, &optarg) == APR_SUCCESS) {
		switch (ch) {
			case 'I':
				request = COMMAND_INSTALL;
				url = optarg;
				break;

			case 'L':
				request = COMMAND_LIST;
				break;

			case 'c':
				config_opts = optarg;
				break;

			case 'm':
				make_opts = optarg;
				break;

			case 'i':
				install_opts = optarg;
				break;

			case 'S':
				request = COMMAND_INIT;
				break;
			
			case 'F':
				request = COMMAND_FETCH;
				url = optarg;
				break;

			case 'B':
				request = COMMAND_BUILD;
				url = optarg;
				break;
		}
	}

	switch(request) {
		case COMMAND_INSTALL:
			check(url, "You must at least give a URL.");
			Command_install(p, url, config_opts, make_opts, install_opts);
			break;

		case COMMAND_LIST:
			DB_list();
			break;

		case COMMAND_FETCH:
			check(url != NULL, "You must give a URL.");
			Command_fetch(p, url, 1);
			log_info("Downloaded to %s and in /tmp/", BUILD_DIR);
			break;

		case COMMAND_BUILD:
			check(url, "You must at least give a URL.");
			Command_build(p, url, config_opts, make_opts, install_opts);
			break;

		case COMMAND_INIT:
			rv = DB_init();
			check(rv == 0, "Failed to make the database.");
			break;

		default:

			sentinel("\nUSAGE: devpkg <command> [<options>]"
					 "\n\tCommands:"
					 "\n\t-I <url> \tInstall package from <url>"
					 "\n\t-L\t\tList installed packages"
					 "\n\t-S\t\tInitialize package database"
					 "\n\t-F <url>\tFetch package from <url>"
					 "\n\t-B <url>\tBuild package from <url>"
					 "\n\n\tinstall options:"
					 "\n\t-c <config-opts>"
					 "\n\t-m <make-opts>"
					 "\n\t-i <install-opts>"
					);
	}


	return 0;

error:
	return 1;
}
