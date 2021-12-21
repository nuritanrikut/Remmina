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

#include "rcw.hpp"
#include "remmina_file.hpp"
#include "remmina_ssh.hpp"



#define REMMINA_PROTOCOL_FEATURE_TOOL_SSH -1
#define REMMINA_PROTOCOL_FEATURE_TOOL_SFTP -2

#define REMMINA_TYPE_PROTOCOL_WIDGET ( remmina_protocol_widget_get_type() )
#define REMMINA_PROTOCOL_WIDGET( obj ) \
    ( G_TYPE_CHECK_INSTANCE_CAST( ( obj ), REMMINA_TYPE_PROTOCOL_WIDGET, RemminaProtocolWidget ) )
#define REMMINA_PROTOCOL_WIDGET_CLASS( klass ) \
    ( G_TYPE_CHECK_CLASS_CAST( ( klass ), REMMINA_TYPE_PROTOCOL_WIDGET, RemminaProtocolWidgetClass ) )
#define REMMINA_IS_PROTOCOL_WIDGET( obj ) ( G_TYPE_CHECK_INSTANCE_TYPE( ( obj ), REMMINA_TYPE_PROTOCOL_WIDGET ) )
#define REMMINA_IS_PROTOCOL_WIDGET_CLASS( klass ) ( G_TYPE_CHECK_CLASS_TYPE( ( klass ), REMMINA_TYPE_PROTOCOL_WIDGET ) )
#define REMMINA_PROTOCOL_WIDGET_GET_CLASS( obj ) \
    ( G_TYPE_INSTANCE_GET_CLASS( ( obj ), REMMINA_TYPE_PROTOCOL_WIDGET, RemminaProtocolWidgetClass ) )

typedef struct _RemminaProtocolWidgetPriv RemminaProtocolWidgetPriv;
typedef struct _RemminaProtocolPlugin RemminaProtocolPlugin;

struct _RemminaProtocolWidget
{
    GtkEventBox event_box;
    RemminaConnectionObject *cnnobj;
    RemminaProtocolWidgetPriv *priv;
    RemminaProtocolPlugin *plugin;
};

struct _RemminaProtocolWidgetClass
{
    GtkEventBoxClass parent_class;

    void ( *connect )( RemminaProtocolWidget *gp );
    void ( *disconnect )( RemminaProtocolWidget *gp );
    void ( *desktop_resize )( RemminaProtocolWidget *gp );
    void ( *update_align )( RemminaProtocolWidget *gp );
    void ( *lock_dynres )( RemminaProtocolWidget *gp );
    void ( *unlock_dynres )( RemminaProtocolWidget *gp );
};

GType remmina_protocol_widget_get_type( void ) G_GNUC_CONST;

GtkWindow *remmina_protocol_widget_get_gtkwindow( RemminaProtocolWidget *gp );
GtkWidget *remmina_protocol_widget_gtkviewport( RemminaProtocolWidget *gp );

GtkWidget *remmina_protocol_widget_new( void );
void remmina_protocol_widget_setup( RemminaProtocolWidget *gp,
                                    RemminaFile *remminafile,
                                    RemminaConnectionObject *cnnobj );

gint remmina_protocol_widget_get_width( RemminaProtocolWidget *gp );
void remmina_protocol_widget_set_width( RemminaProtocolWidget *gp, gint width );
gint remmina_protocol_widget_get_height( RemminaProtocolWidget *gp );
void remmina_protocol_widget_set_height( RemminaProtocolWidget *gp, gint height );
gint remmina_protocol_widget_get_profile_remote_width( RemminaProtocolWidget *gp );
gint remmina_protocol_widget_get_profile_remote_height( RemminaProtocolWidget *gp );
gint remmina_protocol_widget_get_multimon( RemminaProtocolWidget *gp );

