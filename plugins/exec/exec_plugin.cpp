/*
 * Remmina - The GTK+ Remote Desktop Client
 * Copyright (C) 2017-2022 Antenore Gatta, Giovanni Panozzo
 *
 * Initially based on the plugin "Remmina Plugin EXEC", created and written by
 * Fabio Castelli (Muflone) <muflone@vbsimple.net>.
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

#include <gtkmm.h>

#include "exec_plugin_config.hpp"

#include "common/remmina_plugin.hpp"

#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define GET_PLUGIN_DATA( gp ) (RemminaPluginExecData *)g_object_get_data( G_OBJECT( gp ), "plugin-data" )

struct RemminaPluginExecData
{
    Gtk::TextView *log_view;
    Glib::RefPtr<Gtk::TextBuffer> log_buffer;
    Glib::RefPtr<Gtk::TextBuffer> err;
    Gtk::ScrolledWindow *sw;
};

static RemminaPluginService *remmina_plugin_service = NULL;
#define REMMINA_PLUGIN_DEBUG( fmt, ... ) remmina_plugin_service->_remmina_debug( __func__, fmt, ##__VA_ARGS__ )

static void cb_child_watch( GPid pid, gint status )
{
    /* Close pid */
    g_spawn_close_pid( pid );
}

static int cb_out_watch( GIOChannel *channel, GIOCondition cond, RemminaProtocolWidget *gp )
{
    char *string;
    gsize size;

    RemminaPluginExecData *gpdata = GET_PLUGIN_DATA( gp );

    if( cond == G_IO_HUP )
    {
        g_io_channel_unref( channel );
        return FALSE;
    }

    g_io_channel_read_line( channel, &string, &size, NULL, NULL );
    gpdata->log_buffer->insert_at_cursor( Glib::ustring( string, size ) );
    //gtk_text_buffer_insert_at_cursor( gpdata->log_buffer, string, -1 );
    g_free( string );

    return TRUE;
}

static int cb_err_watch( GIOChannel *channel, GIOCondition cond, RemminaProtocolWidget *gp )
{
    char *string;
    gsize size;

    RemminaPluginExecData *gpdata = GET_PLUGIN_DATA( gp );

    if( cond == G_IO_HUP )
    {
        g_io_channel_unref( channel );
        return FALSE;
    }

    g_io_channel_read_line( channel, &string, &size, NULL, NULL );
    gpdata->err->insert_at_cursor( Glib::ustring( string, size ) );
    // gtk_text_buffer_insert_at_cursor( gpdata->err, string, -1 );
    g_free( string );

    return TRUE;
}

static void remmina_plugin_exec_init( RemminaProtocolWidget *gp )
{
    TRACE_CALL( __func__ );
    RemminaPluginExecData *gpdata;

    REMMINA_PLUGIN_DEBUG( "[%s] Plugin init", PLUGIN_NAME );

    gpdata = g_new0( RemminaPluginExecData, 1 );
    g_object_set_data_full( G_OBJECT( gp ), "plugin-data", gpdata, g_free );

    gpdata->log_view = new Gtk::TextView();
    gpdata->log_view->set_wrap_mode( Gtk::WrapMode::WRAP_CHAR );
    gpdata->log_view->set_editable( false );
    gpdata->log_view->set_left_margin( 20 );
    gpdata->log_view->set_right_margin( 20 );

    gpdata->log_buffer = gpdata->log_view->get_buffer();

    gpdata->sw = new Gtk::ScrolledWindow();
    gpdata->sw->set_size_request( 640, 480 );
    gpdata->sw->set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );

    Gtk::Container *container = Glib::wrap( GTK_CONTAINER( gp ) );
    container->add( *gpdata->sw );
    container->add( *gpdata->log_view );

    gpdata->log_buffer->set_text( "Remmina Exec Plugin Logger" );

    gpdata->sw->show_all();
}

