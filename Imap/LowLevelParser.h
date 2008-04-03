/* Copyright (C) 2007 Jan Kundrát <jkt@gentoo.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef IMAP_LOWLEVELPARSER_H
#define IMAP_LOWLEVELPARSER_H

#include <QList>
#include <QPair>
#include <QVariant>

namespace Imap {
namespace LowLevelParser {

    enum ParsedAs {
        ATOM,
        QUOTED,
        LITERAL,
        NIL
    };

    uint getUInt( const QByteArray& line, int& start );

    QByteArray getAtom( const QByteArray& line, int& start );

    QPair<QByteArray,ParsedAs> getString( const QByteArray& line, int& start );

    QPair<QByteArray,ParsedAs> getAString( const QByteArray& line, int& start );

    QPair<QByteArray,ParsedAs> getNString( const QByteArray& line, int& start );

    QString getMailbox( const QByteArray& line, int& start );

    /** @short Parse parenthesized list 
     *
     * Parenthesized lists is defined as a sequence of space-separated strings
     * enclosed between "open" and "close" characters.
     *
     * open, close -- enclosing parentheses
     * line, start -- full line data and starting offset
     * allowNoList -- if false and there's no opening parenthesis, exception is
     *                thrown
     * allowEmptyList -- if false and the list is empty (ie. nothing between opening
     *                   and closing bracket), exception is thrown
     *
     * We need to support parsing of nested lists (as found in the envelope data
     * structure), that's why we deal with QVariant here.
     * */
    QVariantList parseList( const char open, const char close,
            const QByteArray& line, int& start );

    /** @short Read one item from input, store it in a most-appropriate form */
    QVariant getAnything( const QByteArray& line, int& start );

}
}

#endif /* IMAP_LOWLEVELPARSER_H */
