/*
 * Remmina - The GTK+ Remote Desktop Client
 * Copyright (C) 2010 Vic Lee
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

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "remmina_public.hpp"
#include "remmina_log.hpp"
#include "remmina_stats.hpp"
#include "remmina/remmina_trace_calls.hpp"

bool logstart;

/***** Define the log window GUI *****/
#define REMMINA_TYPE_LOG_WINDOW ( remmina_log_window_get_type() )
#define REMMINA_LOG_WINDOW( obj ) ( G_TYPE_CHECK_INSTANCE_CAST( ( obj ), REMMINA_TYPE_LOG_WINDOW, RemminaLogWindow ) )
#define REMMINA_LOG_WINDOW_CLASS( klass ) \
    ( G_TYPE_CHECK_CLASS_CAST( ( klass ), REMMINA_TYPE_LOG_WINDOW, RemminaLogWindowClass ) )
#define REMMINA_IS_LOG_WINDOW( obj ) ( G_TYPE_CHECK_INSTANCE_TYPE( ( obj ), REMMINA_TYPE_LOG_WINDOW ) )
#define REMMINA_IS_LOG_WINDOW_CLASS( klass ) ( G_TYPE_CHECK_CLASS_TYPE( ( klass ), REMMINA_TYPE_LOG_WINDOW ) )
#define REMMINA_LOG_WINDOW_GET_CLASS( obj ) \
    ( G_TYPE_INSTANCE_GET_CLASS( ( obj ), REMMINA_TYPE_LOG_WINDOW, RemminaLogWindowClass ) )

struct RemminaLogWindow
{
    GtkWindow window;

    GtkWidget *log_view;
    GtkTextBuffer *log_buffer;
};

struct RemminaLogWindowClass
{
    GtkWindowClass parent_class;
};

GType remmina_log_window_get_type() G_GNUC_CONST;

G_DEFINE_TYPE( RemminaLogWindow, remmina_log_window, GTK_TYPE_WINDOW )

void remmina_log_stats()
{
    TRACE_CALL( __func__ );
    JsonNode *n;

    n = remmina_stats_get_all();
    if( n != NULL )
    {
        JsonGenerator *g = json_generator_new();
        json_generator_set_pretty( g, TRUE );
        json_generator_set_root( g, n );
        json_node_unref( n );
        g_autofree char *s = json_generator_to_data( g, NULL ); // s=serialized stats
        REMMINA_DEBUG( "STATS: JSON data%s\n", s );
        g_object_unref( g );
    }
}

static void remmina_log_window_class_init( RemminaLogWindowClass *klass )
{
    TRACE_CALL( __func__ );
}

/* We will always only have one log window per instance */
static GtkWidget *log_window = NULL;

static GtkWidget *remmina_log_window_new()
{
    TRACE_CALL( __func__ );
    return GTK_WIDGET( g_object_new( REMMINA_TYPE_LOG_WINDOW, NULL ) );
}

static void remmina_log_end( GtkWidget *widget, gpointer data )
{
    TRACE_CALL( __func__ );
    log_window = NULL;
}

static void remmina_log_start_stop( GtkSwitch *logswitch, gpointer user_data )
{
    TRACE_CALL( __func__ );
    logstart = !logstart;
}

void remmina_log_start()
{
    TRACE_CALL( __func__ );
    if( log_window )
    {
        gtk_window_present( GTK_WINDOW( log_window ) );
    }
    else
    {
        log_window = remmina_log_window_new();
        gtk_window_set_default_size( GTK_WINDOW( log_window ), 640, 480 );
        gtk_window_set_resizable( GTK_WINDOW( log_window ), TRUE );
        gtk_window_set_decorated( GTK_WINDOW( log_window ), TRUE );

        /* Header bar */
        GtkWidget *header = gtk_header_bar_new();
        gtk_header_bar_set_show_close_button( GTK_HEADER_BAR( header ), TRUE );
        gtk_header_bar_set_title( GTK_HEADER_BAR( header ), _( "Remmina debugging window" ) );
        gtk_header_bar_set_has_subtitle( GTK_HEADER_BAR( header ), FALSE );
        /* Stats */
        GtkWidget *getstat = gtk_button_new();
        gtk_widget_set_tooltip_text( getstat, _( "Paste sytem info in the Remmina debugging window" ) );
        GIcon *icon = g_themed_icon_new( "edit-paste-symbolic" );
        GtkWidget *image = gtk_image_new_from_gicon( icon, GTK_ICON_SIZE_BUTTON );
        g_object_unref( icon );
        gtk_container_add( GTK_CONTAINER( getstat ), image );
        gtk_header_bar_pack_start( GTK_HEADER_BAR( header ), getstat );
        /* Start logging */
        GtkWidget *start = gtk_switch_new();
        logstart = TRUE;
        gtk_switch_set_active( GTK_SWITCH( start ), logstart );
        gtk_header_bar_pack_start( GTK_HEADER_BAR( header ), start );

        gtk_window_set_titlebar( GTK_WINDOW( log_window ), header );

        g_signal_connect( getstat, "button-press-event", G_CALLBACK( remmina_log_stats ), NULL );
        g_signal_connect( start, "notify::active", G_CALLBACK( remmina_log_start_stop ), NULL );
        g_signal_connect( G_OBJECT( log_window ), "destroy", G_CALLBACK( remmina_log_end ), NULL );
        gtk_widget_show_all( log_window );
    }

    remmina_log_print(
        _( "This window can help you find connection problems.\n"
           "You can stop and start the logging at any moment using the On/Off switch.\n"
           "The stats button (Ctrl+T), can be useful to gather system info you may share when reporting a bug.\n"
           "There is more info about debugging Remmina on "
           "https://gitlab.com/Remmina/Remmina/-/wikis/Usage/Remmina-debugging\n" ) );
}