static int remmina_plugin_exec_run( RemminaProtocolWidget *gp )
{
    TRACE_CALL( __func__ );
    RemminaFile *remminafile;
    const char *cmd;
    char *stdout_buffer;
    char *stderr_buffer;
    char **argv;
    GError *error = NULL;
    GPid child_pid;
    gint child_stdout, child_stderr;
    GtkDialog *dialog;
    GIOChannel *out_ch, *err_ch;

    REMMINA_PLUGIN_DEBUG( "[%s] Plugin run", PLUGIN_NAME );
    RemminaPluginExecData *gpdata = GET_PLUGIN_DATA( gp );
    remminafile = remmina_plugin_service->protocol_plugin_get_file( gp );

    cmd = remmina_plugin_service->file_get_string( remminafile, "execcommand" );
    if( !cmd )
    {
        gpdata->log_buffer->set_text( _( "You did not set any command to be executed" ) );
        remmina_plugin_service->protocol_plugin_signal_connection_opened( gp );
        return TRUE;
    }

    g_shell_parse_argv( cmd, NULL, &argv, &error );
    if( error )
    {
        gpdata->log_buffer->set_text( error->message );
        remmina_plugin_service->protocol_plugin_signal_connection_opened( gp );
        g_error_free( error );
        return TRUE;
    }

    if( remmina_plugin_service->file_get_int( remminafile, "runasync", FALSE ) )
    {
        REMMINA_PLUGIN_DEBUG( "[%s] Run Async", PLUGIN_NAME );
        g_spawn_async_with_pipes(
            NULL,
            argv,
            NULL,
            static_cast<GSpawnFlags>( G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH_FROM_ENVP | G_SPAWN_SEARCH_PATH ),
            NULL,
            NULL,
            &child_pid,
            NULL,
            &child_stdout,
            &child_stderr,
            &error );
        if( error != NULL )
        {
            gpdata->log_buffer->set_text( error->message );
            g_error_free( error );
            remmina_plugin_service->protocol_plugin_signal_connection_opened( gp );
            return TRUE;
        }
        g_child_watch_add( child_pid, (GChildWatchFunc)cb_child_watch, gp );

        /* Create channels that will be used to read data from pipes. */
        out_ch = g_io_channel_unix_new( child_stdout );
        err_ch = g_io_channel_unix_new( child_stderr );
        /* Add watches to channels */
        g_io_add_watch( out_ch, static_cast<GIOCondition>( G_IO_IN | G_IO_HUP ), (GIOFunc)cb_out_watch, gp );
        g_io_add_watch( err_ch, static_cast<GIOCondition>( G_IO_IN | G_IO_HUP ), (GIOFunc)cb_err_watch, gp );
    }
    else
    {
        dialog = GTK_DIALOG( gtk_message_dialog_new( NULL,
                                                     GTK_DIALOG_MODAL,
                                                     GTK_MESSAGE_QUESTION,
                                                     GTK_BUTTONS_YES_NO,
                                                     _( "Warning: Running a command synchronously may cause Remmina "
                                                        "not to respond.\nDo you really want to continue?" ) ) );
        gint result = gtk_dialog_run( GTK_DIALOG( dialog ) );

        switch( result )
        {
            case GTK_RESPONSE_YES:
                break;
            default:
                gtk_widget_destroy( GTK_WIDGET( dialog ) );
                return FALSE;
                break;
        }
        gtk_widget_destroy( GTK_WIDGET( dialog ) );
        REMMINA_PLUGIN_DEBUG( "[%s] Run Sync", PLUGIN_NAME );
        g_spawn_sync( NULL, // CWD or NULL
                      argv,
                      NULL, // ENVP or NULL
                      static_cast<GSpawnFlags>( G_SPAWN_SEARCH_PATH | G_SPAWN_SEARCH_PATH_FROM_ENVP ),
                      NULL,
                      NULL,
                      &stdout_buffer, // STDOUT
                      &stderr_buffer, // STDERR
                      NULL,           // Exit status
                      &error );
        if( !error )
        {
            REMMINA_PLUGIN_DEBUG( "[%s] Command executed", PLUGIN_NAME );

            gpdata->log_buffer->set_text( stdout_buffer );
        }
        else
        {
            g_warning( "Command %s exited with error: %s\n", cmd, error->message );
            gpdata->log_buffer->set_text( error->message );
            g_error_free( error );
        }
    }

    g_strfreev( argv );

    remmina_plugin_service->protocol_plugin_signal_connection_opened( gp );
    return TRUE;
}