RemminaScaleMode remmina_protocol_widget_get_current_scale_mode( RemminaProtocolWidget *gp );
void remmina_protocol_widget_set_current_scale_mode( RemminaProtocolWidget *gp, RemminaScaleMode scalemode );
int remmina_protocol_widget_get_expand( RemminaProtocolWidget *gp );
void remmina_protocol_widget_set_expand( RemminaProtocolWidget *gp, bool expand );
int remmina_protocol_widget_has_error( RemminaProtocolWidget *gp );
const char *remmina_protocol_widget_get_error_message( RemminaProtocolWidget *gp );
void remmina_protocol_widget_set_error( RemminaProtocolWidget *gp, const char *fmt, ... );
int remmina_protocol_widget_is_closed( RemminaProtocolWidget *gp );
RemminaFile *remmina_protocol_widget_get_file( RemminaProtocolWidget *gp );

void remmina_protocol_widget_open_connection( RemminaProtocolWidget *gp );
void remmina_protocol_widget_close_connection( RemminaProtocolWidget *gp );
void remmina_protocol_widget_signal_connection_closed( RemminaProtocolWidget *gp );
void remmina_protocol_widget_signal_connection_opened( RemminaProtocolWidget *gp );
void remmina_protocol_widget_update_align( RemminaProtocolWidget *gp );
void remmina_protocol_widget_lock_dynres( RemminaProtocolWidget *gp );
void remmina_protocol_widget_unlock_dynres( RemminaProtocolWidget *gp );
void remmina_protocol_widget_desktop_resize( RemminaProtocolWidget *gp );
void remmina_protocol_widget_grab_focus( RemminaProtocolWidget *gp );
const RemminaProtocolFeature *remmina_protocol_widget_get_features( RemminaProtocolWidget *gp );
int remmina_protocol_widget_query_feature_by_type( RemminaProtocolWidget *gp, RemminaProtocolFeatureType type );
int remmina_protocol_widget_query_feature_by_ref( RemminaProtocolWidget *gp,
                                                       const RemminaProtocolFeature *feature );
void remmina_protocol_widget_call_feature_by_type( RemminaProtocolWidget *gp,
                                                   RemminaProtocolFeatureType type,
                                                   gint id );
void remmina_protocol_widget_call_feature_by_ref( RemminaProtocolWidget *gp, const RemminaProtocolFeature *feature );
/* Provide thread-safe way to emit signals */
void remmina_protocol_widget_emit_signal( RemminaProtocolWidget *gp, const char *signal_name );
void remmina_protocol_widget_register_hostkey( RemminaProtocolWidget *gp, GtkWidget *widget );

typedef int ( *RemminaHostkeyFunc )( RemminaProtocolWidget *gp, guint keyval, bool release );
void remmina_protocol_widget_set_hostkey_func( RemminaProtocolWidget *gp, RemminaHostkeyFunc func );

int remmina_protocol_widget_ssh_exec( RemminaProtocolWidget *gp, bool wait, const char *fmt, ... );

/* Start a SSH tunnel if it’s enabled. Returns a newly allocated string indicating:
 * 1. The actual destination (host:port) if SSH tunnel is disable
 * 2. The tunnel local destination (127.0.0.1:port) if SSH tunnel is enabled
 */
char *remmina_protocol_widget_start_direct_tunnel( RemminaProtocolWidget *gp, gint default_port, bool port_plus );

int remmina_protocol_widget_start_reverse_tunnel( RemminaProtocolWidget *gp, gint local_port );
int remmina_protocol_widget_start_xport_tunnel( RemminaProtocolWidget *gp, RemminaXPortTunnelInitFunc init_func );
void remmina_protocol_widget_set_display( RemminaProtocolWidget *gp, gint display );

/* Extension for remmina_protocol_widget_panel_authuserpwd() not currently exported to plugins */
gint remmina_protocol_widget_panel_authuserpwd_ssh_tunnel( RemminaProtocolWidget *gp,
                                                           bool want_domain,
                                                           bool allow_password_saving );

/* Dialog panel API used by the plugins */