int remmina_log_running()
{
    TRACE_CALL( __func__ );
    return ( log_window != NULL );
}

static int remmina_log_scroll_to_end( gpointer data )
{
    TRACE_CALL( __func__ );
    GtkTextIter iter;

    if( log_window )
    {
        gtk_text_buffer_get_end_iter( REMMINA_LOG_WINDOW( log_window )->log_buffer, &iter );
        gtk_text_view_scroll_to_iter(
            GTK_TEXT_VIEW( REMMINA_LOG_WINDOW( log_window )->log_view ), &iter, 0.0, FALSE, 0.0, 0.0 );
    }
    return FALSE;
}

static int remmina_log_print_real( gpointer data )
{
    TRACE_CALL( __func__ );
    GtkTextIter iter;

    if( log_window && logstart )
    {
        gtk_text_buffer_get_end_iter( REMMINA_LOG_WINDOW( log_window )->log_buffer, &iter );
        gtk_text_buffer_insert( REMMINA_LOG_WINDOW( log_window )->log_buffer, &iter, (const char *)data, -1 );
        IDLE_ADD( remmina_log_scroll_to_end, NULL );
    }
    g_free( data );
    return FALSE;
}

// Only prints into Remmina's own debug window. (Not stdout!)
// See _remmina_{debug, info, error, critical, warning}
void remmina_log_print( const char *text )
{
    TRACE_CALL( __func__ );
    if( !log_window )
        return;

    IDLE_ADD( remmina_log_print_real, g_strdup( text ) );
}

void _remmina_info( const char *fmt, ... )
{
    TRACE_CALL( __func__ );

    va_list args;
    g_autofree char *text;
    va_start( args, fmt );
    text = g_strdup_vprintf( fmt, args );
    va_end( args );

    // always appends newline
    g_info( "%s", text );

    g_autofree char *buf_tmp = g_strconcat( text, "\n", NULL );
    /* freed in remmina_log_print_real */
    char *bufn = g_strconcat( "(INFO) - ", buf_tmp, NULL );

    if( !log_window )
    {
        free( bufn );
        return;
    }
    IDLE_ADD( remmina_log_print_real, bufn );
}

void _remmina_message( const char *fmt, ... )
{
    TRACE_CALL( __func__ );

    va_list args;
    g_autofree char *text;
    va_start( args, fmt );
    text = g_strdup_vprintf( fmt, args );
    va_end( args );

    // always appends newline
    g_message( "%s", text );

    if( !log_window )
    {
        return;
    }

    g_autofree char *buf_tmp = g_strconcat( text, "\n", NULL );
    /* freed in remmina_log_print_real */
    char *bufn = g_strconcat( "(MESSAGE) - ", buf_tmp, NULL );

    IDLE_ADD( remmina_log_print_real, bufn );
}

/**
 * Print a string in the Remmina Debug Windows and in the terminal.
 * The string will be visible in the terminal if G_MESSAGES_DEBUG=all
 * Variadic function of REMMINA_DEBUG
 */
void _remmina_debug( const char *fun, const char *fmt, ... )
{
    TRACE_CALL( __func__ );

    va_list args;
    char *text;
    va_start( args, fmt );
    text = g_strdup_vprintf( fmt, args );
    va_end( args );

    g_autofree char *buf = g_strconcat( "(", fun, ") - ", text, NULL );
    g_free( text );

    // always appends newline
    g_debug( "%s", buf );

    if( !log_window )
    {
        return;
    }

    g_autofree char *buf_tmp = g_strconcat( buf, "\n", NULL );
    /* freed in remmina_log_print_real */
    char *bufn = g_strconcat( "(DEBUG) - ", buf_tmp, NULL );

    IDLE_ADD( remmina_log_print_real, bufn );
}

