/*
 * Remmina - The GTK+ Remote Desktop Client
 * Copyright (C) 2010-2011 Vic Lee
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

#include <stdarg.h>
#include <remmina/types.hpp>
#include "remmina/remmina_trace_calls.hpp"

struct RemminaFile;

enum RemminaPluginType
{
    REMMINA_PLUGIN_TYPE_PROTOCOL = 0,
    REMMINA_PLUGIN_TYPE_ENTRY = 1,
    REMMINA_PLUGIN_TYPE_FILE = 2,
    REMMINA_PLUGIN_TYPE_TOOL = 3,
    REMMINA_PLUGIN_TYPE_PREF = 4,
    REMMINA_PLUGIN_TYPE_SECRET = 5
};

struct RemminaPlugin
{
    RemminaPluginType type;
    const char *name;
    const char *description;
    const char *domain;
    const char *version;
};

struct RemminaProtocolPlugin
{
    RemminaPluginType type;
    const char *name;
    const char *description;
    const char *domain;
    const char *version;

    const char *icon_name;
    const char *icon_name_ssh;
    const RemminaProtocolSetting *basic_settings;
    const RemminaProtocolSetting *advanced_settings;
    RemminaProtocolSSHSetting ssh_setting;
    const RemminaProtocolFeature *features;

    void ( *init )( RemminaProtocolWidget *gp );
    int ( *open_connection )( RemminaProtocolWidget *gp );
    int ( *close_connection )( RemminaProtocolWidget *gp );
    int ( *query_feature )( RemminaProtocolWidget *gp, const RemminaProtocolFeature *feature );
    void ( *call_feature )( RemminaProtocolWidget *gp, const RemminaProtocolFeature *feature );
    void ( *send_keystrokes )( RemminaProtocolWidget *gp, const guint keystrokes[], const gint keylen );
    int ( *get_plugin_screenshot )( RemminaProtocolWidget *gp, RemminaPluginScreenshotData *rpsd );
    int ( *map_event )( RemminaProtocolWidget *gp );
    int ( *unmap_event )( RemminaProtocolWidget *gp );
};

struct RemminaEntryPlugin
{
    RemminaPluginType type;
    const char *name;
    const char *description;
    const char *domain;
    const char *version;

    void ( *entry_func )();
};

struct RemminaFilePlugin
{
    RemminaPluginType type;
    const char *name;
    const char *description;
    const char *domain;
    const char *version;

    int ( *import_test_func )( const char *from_file );
    RemminaFile *( *import_func )( const char *from_file );
    int ( *export_test_func )( RemminaFile *file );
    int ( *export_func )( RemminaFile *file, const char *to_file );
    const char *export_hints;
};

struct RemminaToolPlugin
{
    RemminaPluginType type;
    const char *name;
    const char *description;
    const char *domain;
    const char *version;

    void ( *exec_func )();
};

struct RemminaPrefPlugin
{
    RemminaPluginType type;
    const char *name;
    const char *description;
    const char *domain;
    const char *version;

    const char *pref_label;
    GtkWidget *( *get_pref_body )();
};

struct RemminaSecretPlugin
{
    RemminaPluginType type;
    const char *name;
    const char *description;
    const char *domain;
    const char *version;
    int init_order;

    int ( *init )();
    int ( *is_service_available )();
    void ( *store_password )( RemminaFile *remminafile, const char *key, const char *password );
    char *( *get_password )( RemminaFile *remminafile, const char *key );
    void ( *delete_password )( RemminaFile *remminafile, const char *key );
};

/* Plugin Service is a struct containing a list of function pointers,
 * which is passed from Remmina main program to the plugin module
 * through the plugin entry function remmina_plugin_entry() */
struct RemminaPluginService
{
    int ( *register_plugin )( RemminaPlugin *plugin );

