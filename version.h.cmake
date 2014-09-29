/*
   Copyright (C) 2007-2010 Sebastian Trueg <trueg@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) version 3, or any
   later version accepted by the membership of KDE e.V. (or its
   successor approved by the membership of KDE e.V.), which shall
   act as a proxy defined in Section 6 of version 3 of the license.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _TVDB_VERSION_H_
#define _TVDB_VERSION_H_

#include "tvdb_export.h"

/// @brief Tvdb version as string at compile time.
#define TVDB_VERSION_STRING "${CMAKE_LIBTVDB_VERSION_STRING}"

/// @brief The major Tvdb version number at compile time
#define TVDB_VERSION_MAJOR ${CMAKE_LIBTVDB_VERSION_MAJOR}

/// @brief The minor Tvdb version number at compile time
#define TVDB_VERSION_MINOR ${CMAKE_LIBTVDB_VERSION_MINOR}

/// @brief The Tvdb release version number at compile time
#define TVDB_VERSION_RELEASE ${CMAKE_LIBTVDB_VERSION_RELEASE}

/**
 * \brief Create a unique number from the major, minor and release number of a %Tvdb version
 *
 * This function can be used for preprocessing. For version information at runtime
 * use the version methods in the Tvdb namespace.
 */
#define TVDB_MAKE_VERSION( a,b,c ) (((a) << 16) | ((b) << 8) | (c))

/**
 * \brief %Tvdb Version as a unique number at compile time
 *
 * This macro calculates the %Tvdb version into a number. It is mainly used
 * through TVDB_IS_VERSION in preprocessing. For version information at runtime
 * use the version methods in the Tvdb namespace.
 */
#define TVDB_VERSION \
    TVDB_MAKE_VERSION(TVDB_VERSION_MAJOR,TVDB_VERSION_MINOR,TVDB_VERSION_RELEASE)

/**
 * \brief Check if the %Tvdb version matches a certain version or is higher
 *
 * This macro is typically used to compile conditionally a part of code:
 * \code
 * #if TVDB_IS_VERSION(2,1)
 * // Code for Tvdb 2.1
 * #else
 * // Code for Tvdb 2.0
 * #endif
 * \endcode
 *
 * For version information at runtime
 * use the version methods in the Tvdb namespace.
 */
#define TVDB_IS_VERSION(a,b,c) ( TVDB_VERSION >= TVDB_MAKE_VERSION(a,b,c) )


namespace Tvdb {
    /**
     * @brief Returns the major number of Tvdb's version, e.g.
     * 1 for %Tvdb 1.0.2.
     * @return the major version number at runtime.
     */
    TVDB_EXPORT unsigned int versionMajor();

    /**
     * @brief Returns the minor number of Tvdb's version, e.g.
     * 0 for %Tvdb 1.0.2.
     * @return the minor version number at runtime.
     */
    TVDB_EXPORT unsigned int versionMinor();

    /**
     * @brief Returns the release of Tvdb's version, e.g.
     * 2 for %Tvdb 1.0.2.
     * @return the release number at runtime.
     */
    TVDB_EXPORT unsigned int versionRelease();

    /**
     * @brief Returns the %Tvdb version as string, e.g. "1.0.2".
     *
     * On contrary to the macro TVDB_VERSION_STRING this function returns
     * the version number of Tvdb at runtime.
     * @return the %Tvdb version. You can keep the string forever
     */
    TVDB_EXPORT const char* versionString();
}

#endif