void _remmina_warning( const char *fun, const char *fmt, ... )
{
    TRACE_CALL( __func__ );

    va_list args;
    char *text;
    va_start( args, fmt );
    text = g_strdup_vprintf( fmt, args );
    va_end( args );

    g_autofree char *buf = g_strconcat( "(", fun, ") - ", text, NULL );
    g_free( text );

    // always appends newline
    g_warning( "%s", buf );

    if( !log_window )
    {
        return;
    }

    g_autofree char *buf_tmp = g_strconcat( buf, "\n", NULL );
    /* freed in remmina_log_print_real */
    char *bufn = g_strconcat( "(WARN) - ", buf_tmp, NULL );

    IDLE_ADD( remmina_log_print_real, bufn );
}

// !!! Calling this function will crash Remmina !!!
// !!! purposefully and send a trap signal !!!
void _remmina_error( const char *fun, const char *fmt, ... )
{
    TRACE_CALL( __func__ );

    va_list args;
    char *text;
    va_start( args, fmt );
    text = g_strdup_vprintf( fmt, args );
    va_end( args );

    g_autofree char *buf = g_strconcat( "(", fun, ") - ", text, NULL );
    g_free( text );

    // always appends newline
    g_error( "%s", buf );

    if( !log_window )
    {
        return;
    }

    g_autofree char *buf_tmp = g_strconcat( buf, "\n", NULL );
    /* freed in remmina_log_print_real */
    char *bufn = g_strconcat( "(ERROR) - ", buf_tmp, NULL );

    IDLE_ADD( remmina_log_print_real, bufn );
}

void _remmina_critical( const char *fun, const char *fmt, ... )
{
    TRACE_CALL( __func__ );

    va_list args;
    char *text;
    va_start( args, fmt );
    text = g_strdup_vprintf( fmt, args );
    va_end( args );

    g_autofree char *buf = g_strconcat( "(", fun, ") - ", text, NULL );
    g_free( text );

    // always appends newline
    g_critical( "%s", buf );

    if( !log_window )
    {
        return;
    }

    g_autofree char *buf_tmp = g_strconcat( buf, "\n", NULL );
    /* freed in remmina_log_print_real */
    char *bufn = g_strconcat( "(CRIT) - ", buf_tmp, NULL );

    IDLE_ADD( remmina_log_print_real, bufn );
}

// Only prints into Remmina's own debug window. (Not stdout!)
// See _remmina_{message, info, debug warning, error, critical}
void remmina_log_printf( const char *fmt, ... )
{
    TRACE_CALL( __func__ );
    va_list args;
    char *text;

    if( !log_window )
        return;

    va_start( args, fmt );
    text = g_strdup_vprintf( fmt, args );
    va_end( args );

    IDLE_ADD( remmina_log_print_real, text );
}

static int remmina_log_on_keypress( GtkWidget *widget, GdkEvent *event, gpointer user_data )
{
    TRACE_CALL( __func__ );

    if( !log_window )
        return FALSE;

    GdkEventKey *e = (GdkEventKey *)event;

    if( ( e->state & GDK_CONTROL_MASK ) == GDK_CONTROL_MASK )
    {
        if( e->keyval == GDK_KEY_t )
        {
            remmina_log_stats();
        }
        return TRUE;
    }

    return FALSE;
}

static void remmina_log_window_init( RemminaLogWindow *logwin )
{
    TRACE_CALL( __func__ );
    GtkWidget *scrolledwindow;
    GtkWidget *widget;

    gtk_container_set_border_width( GTK_CONTAINER( logwin ), 4 );

    scrolledwindow = gtk_scrolled_window_new( NULL, NULL );
    gtk_widget_show( scrolledwindow );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolledwindow ), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS );
    gtk_container_add( GTK_CONTAINER( logwin ), scrolledwindow );

    widget = gtk_text_view_new();
    gtk_widget_show( widget );
    gtk_text_view_set_wrap_mode( GTK_TEXT_VIEW( widget ), GTK_WRAP_WORD_CHAR );
    gtk_text_view_set_editable( GTK_TEXT_VIEW( widget ), FALSE );
    gtk_text_view_set_monospace( GTK_TEXT_VIEW( widget ), TRUE );
    gtk_container_add( GTK_CONTAINER( scrolledwindow ), widget );
    logwin->log_view = widget;
    logwin->log_buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW( widget ) );

    g_signal_connect(
        G_OBJECT( logwin->log_view ), "key-press-event", G_CALLBACK( remmina_log_on_keypress ), (gpointer)logwin );
}
