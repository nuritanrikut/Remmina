# Remmina - The GTK+ Remote Desktop Client
#
# Copyright (C) 2011 Marc-Andre Moreau
# Copyright (C) 2014-2015 Antenore Gatta, Fabio Castelli, Giovanni Panozzo
# Copyright (C) 2016-2022 Antenore Gatta, Giovanni Panozzo
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.
#
# In addition, as a special exception, the copyright holders give
# permission to link the code of portions of this program with the
# OpenSSL library under certain conditions as described in each
# individual source file, and distribute linked combinations
# including the two.
# You must obey the GNU General Public License in all respects
# for all of the code used other than OpenSSL. If you modify
# file(s) with this exception, you may extend this exception to your
# version of the file(s), but you are not obligated to do so. If you
# do not wish to do so, delete this exception statement from your
# version. If you delete this exception statement from all source
# files in the program, then also delete it here.

# Needed FreeRDP version to build freerdp plugin


include_directories(${CMAKE_SOURCE_DIR}/plugins)

set(APPICONSCALE_EMBLEMS_DIR "${REMMINA_DATADIR}/icons/hicolor/scalable/emblems")

set(REMMINA_COMMON_INCLUDE_DIRS ${GTK3_INCLUDE_DIRS})
set(REMMINA_COMMON_LIBRARIES ${GTK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

find_suggested_package(LIBSSH)
if(LIBSSH_FOUND)
    add_definitions(-DHAVE_LIBSSH)
    include_directories(${SSH_INCLUDE_DIRS})
    target_link_libraries(remmina ${SSH_LIBRARIES})
endif()

set(FREERDP_REQUIRED_MAJOR 2)
set(FREERDP_REQUIRED_MINOR 0)
set(FREERDP_REQUIRED_REVISION 0)
set(FREERDP_REQUIRED_VERSIONSTRING
    ${FREERDP_REQUIRED_MAJOR}.${FREERDP_REQUIRED_MINOR}.${FREERDP_REQUIRED_REVISION})
find_package(FREERDP3)
find_package(FREERDP)
if(FREERDP3_FOUND)
    add_definitions(-DWITH_FREERDP3)
    option(WITH_FREERDP3 "Building RDP with FREERDP 3" ON)
    set(FREERDP_REQUIRED_MAJOR 3)
    set(FREERDP_FOUND FREERDP3_FOUND)
    add_subdirectory(rdp)
elseif(FREERDP_FOUND)
    add_subdirectory(rdp)
endif()

# X2Go
option(WITH_X2GO "Build X2Go plugin" OFF)
if(WITH_X2GO)
    message(STATUS "Enabling X2Go plugin.")
    message(AUTHOR_WARNING "pyhoca-cli is needed at runtime, be sure to install it")
    add_subdirectory(x2go)
else()
    message(STATUS "Disabling X2Go plugin.")
endif()

find_suggested_package(LIBVNCSERVER)
if(LIBVNCSERVER_FOUND)
    add_subdirectory(vnc)
endif()

option(WITH_GVNC "Build GVNC plugin" OFF)
if(WITH_GVNC)
    message(STATUS "Enabling GVNC plugin.")
    add_definitions(-DWITH_GVNC)
    find_suggested_package(GTK-VNC)
    if(GTK-VNC_FOUND)
        add_subdirectory(gvnc)
    endif()
else()
    message(STATUS "Disabling GVNC plugin.")
endif()

find_suggested_package(Spice)
if(SPICE_FOUND)
    add_subdirectory(spice)
endif()

option(WITH_WWW "Build WWW plugin" ON)
if(WITH_WWW)
    message(STATUS "Enabling WWW plugin.")
    add_definitions(-DWITH_WWW)
    pkg_search_module(WEBKIT2
        webkit2gtk-4.0
        webkit2gtk-3.0
        libwebkit2gtk-4.0
        libwebkit2gtk-3.0
        )
    if(WEBKIT2_FOUND)
        message(STATUS "WWW plugin dependencies found")
        add_subdirectory(www)
    else()
        message(WARNING "WEBKIT not found")
    endif()
else()
    message(STATUS "Disabling WWW plugin.")
endif()

if(WITH_EXAMPLES)
    message(STATUS "Enabling examples and test plugins.")
    add_subdirectory(tool_hello_world)
endif()

add_subdirectory(exec)

option(WITH_KF5WALLET "Building KF5WALLET plugin" OFF)
if(WITH_KF5WALLET)
    CheckHasModule(KF5Wallet)
    if(HAS_MODULE_KF5Wallet)
        message(STATUS "Enabling KDE Wallet plugin.")
        add_definitions(-DWITH_KF5WALLET)
        add_subdirectory(kwallet)
    else()
        message(FATAL_ERROR "libKF5wallet not found but requested")
    endif()
endif()

option(WITH_ST "Build Simple Terminal plugin" OFF)
if(WITH_ST)
    message(STATUS "Enabling ST plugin.")
    add_definitions(-DWITH_ST)
    get_filename_component(_st_fullpath "st" REALPATH)
    if(EXISTS "${_st_fullpath}" AND EXISTS "${_st_fullpath}/CMakeLists.txt")
        add_subdirectory(st)
    else()
        message(WARNING "Can't find ${_st_fullpath}/CMakeLists.txt")
    endif()
endif()

option(WITH_XDMCP "Build XDMCP plugin" OFF)
if(WITH_XDMCP)
    add_definitions(-DWITH_XDMCP)
    message(STATUS "Enabling XDMCP plugin.")
    get_filename_component(_xdmcp_fullpath "xdmcp" REALPATH)
    if(EXISTS "${_xdmcp_fullpath}" AND EXISTS "${_xdmcp_fullpath}/CMakeLists.txt")
        add_subdirectory(xdmcp)
    else()
        message(WARNING "Can't find ${_xdmcp_fullpath}/CMakeLists.txt")
    endif()
endif()

option(WITH_NX "Build NX plugin" OFF)
if(WITH_NX)
    add_definitions(-DWITH_NX)
    # After the plugin split only the NX plugin needs XKBfile.h
    find_required_package(XKBFILE)
    if(LIBSSH_FOUND AND XKBFILE_FOUND)
        message(STATUS "Enabling NX plugin.")
        get_filename_component(_nx_fullpath "nx" REALPATH)
        if(EXISTS "${_nx_fullpath}" AND EXISTS "${_nx_fullpath}/CMakeLists.txt")
            add_subdirectory(nx)
        else()
            message(WARNING "Can't find ${_nx_fullpath}/CMakeLists.txt")
        endif()
    endif()
endif()

#find_suggested_package(TELEPATHY)
#if(TELEPATHY_FOUND)
#add_subdirectory(telepathy)
#endif()
