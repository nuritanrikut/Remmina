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

#pragma once

#define REMMINA_TYPE_FILE_EDITOR ( remmina_file_editor_get_type() )
#define REMMINA_FILE_EDITOR( obj ) \
    ( G_TYPE_CHECK_INSTANCE_CAST( ( obj ), REMMINA_TYPE_FILE_EDITOR, RemminaFileEditor ) )
#define REMMINA_FILE_EDITOR_CLASS( klass ) \
    ( G_TYPE_CHECK_CLASS_CAST( ( klass ), REMMINA_TYPE_FILE_EDITOR, RemminaFileEditorClass ) )
#define REMMINA_IS_FILE_EDITOR( obj ) ( G_TYPE_CHECK_INSTANCE_TYPE( ( obj ), REMMINA_TYPE_FILE_EDITOR ) )
#define REMMINA_IS_FILE_EDITOR_CLASS( klass ) ( G_TYPE_CHECK_CLASS_TYPE( ( klass ), REMMINA_TYPE_FILE_EDITOR ) )
#define REMMINA_FILE_EDITOR_GET_CLASS( obj ) \
    ( G_TYPE_INSTANCE_GET_CLASS( ( obj ), REMMINA_TYPE_FILE_EDITOR, RemminaFileEditorClass ) )

struct RemminaFileEditorPriv;

struct RemminaFileEditor
{
    GtkDialog dialog;

    RemminaFileEditorPriv *priv;
};

struct RemminaFileEditorClass
{
    GtkDialogClass parent_class;
};

GType remmina_file_editor_get_type() G_GNUC_CONST;

/* Base constructor */
GtkWidget *remmina_file_editor_new_from_file( RemminaFile *remminafile );
/* Create new file */
GtkWidget *remmina_file_editor_new();
GtkWidget *remmina_file_editor_new_full( const char *server, const char *protocol );
GtkWidget *remmina_file_editor_new_copy( const char *filename );
/* Open existing file */
GtkWidget *remmina_file_editor_new_from_filename( const char *filename );
void remmina_file_editor_check_profile( RemminaFileEditor *gfe );
void remmina_file_editor_file_save( RemminaFileEditor *gfe );