static int remmina_plugin_exec_close( RemminaProtocolWidget *gp )
{
    TRACE_CALL( __func__ );
    REMMINA_PLUGIN_DEBUG( "[%s] Plugin close", PLUGIN_NAME );
    remmina_plugin_service->protocol_plugin_signal_connection_closed( gp );
    return FALSE;
}

/* Array of RemminaProtocolSetting for basic settings.
 * Each item is composed by:
 * a) RemminaProtocolSettingType for setting type
 * b) Setting name
 * c) Setting description
 * d) Compact disposition
 * e) Values for REMMINA_PROTOCOL_SETTING_TYPE_SELECT or REMMINA_PROTOCOL_SETTING_TYPE_COMBO
 * f) Setting tooltip
 * g) Validation data pointer, will be passed to the validation callback method.
 * h) Validation callback method (Can be NULL. Every entry will be valid then.)
 *		use following prototype:
 *		bool mysetting_validator_method(gpointer key, gpointer value,
 *						    gpointer validator_data);
 *		gpointer key is a char* containing the setting's name,
 *		gpointer value contains the value which should be validated,
 *		gpointer validator_data contains your passed data.
 */
static const RemminaProtocolSetting remmina_plugin_exec_basic_settings[] = {
    { REMMINA_PROTOCOL_SETTING_TYPE_TEXT, "execcommand", N_( "Command" ), FALSE, NULL, NULL, NULL, NULL },
    { REMMINA_PROTOCOL_SETTING_TYPE_CHECK, "runasync", N_( "Asynchronous execution" ), FALSE, NULL, NULL, NULL, NULL },
    { REMMINA_PROTOCOL_SETTING_TYPE_END, NULL, NULL, FALSE, NULL, NULL, NULL, NULL } };

/* Protocol plugin definition and features */
static RemminaProtocolPlugin remmina_plugin = {
    REMMINA_PLUGIN_TYPE_PROTOCOL,       // Type
    PLUGIN_NAME,                        // Name
    PLUGIN_DESCRIPTION,                 // Description
    GETTEXT_PACKAGE,                    // Translation domain
    PLUGIN_VERSION,                     // Version number
    PLUGIN_APPICON,                     // Icon for normal connection
    PLUGIN_APPICON,                     // Icon for SSH connection
    remmina_plugin_exec_basic_settings, // Array for basic settings
    NULL,                               // Array for advanced settings
    REMMINA_PROTOCOL_SSH_SETTING_NONE,  // SSH settings type
    NULL,                               // Array for available features
    remmina_plugin_exec_init,           // Plugin initialization
    remmina_plugin_exec_run,            // Plugin open connection
    remmina_plugin_exec_close,          // Plugin close connection
    NULL,                               // Query for available features
    NULL,                               // Call a feature
    NULL,                               // Send a keystroke
    NULL,                               // No screenshot support available
    NULL,                               // RCW map event
    NULL                                // RCW unmap event
};

extern "C"
{
    G_MODULE_EXPORT int remmina_plugin_entry( RemminaPluginService *service )
    {
        TRACE_CALL( __func__ );
        remmina_plugin_service = service;

        bindtextdomain( GETTEXT_PACKAGE, REMMINA_RUNTIME_LOCALEDIR );
        bind_textdomain_codeset( GETTEXT_PACKAGE, "UTF-8" );

        if( !service->register_plugin( (RemminaPlugin *)&remmina_plugin ) )
        {
            return FALSE;
        }

        return TRUE;
    }
}