    gint ( *protocol_plugin_get_width )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_set_width )( RemminaProtocolWidget *gp, gint width );
    gint ( *protocol_plugin_get_height )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_set_height )( RemminaProtocolWidget *gp, gint height );
    RemminaScaleMode ( *remmina_protocol_widget_get_current_scale_mode )( RemminaProtocolWidget *gp );
    int ( *protocol_plugin_get_expand )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_set_expand )( RemminaProtocolWidget *gp, bool expand );
    int ( *protocol_plugin_has_error )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_set_error )( RemminaProtocolWidget *gp, const char *fmt, ... );
    int ( *protocol_plugin_is_closed )( RemminaProtocolWidget *gp );
    RemminaFile *( *protocol_plugin_get_file )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_emit_signal )( RemminaProtocolWidget *gp, const char *signal_name );
    void ( *protocol_plugin_register_hostkey )( RemminaProtocolWidget *gp, GtkWidget *widget );
    char *( *protocol_plugin_start_direct_tunnel )( RemminaProtocolWidget *gp, gint default_port, bool port_plus );
    int ( *protocol_plugin_start_reverse_tunnel )( RemminaProtocolWidget *gp, gint local_port );
    int ( *protocol_plugin_start_xport_tunnel )( RemminaProtocolWidget *gp, RemminaXPortTunnelInitFunc init_func );
    void ( *protocol_plugin_set_display )( RemminaProtocolWidget *gp, gint display );
    void ( *protocol_plugin_signal_connection_closed )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_signal_connection_opened )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_update_align )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_lock_dynres )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_unlock_dynres )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_desktop_resize )( RemminaProtocolWidget *gp );
    gint ( *protocol_plugin_init_auth )( RemminaProtocolWidget *gp,
                                         RemminaMessagePanelFlags pflags,
                                         const char *title,
                                         const char *default_username,
                                         const char *default_password,
                                         const char *default_domain,
                                         const char *password_prompt );
    gint ( *protocol_plugin_init_certificate )( RemminaProtocolWidget *gp,
                                                const char *subject,
                                                const char *issuer,
                                                const char *fingerprint );
    gint ( *protocol_plugin_changed_certificate )( RemminaProtocolWidget *gp,
                                                   const char *subject,
                                                   const char *issuer,
                                                   const char *new_fingerprint,
                                                   const char *old_fingerprint );
    char *( *protocol_plugin_init_get_username )( RemminaProtocolWidget *gp );
    char *( *protocol_plugin_init_get_password )( RemminaProtocolWidget *gp );
    char *( *protocol_plugin_init_get_domain )( RemminaProtocolWidget *gp );
    int ( *protocol_plugin_init_get_savepassword )( RemminaProtocolWidget *gp );
    gint ( *protocol_plugin_init_authx509 )( RemminaProtocolWidget *gp );
    char *( *protocol_plugin_init_get_cacert )( RemminaProtocolWidget *gp );
    char *( *protocol_plugin_init_get_cacrl )( RemminaProtocolWidget *gp );
    char *( *protocol_plugin_init_get_clientcert )( RemminaProtocolWidget *gp );
    char *( *protocol_plugin_init_get_clientkey )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_init_save_cred )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_init_show_listen )( RemminaProtocolWidget *gp, gint port );
    void ( *protocol_plugin_init_show_retry )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_init_show )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_init_hide )( RemminaProtocolWidget *gp );
    int ( *protocol_plugin_ssh_exec )( RemminaProtocolWidget *gp, bool wait, const char *fmt, ... );
    void ( *protocol_plugin_chat_open )( RemminaProtocolWidget *gp,
                                         const char *name,
                                         void ( *on_send )( RemminaProtocolWidget *gp, const char *text ),
                                         void ( *on_destroy )( RemminaProtocolWidget *gp ) );
    void ( *protocol_plugin_chat_close )( RemminaProtocolWidget *gp );
    void ( *protocol_plugin_chat_receive )( RemminaProtocolWidget *gp, const char *text );
    void ( *protocol_plugin_send_keys_signals )( GtkWidget *widget,
                                                 const guint *keyvals,
                                                 int length,
                                                 GdkEventType action );

    char *( *file_get_user_datadir )();

    RemminaFile *( *file_new )();
    const char *( *file_get_path )( RemminaFile *remminafile );
    void ( *file_set_string )( RemminaFile *remminafile, const char *setting, const char *value );
    const char *( *file_get_string )( RemminaFile *remminafile, const char *setting );
    char *( *file_get_secret )( RemminaFile *remminafile, const char *setting );
    void ( *file_set_int )( RemminaFile *remminafile, const char *setting, gint value );
    gint ( *file_get_int )( RemminaFile *remminafile, const char *setting, gint default_value );
    void ( *file_unsave_passwords )( RemminaFile *remminafile );

    void ( *pref_set_value )( const char *key, const char *value );
    char *( *pref_get_value )( const char *key );
    gint ( *pref_get_scale_quality )();
    gint ( *pref_get_sshtunnel_port )();
    gint ( *pref_get_ssh_loglevel )();
    int ( *pref_get_ssh_parseconfig )();
    guint *( *pref_keymap_get_table )( const char *keymap );
    guint ( *pref_keymap_get_keyval )( const char *keymap, guint keyval );

    void ( *_remmina_info )( const char *fmt, ... );
    void ( *_remmina_message )( const char *fmt, ... );
    void ( *_remmina_debug )( const char *func, const char *fmt, ... );
    void ( *_remmina_warning )( const char *func, const char *fmt, ... );
    void ( *_remmina_error )( const char *func, const char *fmt, ... );
    void ( *_remmina_critical )( const char *func, const char *fmt, ... );
    void ( *log_print )( const char *text );
    void ( *log_printf )( const char *fmt, ... );

    void ( *ui_register )( GtkWidget *widget );

    GtkWidget *( *open_connection )( RemminaFile *remminafile, GCallback disconnect_cb, gpointer data, guint *handler );
    gint ( *open_unix_sock )( const char *unixsock );
    void ( *get_server_port )( const char *server, gint defaultport, char **host, gint *port );
    int ( *is_main_thread )();
    int ( *gtksocket_available )();
    gint ( *get_profile_remote_width )( RemminaProtocolWidget *gp );
    gint ( *get_profile_remote_height )( RemminaProtocolWidget *gp );
};

/* "Prototype" of the plugin entry function */
typedef int ( *RemminaPluginEntryFunc )( RemminaPluginService *service );
