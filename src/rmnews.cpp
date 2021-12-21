/*
 * Remmina - The GTK+ Remote Desktop Client
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

#include "config.h"
#include "remmina/remmina_trace_calls.hpp"

#include <fcntl.h>
#include <gio/gdesktopappinfo.h>
#include <gio/gio.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <libsoup/soup.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "remmina.hpp"
#include "remmina_main.hpp"
#include "remmina_log.hpp"
#include "remmina_pref.hpp"
#include "remmina_public.hpp"
#include "remmina_sodium.hpp"
#include "remmina_utils.hpp"
#include "remmina_scheduler.hpp"
#include "remmina_sysinfo.hpp"
#include "rmnews.hpp"

#define ARR_SIZE( arr ) ( sizeof( ( arr ) ) / sizeof( ( arr[0] ) ) )
/* Neas file buffer */
#define READ_BUFFER_LEN 1024
/* Timers */
#define RMNEWS_CHECK_1ST_MS 3000
#define RMNEWS_CHECK_INTERVAL_MS 12000
/* How many seconds before to get news */
#define RMNEWS_INTERVAL_SEC 604800
/* TODO: move in config.h */
#define REMMINA_URL "https://remmina.org/"
#define RMNEWS_OUTPUT "/var/tmp/latest_news.md"

static RemminaNewsDialog *rmnews_news_dialog;
#define GET_OBJ( object_name ) gtk_builder_get_object( rmnews_news_dialog->builder, object_name )

static SoupSession *session;
static const char *output_file_path = NULL;

static const char *supported_mime_types[] = { "x-scheme-handler/rdp",
                                              "x-scheme-handler/spice",
                                              "x-scheme-handler/vnc",
                                              "x-scheme-handler/remmina",
                                              "application/x-remmina",
                                              NULL };

gint eweekdays[7] = { 86400, 172800, 259200, 345600, 432000, 518400, 604800 };

void rmnews_news_switch_state_set_cb()
{
    TRACE_CALL( __func__ );
    if( rmnews_news_dialog->rmnews_news_switch && gtk_switch_get_active( rmnews_news_dialog->rmnews_news_switch ) )
    {
        remmina_pref.periodic_news_permitted = TRUE;
        remmina_pref_save();
    }
    else
    {
        remmina_pref.periodic_news_permitted = FALSE;
        remmina_pref_save();
    }
}

void rmnews_defaultcl_on_click()
{
    TRACE_CALL( __func__ );
    g_autoptr( GError ) error = NULL;
    GDesktopAppInfo *desktop_info;
    GAppInfo *info = NULL;
    g_autofree char *id = g_strconcat( REMMINA_APP_ID, ".desktop", NULL );
    int i;

    desktop_info = g_desktop_app_info_new( id );
    if( !desktop_info )
        return;

    info = G_APP_INFO( desktop_info );

    for( i = 0; supported_mime_types[i]; i++ )
    {
        if( !g_app_info_set_as_default_for_type( info, supported_mime_types[i], &error ) )
            REMMINA_DEBUG( "Failed to set '%s' as the default application for secondary content type '%s': %s",
                           g_app_info_get_name( info ),
                           supported_mime_types[i],
                           error->message );
        else
            REMMINA_DEBUG(
                "Set '%s' as the default application for '%s'", g_app_info_get_name( info ), supported_mime_types[i] );
    }
}

static char *rmnews_get_file_contents( char *path )
{
    gsize size;
    char *content;

    if( g_file_get_contents( path, &content, &size, NULL ) )
    {
        if( !g_utf8_validate( content, size, NULL ) )
        {
            REMMINA_DEBUG( "%s content is not UTF-8", path );
            g_free( content );
            content = NULL;
        }
    }
    //return g_markup_escape_text(content, strlen(content));
    return content;
}

static void rmnews_close_clicked( GtkButton *btn, gpointer user_data )
{
    TRACE_CALL( __func__ );
    if( rmnews_news_dialog->dialog )
        gtk_widget_destroy( GTK_WIDGET( rmnews_news_dialog->dialog ) );
    rmnews_news_dialog->dialog = NULL;
    g_free( rmnews_news_dialog );
    rmnews_news_dialog = NULL;
}

static int rmnews_dialog_deleted( GtkButton *btn, gpointer user_data )
{
    TRACE_CALL( __func__ );
    gtk_widget_destroy( GTK_WIDGET( rmnews_news_dialog->dialog ) );
    rmnews_news_dialog->dialog = NULL;
    g_free( rmnews_news_dialog );
    rmnews_news_dialog = NULL;

    return FALSE;
}

