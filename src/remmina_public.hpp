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

#include "config.h"
#include <gtk/gtk.h>

#define IDLE_ADD gdk_threads_add_idle
#define TIMEOUT_ADD gdk_threads_add_timeout
#define CANCEL_ASYNC \
    pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL ); \
    pthread_testcancel();
#define CANCEL_DEFER pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, NULL );

#define THREADS_ENTER _Pragma( "GCC error \"THREADS_ENTER has been deprecated in Remmina 1.2\"" )
#define THREADS_LEAVE _Pragma( "GCC error \"THREADS_LEAVE has been deprecated in Remmina 1.2\"" )

#define MAX_PATH_LEN 255

#define MAX_X_DISPLAY_NUMBER 99
#define X_UNIX_SOCKET "/tmp/.X11-unix/X%d"

#define CHAR_DELIMITOR ','
#define STRING_DELIMITOR ","
#define STRING_DELIMITOR2 "§"

#define MOUSE_BUTTON_LEFT 1
#define MOUSE_BUTTON_MIDDLE 2
#define MOUSE_BUTTON_RIGHT 3

/* Bind a template widget to its class member and callback */
#define BIND_TEMPLATE_CHILD( wc, type, action, callback ) \
    gtk_widget_class_bind_template_child( wc, type, action ); \
    gtk_widget_class_bind_template_callback( wc, callback );



/* items is separated by STRING_DELIMTOR */
GtkWidget *remmina_public_create_combo_entry( const char *text, const char *def, bool descending );
GtkWidget *remmina_public_create_combo_text_d( const char *text, const char *def, const char *empty_choice );
void remmina_public_load_combo_text_d( GtkWidget *combo,
                                       const char *text,
                                       const char *def,
                                       const char *empty_choice );
GtkWidget *remmina_public_create_combo( bool use_icon );
GtkWidget *remmina_public_create_combo_map( const gpointer *key_value_list,
                                            const char *def,
                                            bool use_icon,
                                            const char *domain );
GtkWidget *
remmina_public_create_combo_mapint( const gpointer *key_value_list, gint def, bool use_icon, const char *domain );

void remmina_public_create_group( GtkGrid *grid, const char *group, gint row, gint rows, gint cols );

char *remmina_public_combo_get_active_text( GtkComboBox *combo );

#if !GTK_CHECK_VERSION( 3, 22, 0 )
/* A function for gtk_menu_popup to get the position right below the widget specified by user_data */
void remmina_public_popup_position( GtkMenu *menu, gint *x, gint *y, bool *push_in, gpointer user_data );
#endif

/* Combine two paths into one by correctly handling trailing slash. Return newly allocated string */
char *remmina_public_combine_path( const char *path1, const char *path2 );

/**
 * Return a file descriptor handle for a unix socket
 * @return a file descriptor (socket) or -1
 *
 * */
gint remmina_public_open_unix_sock( const char *unixsock );

/* Parse a server entry with server name and port */
void remmina_public_get_server_port_old( const char *server, gint defaultport, char **host, gint *port );
void remmina_public_get_server_port( const char *server, gint defaultport, char **host, gint *port );

/* X */
int remmina_public_get_xauth_cookie( const char *display, char **msg );
gint remmina_public_open_xdisplay( const char *disp );

/* Find hardware keycode for the requested keyval */
guint16 remmina_public_get_keycode_for_keyval( GdkKeymap *keymap, guint keyval );
/* Check if the requested keycode is a key modifier */
int remmina_public_get_modifier_for_keycode( GdkKeymap *keymap, guint16 keycode );
/* Load a GtkBuilder object from a filename */
GtkBuilder *remmina_public_gtk_builder_new_from_file( char *filename );
/* Load a GtkBuilder object from a resource */
GtkBuilder *remmina_public_gtk_builder_new_from_resource( const char *resource );
/* Change parent container for a widget */
void remmina_public_gtk_widget_reparent( GtkWidget *widget, GtkContainer *container );
/* Used to send desktop notifications */
void remmina_public_send_notification( const char *notification_id,
                                       const char *notification_title,
                                       const char *notification_message );
/* Validate the inserted value for a new resolution */
int remmina_public_resolution_validation_func( const char *new_str, char **error );
/* Replaces all occurrences of search in a new copy of string by replacement. */
char *remmina_public_str_replace( const char *string, const char *search, const char *replacement );
/* Replaces all occurrences of search in a new copy of string by replacement
 * and overwrites the original string */
char *remmina_public_str_replace_in_place( char *string, const char *search, const char *replacement );
int remmina_public_split_resolution_string( const char *resolution_string, int *w, int *h );
int remmina_gtk_check_version( guint major, guint minor, guint micro );


