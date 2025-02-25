/*
 * Remmina - The GTK+ Remote Desktop Client
 * Copyright (C) 2010 Vic Lee
 * Copyright (C) 2017-2022 Antenore Gatta, Giovanni Panozzo
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

#pragma once

enum RemminaCommandType
{
    REMMINA_COMMAND_NONE = 0,
    REMMINA_COMMAND_MAIN = 1,
    REMMINA_COMMAND_PREF = 2,
    REMMINA_COMMAND_NEW = 3,
    REMMINA_COMMAND_CONNECT = 4,
    REMMINA_COMMAND_EDIT = 5,
    REMMINA_COMMAND_ABOUT = 6,
    REMMINA_COMMAND_VERSION = 7,
    REMMINA_COMMAND_FULL_VERSION = 8,
    REMMINA_COMMAND_PLUGIN = 9,
    REMMINA_COMMAND_EXIT = 10,
    REMMINA_COMMAND_AUTOSTART = 11,
    REMMINA_COMMAND_ENCRYPT_PASSWORD = 12
};

enum RemminaCondExitType
{
    REMMINA_CONDEXIT_ONDISCONNECT = 0,
    REMMINA_CONDEXIT_ONQUIT = 1,
    REMMINA_CONDEXIT_ONMAINWINDELETE = 2
};

void remmina_exec_command( RemminaCommandType command, const char *data );
void remmina_exec_exitremmina();
void remmina_application_condexit( RemminaCondExitType why );

int remmina_exec_set_setting( char *profilefilename, char **settings );