void rmnews_show_news( GtkWindow *parent )
{
    TRACE_CALL( __func__ );

    rmnews_news_dialog = g_new0( RemminaNewsDialog, 1 );
    rmnews_news_dialog->retval = 1;

    rmnews_news_dialog->builder =
        remmina_public_gtk_builder_new_from_resource( "/org/remmina/Remmina/src/../data/ui/remmina_news.glade" );
    rmnews_news_dialog->dialog =
        GTK_DIALOG( gtk_builder_get_object( rmnews_news_dialog->builder, "RemminaNewsDialog" ) );

    rmnews_news_dialog->rmnews_text_view = GTK_TEXT_VIEW( GET_OBJ( "rmnews_text_view" ) );
    rmnews_news_dialog->rmnews_label = GTK_LABEL( GET_OBJ( "rmnews_label" ) );
    rmnews_news_dialog->rmnews_defaultcl_label = GTK_LABEL( GET_OBJ( "rmnews_defaultcl_label" ) );
    rmnews_news_dialog->rmnews_defaultcl_button = GTK_BUTTON( GET_OBJ( "rmnews_defaultcl_switch" ) );
    rmnews_news_dialog->rmnews_news_switch = GTK_SWITCH( GET_OBJ( "rmnews_news_switch" ) );
    if( remmina_pref.periodic_news_permitted == 1 )
        gtk_switch_set_active( rmnews_news_dialog->rmnews_news_switch, TRUE );
    else
        gtk_switch_set_active( rmnews_news_dialog->rmnews_news_switch, FALSE );
    gtk_widget_set_sensitive( GTK_WIDGET( rmnews_news_dialog->rmnews_news_switch ), RMNEWS_ENABLE_NEWS );

    rmnews_news_dialog->rmnews_button_close = GTK_BUTTON( GET_OBJ( "rmnews_button_close" ) );
    gtk_widget_set_can_default( GTK_WIDGET( rmnews_news_dialog->rmnews_button_close ), TRUE );
    gtk_widget_grab_default( GTK_WIDGET( rmnews_news_dialog->rmnews_button_close ) );

    if( remmina_pref.periodic_news_permitted == 1 )
    {
        char *contents = rmnews_get_file_contents( g_strdup( output_file_path ) );
        if( contents )
        {
            gtk_label_set_markup( rmnews_news_dialog->rmnews_label, contents );
            g_free( contents );
        }
    }

    g_signal_connect( rmnews_news_dialog->rmnews_button_close,
                      "clicked",
                      G_CALLBACK( rmnews_close_clicked ),
                      (gpointer)rmnews_news_dialog );
    g_signal_connect( rmnews_news_dialog->dialog, "close", G_CALLBACK( rmnews_close_clicked ), NULL );
    g_signal_connect( rmnews_news_dialog->dialog, "delete-event", G_CALLBACK( rmnews_dialog_deleted ), NULL );

    /* Connect signals */
    gtk_builder_connect_signals( rmnews_news_dialog->builder, NULL );

    /* Show the non-modal news dialog */
    gtk_widget_show_all( GTK_WIDGET( rmnews_news_dialog->dialog ) );
    gtk_window_present( GTK_WINDOW( rmnews_news_dialog->dialog ) );
    if( parent )
        gtk_window_set_transient_for( GTK_WINDOW( rmnews_news_dialog->dialog ), parent );
    gtk_window_set_modal( GTK_WINDOW( rmnews_news_dialog->dialog ), TRUE );
}