gint remmina_protocol_widget_panel_auth( RemminaProtocolWidget *gp,
                                         RemminaMessagePanelFlags pflags,
                                         const char *title,
                                         const char *default_username,
                                         const char *default_password,
                                         const char *default_domain,
                                         const char *password_prompt );
gint remmina_protocol_widget_panel_new_certificate( RemminaProtocolWidget *gp,
                                                    const char *subject,
                                                    const char *issuer,
                                                    const char *fingerprint );
gint remmina_protocol_widget_panel_changed_certificate( RemminaProtocolWidget *gp,
                                                        const char *subject,
                                                        const char *issuer,
                                                        const char *new_fingerprint,
                                                        const char *old_fingerprint );
gint remmina_protocol_widget_panel_question_yesno( RemminaProtocolWidget *gp, const char *msg );

void remmina_protocol_widget_panel_show( RemminaProtocolWidget *gp );
void remmina_protocol_widget_panel_hide( RemminaProtocolWidget *gp );
void remmina_protocol_widget_panel_destroy( RemminaProtocolWidget *gp );
gint remmina_protocol_widget_panel_authx509( RemminaProtocolWidget *gp );
void remmina_protocol_widget_panel_show_listen( RemminaProtocolWidget *gp, gint port );
void remmina_protocol_widget_panel_show_retry( RemminaProtocolWidget *gp );

void remmina_protocol_widget_save_cred( RemminaProtocolWidget *gp );

char *remmina_protocol_widget_get_username( RemminaProtocolWidget *gp );
char *remmina_protocol_widget_get_password( RemminaProtocolWidget *gp );
char *remmina_protocol_widget_get_domain( RemminaProtocolWidget *gp );
int remmina_protocol_widget_get_savepassword( RemminaProtocolWidget *gp );
char *remmina_protocol_widget_get_cacert( RemminaProtocolWidget *gp );
char *remmina_protocol_widget_get_cacrl( RemminaProtocolWidget *gp );
char *remmina_protocol_widget_get_clientcert( RemminaProtocolWidget *gp );
char *remmina_protocol_widget_get_clientkey( RemminaProtocolWidget *gp );

void remmina_protocol_widget_chat_open( RemminaProtocolWidget *gp,
                                        const char *name,
                                        void ( *on_send )( RemminaProtocolWidget *gp, const char *text ),
                                        void ( *on_destroy )( RemminaProtocolWidget *gp ) );
void remmina_protocol_widget_chat_close( RemminaProtocolWidget *gp );
void remmina_protocol_widget_chat_receive( RemminaProtocolWidget *gp, const char *text );
void remmina_protocol_widget_send_keys_signals( GtkWidget *widget,
                                                const guint *keyvals,
                                                int keyvals_length,
                                                GdkEventType action );
/* Check if the plugin accepts keystrokes */
int remmina_protocol_widget_plugin_receives_keystrokes( RemminaProtocolWidget *gp );
/* Send to the plugin some keystrokes */
void remmina_protocol_widget_send_keystrokes( RemminaProtocolWidget *gp, GtkMenuItem *widget );
void remmina_protocol_widget_send_clipboard( RemminaProtocolWidget *gp, GtkMenuItem *widget );
/* Take screenshot of plugin */
int remmina_protocol_widget_plugin_screenshot( RemminaProtocolWidget *gp, RemminaPluginScreenshotData *rpsd );
/* Deal with the remimna connection window map/unmap events */
int remmina_protocol_widget_map_event( RemminaProtocolWidget *gp );
int remmina_protocol_widget_unmap_event( RemminaProtocolWidget *gp );

void remmina_protocol_widget_update_remote_resolution( RemminaProtocolWidget *gp );

/* Functions to support execution of GTK code on master thread */
RemminaMessagePanel *remmina_protocol_widget_mpprogress( RemminaConnectionObject *cnnobj,
                                                         const char *msg,
                                                         RemminaMessagePanelCallback response_callback,
                                                         gpointer response_callback_data );
void remmina_protocol_widget_mpdestroy( RemminaConnectionObject *cnnobj, RemminaMessagePanel *mp );


