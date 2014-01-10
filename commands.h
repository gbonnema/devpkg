/*
This program provides functions to build, make and install programs, based on shell utilities.

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


#ifndef _commands_h
#define _commands_h

#include <apr_pools.h>

#define DEPENDS_PATH "/tmp/DEPENDS"
#define TAR_GZ_SRC "/tmp/pkg-src.tar.gz"
#define TAR_BZ2_SRC "/tmp/pkg-src.tar.bz2"
#define BUILD_DIR "/tmp/pkg-build"
#define GIT_PAT "*.git"
#define DEPEND_PAT "*DEPENDS"
#define TAR_GZ_PAT "*.tar.gz"
#define TAR_BZ2_PAT "*.tar.bz2"
#define CONFIG_SCRIPT "/tmp/pkg-build/configure"

enum CommandType {
	COMMAND_NONE, COMMAND_INSTALL, COMMAND_LIST, COMMAND_FETCH,
	COMMAND_INIT, COMMAND_BUILD
};


int Command_fetch(apr_pool_t *p, const char *url, int fetch_only);

int Command_install(apr_pool_t *p, const char *url, const char *configure_opts,
		const char *make_opts, const char *install_opts, const char *prebuild);

int Command_depends(apr_pool_t *p, const char *path);

int Command_build(apr_pool_t *p, const char *url, const char *configure_opts,
		const char *make_opts, const char *install_opts, const char *prebuild);

#endif