static void rmnews_get_url_cb( SoupSession *session, SoupMessage *msg, gpointer data )
{
    TRACE_CALL( __func__ );
    const char *name;
    const char *header;
    SoupBuffer *sb;
    FILE *output_file = NULL;
    const char *filesha = NULL;
    const char *filesha_after = NULL;
    GDateTime *gdt;
    gint64 unixts;

    REMMINA_DEBUG( "Status code %d", msg->status_code );

    name = soup_message_get_uri( msg )->path;

    gdt = g_date_time_new_now_utc();
    unixts = g_date_time_to_unix( gdt );
    g_date_time_unref( gdt );

    if( SOUP_STATUS_IS_CLIENT_ERROR( msg->status_code ) )
    {
        REMMINA_DEBUG( "Status 404 - Release file not available" );
        remmina_pref.periodic_rmnews_last_get = unixts;
        REMMINA_DEBUG( "periodic_rmnews_last_get set to %ld", remmina_pref.periodic_rmnews_last_get );
        REMMINA_DEBUG( "Saving preferences" );
        remmina_pref_save();
        return;
    }

    if( SOUP_STATUS_IS_SERVER_ERROR( msg->status_code ) )
    {
        REMMINA_DEBUG( "Server not available" );
        remmina_pref.periodic_rmnews_last_get = unixts;
        REMMINA_DEBUG( "periodic_rmnews_last_get set to %ld", remmina_pref.periodic_rmnews_last_get );
        REMMINA_DEBUG( "Saving preferences" );
        remmina_pref_save();
        return;
    }

    if( SOUP_STATUS_IS_TRANSPORT_ERROR( msg->status_code ) )
    {
        REMMINA_DEBUG( "Transport Error" );
        remmina_pref.periodic_rmnews_last_get = unixts;
        REMMINA_DEBUG( "periodic_rmnews_last_get set to %ld", remmina_pref.periodic_rmnews_last_get );
        REMMINA_DEBUG( "Saving preferences" );
        remmina_pref_save();
        return;
    }

    if( msg->status_code == SOUP_STATUS_SSL_FAILED )
    {
        GTlsCertificateFlags flags;

        if( soup_message_get_https_status( msg, NULL, &flags ) )
            REMMINA_DEBUG( "%s: %d %s (0x%x)\n", name, msg->status_code, msg->reason_phrase, flags );
        else
            REMMINA_DEBUG( "%s: %d %s (no handshake status)\n", name, msg->status_code, msg->reason_phrase );
        remmina_pref.periodic_rmnews_last_get = unixts;
        REMMINA_DEBUG( "periodic_rmnews_last_get set to %ld", remmina_pref.periodic_rmnews_last_get );
        REMMINA_DEBUG( "Saving preferences" );
        remmina_pref_save();
        return;
    }
    else if( SOUP_STATUS_IS_TRANSPORT_ERROR( msg->status_code ) )
    {
        REMMINA_DEBUG( "%s: %d %s\n", name, msg->status_code, msg->reason_phrase );
    }

    if( SOUP_STATUS_IS_REDIRECTION( msg->status_code ) )
    {
        header = soup_message_headers_get_one( msg->response_headers, "Location" );
        REMMINA_DEBUG( "Redirection detected" );
        if( header )
        {
            SoupURI *uri;
            char *uri_string;

            REMMINA_DEBUG( "  -> %s\n", header );

            uri = soup_uri_new_with_base( soup_message_get_uri( msg ), header );
            uri_string = soup_uri_to_string( uri, FALSE );
            rmnews_get_url( uri_string );
            g_free( uri_string );
            soup_uri_free( uri );
        }
        remmina_pref.periodic_rmnews_last_get = unixts;
        REMMINA_DEBUG( "periodic_rmnews_last_get set to %ld", remmina_pref.periodic_rmnews_last_get );
        REMMINA_DEBUG( "Saving preferences" );
        remmina_pref_save();
        return;
    }
    else if( SOUP_STATUS_IS_SUCCESSFUL( msg->status_code ) )
    {
        REMMINA_DEBUG( "Status 200" );
        if( output_file_path )
        {
            REMMINA_DEBUG( "Calculating the SHA1 of the local file" );
            filesha = remmina_sha1_file( output_file_path );
            REMMINA_DEBUG( "SHA1 is %s", filesha );
            if( filesha == NULL || filesha[0] == 0 )
                filesha = "0\0";
            REMMINA_DEBUG( "Opening %s output file for writing", output_file_path );
            output_file = fopen( output_file_path, "w" );
            if( !output_file )
            {
                g_printerr( "Error trying to create file %s.\n", output_file_path );
                remmina_pref.periodic_rmnews_last_get = unixts;
                REMMINA_DEBUG( "periodic_rmnews_last_get set to %ld", remmina_pref.periodic_rmnews_last_get );
                REMMINA_DEBUG( "Saving preferences" );
                remmina_pref_save();
                g_free( const_cast<char *>( filesha ) );
                filesha = NULL;
                return;
            }
        }
        else
        {
            REMMINA_DEBUG( "Cannot open output file for writing, because output_file_path is NULL" );
            remmina_pref.periodic_rmnews_last_get = unixts;
            REMMINA_DEBUG( "periodic_rmnews_last_get set to %ld", remmina_pref.periodic_rmnews_last_get );
            REMMINA_DEBUG( "Saving preferences" );
            remmina_pref_save();
            return;
        }

        sb = soup_message_body_flatten( msg->response_body );
        if( output_file )
        {
            fwrite( sb->data, 1, sb->length, output_file );

            if( output_file_path )
            {
                fclose( output_file );
                filesha_after = remmina_sha1_file( output_file_path );
            }
            REMMINA_DEBUG( "SHA1 after download is %s", filesha_after );
            if( g_strcmp0( filesha, filesha_after ) != 0 )
            {
                REMMINA_DEBUG( "SHA1 differs, we show the news and reset the counter" );
                remmina_pref.periodic_rmnews_last_get = 0;
                REMMINA_DEBUG( "periodic_rmnews_last_get set to %ld", remmina_pref.periodic_rmnews_last_get );
                REMMINA_DEBUG( "Saving preferences" );
                GtkWindow *parent = remmina_main_get_window();
                if( !kioskmode && kioskmode == FALSE )
                    rmnews_show_news( parent );
            }
            else
            {
                remmina_pref.periodic_rmnews_last_get = unixts;
            }
            /* Increase counter with number of successful GETs */
            remmina_pref.periodic_rmnews_get_count = remmina_pref.periodic_rmnews_get_count + 1;
            remmina_pref_save();
            g_free( const_cast<char *>( filesha ) );
            filesha = NULL;
        }
    }
    g_object_unref( msg );
}

