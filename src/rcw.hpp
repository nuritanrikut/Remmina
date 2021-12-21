/*
 * Remmina - The GTK+ Remote Desktop Client
 * Copyright (C) 2009 - Vic Lee
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

#include "remmina_file.hpp"
#include "remmina_message_panel.hpp"

#define REMMINA_TYPE_CONNECTION_WINDOW ( rcw_get_type() )
#define RCW( obj ) ( G_TYPE_CHECK_INSTANCE_CAST( ( obj ), REMMINA_TYPE_CONNECTION_WINDOW, RemminaConnectionWindow ) )
#define RCW_CLASS( klass ) \
    ( G_TYPE_CHECK_CLASS_CAST( ( klass ), REMMINA_TYPE_CONNECTION_WINDOW, RemminaConnectionWindowClass ) )
#define REMMINA_IS_CONNECTION_WINDOW( obj ) ( G_TYPE_CHECK_INSTANCE_TYPE( ( obj ), REMMINA_TYPE_CONNECTION_WINDOW ) )
#define REMMINA_IS_CONNECTION_WINDOW_CLASS( klass ) \
    ( G_TYPE_CHECK_CLASS_TYPE( ( klass ), REMMINA_TYPE_CONNECTION_WINDOW ) )
#define RCW_GET_CLASS( obj ) \
    ( G_TYPE_INSTANCE_GET_CLASS( ( obj ), REMMINA_TYPE_CONNECTION_WINDOW, RemminaConnectionWindowClass ) )

struct RemminaConnectionWindowPriv;
struct RemminaConnectionObject;

struct RemminaConnectionWindow
{
    GtkWindow window;
    RemminaConnectionWindowPriv *priv;
};

struct RemminaConnectionWindowClass
{
    GtkWindowClass parent_class;
    void ( *toolbar_place )( RemminaConnectionWindow *gp );
};

enum RemminaConnectionWindowOnDeleteConfirmMode
{
    RCW_ONDELETE_CONFIRM_IF_2_OR_MORE = 0,
    RCW_ONDELETE_NOCONFIRM = 1
};

GType rcw_get_type() G_GNUC_CONST;

/* Open a new connection window for a .remmina file */
int rcw_open_from_filename( const char *filename );
/* Open a new connection window for a given RemminaFile struct. The struct will be freed after the call */
void rcw_open_from_file( RemminaFile *remminafile );
int rcw_delete( RemminaConnectionWindow *cnnwin );
void rcw_set_delete_confirm_mode( RemminaConnectionWindow *cnnwin, RemminaConnectionWindowOnDeleteConfirmMode mode );
GtkWidget *rcw_open_from_file_full( RemminaFile *remminafile, GCallback disconnect_cb, gpointer data, guint *handler );
GtkWindow *rcw_get_gtkwindow( RemminaConnectionObject *cnnobj );
GtkWidget *rcw_get_gtkviewport( RemminaConnectionObject *cnnobj );

void rco_destroy_message_panel( RemminaConnectionObject *cnnobj, RemminaMessagePanel *mp );
void rco_show_message_panel( RemminaConnectionObject *cnnobj, RemminaMessagePanel *mp );
void rco_get_monitor_geometry( RemminaConnectionObject *cnnobj, GdkRectangle *sz );

#define MESSAGE_PANEL_SPINNER 0x00000001
#define MESSAGE_PANEL_OKBUTTON 0x00000002
