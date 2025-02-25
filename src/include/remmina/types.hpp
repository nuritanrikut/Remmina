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

#pragma once

#include <glib.h>

struct RemminaProtocolWidget;

enum RemminaProtocolFeatureType
{
    REMMINA_PROTOCOL_FEATURE_TYPE_END,
    REMMINA_PROTOCOL_FEATURE_TYPE_PREF,
    REMMINA_PROTOCOL_FEATURE_TYPE_TOOL,
    REMMINA_PROTOCOL_FEATURE_TYPE_UNFOCUS,
    REMMINA_PROTOCOL_FEATURE_TYPE_SCALE,
    REMMINA_PROTOCOL_FEATURE_TYPE_DYNRESUPDATE,
    REMMINA_PROTOCOL_FEATURE_TYPE_MULTIMON,
    REMMINA_PROTOCOL_FEATURE_TYPE_GTKSOCKET
};

#define REMMINA_PROTOCOL_FEATURE_PREF_RADIO 1
#define REMMINA_PROTOCOL_FEATURE_PREF_CHECK 2

struct RemminaProtocolFeature
{
    RemminaProtocolFeatureType type;
    gint id;
    gpointer opt1;
    gpointer opt2;
    gpointer opt3;
};

struct RemminaPluginScreenshotData
{
    unsigned char *buffer;
    int bitsPerPixel;
    int bytesPerPixel;
    int width;
    int height;
};

typedef int ( *RemminaXPortTunnelInitFunc )( RemminaProtocolWidget *gp,
                                             gint remotedisplay,
                                             const char *server,
                                             gint port );

enum RemminaProtocolSettingType
{
    REMMINA_PROTOCOL_SETTING_TYPE_END,

    REMMINA_PROTOCOL_SETTING_TYPE_SERVER,
    REMMINA_PROTOCOL_SETTING_TYPE_PASSWORD,
    REMMINA_PROTOCOL_SETTING_TYPE_RESOLUTION,
    REMMINA_PROTOCOL_SETTING_TYPE_KEYMAP,

    REMMINA_PROTOCOL_SETTING_TYPE_TEXT,
    REMMINA_PROTOCOL_SETTING_TYPE_TEXTAREA,
    REMMINA_PROTOCOL_SETTING_TYPE_SELECT,
    REMMINA_PROTOCOL_SETTING_TYPE_COMBO,
    REMMINA_PROTOCOL_SETTING_TYPE_CHECK,
    REMMINA_PROTOCOL_SETTING_TYPE_FILE,
    REMMINA_PROTOCOL_SETTING_TYPE_FOLDER,
    REMMINA_PROTOCOL_SETTING_TYPE_INT,
    REMMINA_PROTOCOL_SETTING_TYPE_DOUBLE
};

struct RemminaProtocolSetting
{
    RemminaProtocolSettingType type;
    const char *name;
    const char *label;
    gboolean compact;
    gpointer opt1;
    gpointer opt2;
    gpointer validator_data;
    GCallback validator;
};

enum RemminaProtocolSSHSetting
{
    REMMINA_PROTOCOL_SSH_SETTING_NONE,
    REMMINA_PROTOCOL_SSH_SETTING_TUNNEL,
    REMMINA_PROTOCOL_SSH_SETTING_SSH,
    REMMINA_PROTOCOL_SSH_SETTING_REVERSE_TUNNEL,
    REMMINA_PROTOCOL_SSH_SETTING_SFTP
};

enum RemminaAuthpwdType
{
    REMMINA_AUTHPWD_TYPE_PROTOCOL,
    REMMINA_AUTHPWD_TYPE_SSH_PWD,
    REMMINA_AUTHPWD_TYPE_SSH_PRIVKEY
};

enum RemminaScaleMode
{
    REMMINA_PROTOCOL_WIDGET_SCALE_MODE_NONE = 0,
    REMMINA_PROTOCOL_WIDGET_SCALE_MODE_SCALED = 1,
    REMMINA_PROTOCOL_WIDGET_SCALE_MODE_DYNRES = 2
};

enum RemminaProtocolWidgetResolutionMode
{
    RES_INVALID = -1,
    RES_USE_CUSTOM = 0,
    RES_USE_CLIENT = 1,
    RES_USE_INITIAL_WINDOW_SIZE = 2
};

/* pflags field for remmina_protocol_widget_panel_auth() */
enum RemminaMessagePanelFlags
{
    REMMINA_MESSAGE_PANEL_FLAG_USERNAME = 1,          /* require username in auth panel */
    REMMINA_MESSAGE_PANEL_FLAG_USERNAME_READONLY = 2, /* Username, if required, is readonly */
    REMMINA_MESSAGE_PANEL_FLAG_DOMAIN = 4,            /* require domain in auth panel */
    REMMINA_MESSAGE_PANEL_FLAG_SAVEPASSWORD = 8       /* require savepassword switch in auth panel */

};