/**
 * Try to get a unique system+user ID to identify this remmina user
 * and avoid some duplicated task, especially on news management
 * @return a string
 * @warning The returned string must be freed with g_free.
 */
char *rmnews_get_uid()
{
    TRACE_CALL( __func__ );
    GChecksum *chs;
    const char *uname, *hname;
    const char *uid_suffix;
    char *uid_prefix;
    char *uid;

    /* This code is very similar to remmina_stats_get_uid() */

    if( remmina_pref.periodic_rmnews_uuid_prefix == NULL || remmina_pref.periodic_rmnews_uuid_prefix[0] == 0 )
    {
        /* Generate a new UUID_PREFIX for news on this installation */
        uid_prefix = remmina_gen_random_uuid();
        if( remmina_pref.periodic_rmnews_uuid_prefix )
            g_free( remmina_pref.periodic_rmnews_uuid_prefix );
        remmina_pref.periodic_rmnews_uuid_prefix = uid_prefix;
        remmina_pref_save();
    }

    uname = g_get_user_name();
    hname = g_get_host_name();
    chs = g_checksum_new( G_CHECKSUM_SHA256 );
    g_checksum_update( chs, (const guchar *)uname, strlen( uname ) );
    g_checksum_update( chs, (const guchar *)hname, strlen( hname ) );
    uid_suffix = g_checksum_get_string( chs );

    uid = g_strdup_printf( "02-%s-%.10s", remmina_pref.periodic_rmnews_uuid_prefix, uid_suffix );
    g_checksum_free( chs );

    return uid;
}

void rmnews_get_url( const char *url )
{
    TRACE_CALL( __func__ );

    SoupMessage *msg;

    msg = soup_message_new( "GET", url );
    soup_message_set_flags( msg, SOUP_MESSAGE_NO_REDIRECT );

    REMMINA_DEBUG( "Fetching %s", url );

    g_object_ref( msg );
    soup_session_queue_message( session, msg, rmnews_get_url_cb, NULL );
}

