/*
 * Remmina - The GTK+ Remote Desktop Client
 * Copyright (C) 2009-2011 Vic Lee
 * Copyright (C) 2014-2015 Antenore Gatta, Fabio Castelli, Giovanni Panozzo
 * Copyright (C) 2016-2022 Antenore Gatta, Giovanni Panozzo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 *  In addition, as a special exception, the copyright holders give
 *  permission to link the code of portions of this program with the
 *  OpenSSL library under certain conditions as described in each
 *  individual source file, and distribute linked combinations
 *  including the two.
 *  You must obey the GNU General Public License in all respects
 *  for all of the code used other than OpenSSL. *  If you modify
 *  file(s) with this exception, you may extend this exception to your
 *  version of the file(s), but you are not obligated to do so. *  If you
 *  do not wish to do so, delete this exception statement from your
 *  version. *  If you delete this exception statement from all source
 *  files in the program, then also delete it here.
 *
 */

#include <glib-object.h>
#include <gobject/gvaluecollector.h>

#include "remmina/types.hpp"

#pragma once



struct _RemminaFile
{
    char *filename;
    // @todo Add a cache file with content remminafile->filename = last_success
    char *statefile;
    GHashTable *settings;
    GHashTable *states;
    GHashTable *spsettings;
    bool prevent_saving;
};

/**
 * used in remmina_ssh.cpp and remmina_ssh_plugin.cpp
 *
 * #define SSH_AUTH_METHOD_UNKNOWN     0x0000u
 * #define SSH_AUTH_METHOD_NONE        0x0001u
 * #define SSH_AUTH_METHOD_PASSWORD    0x0002u
 * #define SSH_AUTH_METHOD_PUBLICKEY   0x0004u
 * #define SSH_AUTH_METHOD_HOSTBASED   0x0008u
 * #define SSH_AUTH_METHOD_INTERACTIVE 0x0010u
 * #define SSH_AUTH_METHOD_GSSAPI_MIC  0x0020u
 */
enum
{
    SSH_AUTH_PASSWORD,
    SSH_AUTH_PUBLICKEY,
    SSH_AUTH_AGENT,
    SSH_AUTH_AUTO_PUBLICKEY,
    SSH_AUTH_GSSAPI,
    SSH_AUTH_KBDINTERACTIVE
};

#define TOOLBAR_OPACITY_LEVEL 8
#define TOOLBAR_OPACITY_MIN 0.2

/* Create a empty .remmina file */
RemminaFile *remmina_file_new( void );
RemminaFile *remmina_file_copy( const char *filename );
void remmina_file_generate_filename( RemminaFile *remminafile );
void remmina_file_set_filename( RemminaFile *remminafile, const char *filename );
void remmina_file_set_statefile( RemminaFile *remminafile );
void remmina_file_state_last_success( RemminaFile *remminafile );
const char *remmina_file_get_filename( RemminaFile *remminafile );
const char *remmina_file_get_statefile( RemminaFile *remminafile );
/* Load a new .remmina file and return the allocated RemminaFile object */
RemminaFile *remmina_file_load( const char *filename );
/* Settings get/set functions */
void remmina_file_set_string( RemminaFile *remminafile, const char *setting, const char *value );
void remmina_file_set_string_ref( RemminaFile *remminafile, const char *setting, char *value );
const char *remmina_file_get_string( RemminaFile *remminafile, const char *setting );
char *remmina_file_get_secret( RemminaFile *remminafile, const char *setting );
char *remmina_file_format_properties( RemminaFile *remminafile, const char *setting );
void remmina_file_set_int( RemminaFile *remminafile, const char *setting, gint value );
gint remmina_file_get_int( RemminaFile *remminafile, const char *setting, gint default_value );
void remmina_file_store_secret_plugin_password( RemminaFile *remminafile, const char *key, const char *value );
int remmina_file_remove_key( RemminaFile *remminafile, const char *setting );
void remmina_file_set_state( RemminaFile *remminafile, const char *setting, const char *value );
const char *remmina_file_get_state( RemminaFile *remminafile, const char *setting );
void remmina_file_set_state_int( RemminaFile *remminafile, const char *setting, gint value );
gint remmina_file_get_state_int( RemminaFile *remminafile, const char *setting, gint default_value );
gdouble remmina_file_get_state_double( RemminaFile *remminafile, const char *setting, gdouble default_value );
/* Create or overwrite the .remmina file */
void remmina_file_save( RemminaFile *remminafile );
/* Free the RemminaFile object */
void remmina_file_free( RemminaFile *remminafile );
/* Duplicate a RemminaFile object */
RemminaFile *remmina_file_dup( RemminaFile *remminafile );
/* Get the protocol icon name */
const char *remmina_file_get_icon_name( RemminaFile *remminafile );
/* Duplicate a temporary RemminaFile and change the protocol */
RemminaFile *remmina_file_dup_temp_protocol( RemminaFile *remminafile, const char *new_protocol );
/* Delete a .remmina file */
void remmina_file_delete( const char *filename );
/* Delete a "password" field and save into .remmina file */
void remmina_file_unsave_passwords( RemminaFile *remminafile );
/* Function used to update the atime and mtime of a given remmina file, partially
 * taken from suckless sbase */
char *remmina_file_get_datetime( RemminaFile *remminafile );
/* Function used to update the atime and mtime of a given remmina file */
void remmina_file_touch( RemminaFile *remminafile );