void rmnews_get_news()
{
    TRACE_CALL( __func__ );

    SoupLogger *logger = NULL;
    int fd;
    char *uid;
    char mage[20], gcount[20];
    struct stat sb;

    char *cachedir = g_build_path( "/", g_get_user_cache_dir(), REMMINA_APP_ID, NULL );
    gint d = g_mkdir_with_parents( cachedir, 0750 );
    if( d < 0 )
        output_file_path = RMNEWS_OUTPUT;
    else
        output_file_path = g_build_path( "/", cachedir, "latest_news.md", NULL );

    REMMINA_DEBUG( "Output file set to %s", output_file_path );

    if( remmina_pref.periodic_rmnews_last_get == 0 && remmina_pref.periodic_rmnews_get_count == 0 )
    {
        g_file_set_contents( output_file_path, "", 0, NULL );
        /* Just a symolic date */
        remmina_pref.periodic_rmnews_last_get = 191469343000;
        REMMINA_DEBUG( "periodic_rmnews_last_get set to %ld", remmina_pref.periodic_rmnews_last_get );
        REMMINA_DEBUG( "Preferences NOT saved" );
    }

    fd = g_open( output_file_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
    REMMINA_DEBUG( "Returned %d while creating %s", fd, output_file_path );
    /* If we cannot create the remmina_news file, we avoid connections */
    if( fd < 0 )
    {
        REMMINA_DEBUG( "Cannot store the remmina news file" );
        return;
    }
    g_close( fd, NULL );

    REMMINA_DEBUG( "Output file %s created successfully", output_file_path );

    if( output_file_path )
    {
    }
    else
    {
        REMMINA_DEBUG( "Output file set to %s", output_file_path );
    }

    REMMINA_DEBUG( "Gathering news" );
    session = static_cast<SoupSession *>( g_object_new( SOUP_TYPE_SESSION,
                                                        SOUP_SESSION_ADD_FEATURE_BY_TYPE,
                                                        SOUP_TYPE_CONTENT_DECODER,
                                                        SOUP_SESSION_ADD_FEATURE_BY_TYPE,
                                                        SOUP_TYPE_COOKIE_JAR,
                                                        SOUP_SESSION_USER_AGENT,
                                                        "get ",
                                                        SOUP_SESSION_ACCEPT_LANGUAGE_AUTO,
                                                        TRUE,
                                                        NULL ) );
    /* TODO: Catch log level and set SOUP_LOGGER_LOG_MINIMAL or more */
    logger = soup_logger_new( SOUP_LOGGER_LOG_NONE, -1 );
    soup_session_add_feature( session, SOUP_SESSION_FEATURE( logger ) );
    g_object_unref( logger );

    char *lang = remmina_utils_get_lang();
    REMMINA_DEBUG( "Language %s", lang );

    uid = rmnews_get_uid();

    if( stat( "/etc/machine-id", &sb ) == 0 )
        sprintf( mage, "%ld", (long)( time( NULL ) - sb.st_mtim.tv_sec ) );
    else
        strcpy( mage, "0" );

    sprintf( gcount, "%ld", remmina_pref.periodic_rmnews_get_count );

    rmnews_get_url( g_strconcat( REMMINA_URL,
                                 "news/remmina_news.php?lang=",
                                 lang,
                                 "&ver=" VERSION,
                                 "&uid=",
                                 uid,
                                 "&sa=0",
                                 "&mage=",
                                 mage,
                                 "&gcount=",
                                 gcount,
                                 NULL ) );

    g_free( uid );
    g_object_unref( session );
}

static int rmnews_periodic_check( gpointer user_data )
{
    TRACE_CALL( __func__ );
    GDateTime *gdt;
    gint64 unixts;
    glong next = 0;

    gdt = g_date_time_new_now_utc();
    unixts = g_date_time_to_unix( gdt );
    g_date_time_unref( gdt );

    /* if remmina_pref is not writable ... */
    if( remmina_pref_is_rw() == FALSE && remmina_pref.periodic_rmnews_last_get == 0 )
    {
        /* We randomly set periodic_rmnews_last_get to a day between today
		 * and 7 days ago */
        gint randidx = randombytes_uniform( 8 );
        REMMINA_DEBUG( "Setting a random periodic_rmnews_last_get to %d - %d", unixts, eweekdays[randidx] );
        remmina_pref.periodic_rmnews_last_get = unixts - eweekdays[randidx];
    }
    //REMMINA_DEBUG("periodic_rmnews_last_get is %ld", remmina_pref.periodic_rmnews_last_get);

    if( remmina_pref.periodic_news_permitted == 0 && remmina_pref.periodic_rmnews_get_count < 1 )
    {
        remmina_pref.periodic_rmnews_last_get =
            ( remmina_pref.periodic_rmnews_last_get > 1514764800 ? remmina_pref.periodic_rmnews_last_get : 0 );
        remmina_pref_save();
    }
    next = remmina_pref.periodic_rmnews_last_get + RMNEWS_INTERVAL_SEC;
    if( unixts > next || ( unixts < remmina_pref.periodic_rmnews_last_get && unixts > 1514764800 ) )
    {
        //REMMINA_DEBUG("remmina_pref.periodic_news_permitted is %d", remmina_pref.periodic_news_permitted);
        if( remmina_pref.periodic_news_permitted == 1 )
        {
            rmnews_get_news();
        }
        else if( remmina_pref.periodic_rmnews_get_count == 0 )
        {
            rmnews_show_news( remmina_main_get_window() );
            remmina_pref.periodic_rmnews_get_count = 1;
            remmina_pref_save();
        }
    }
    return G_SOURCE_CONTINUE;
}

void rmnews_schedule()
{
    TRACE_CALL( __func__ );
    remmina_scheduler_setup( rmnews_periodic_check, NULL, RMNEWS_CHECK_1ST_MS, RMNEWS_CHECK_INTERVAL_MS );
}
