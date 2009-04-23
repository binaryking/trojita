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

#include <QProcess>
#include <QSslSocket>
#include "IODeviceSocket.h"
#include "Imap/Exceptions.h"

namespace Imap {

IODeviceSocket::IODeviceSocket( QIODevice* device ): d(device)
{
    connect( d, SIGNAL(readyRead()), this, SIGNAL(readyRead()) );
    connect( d, SIGNAL(readChannelFinished()), this, SIGNAL(readChannelFinished()) );
}

IODeviceSocket::~IODeviceSocket()
{
    if ( QProcess* proc = qobject_cast<QProcess*>( d ) ) {
        // Be nice to it, let it die peacefully before using an axe
        proc->terminate();
        proc->waitForFinished(200);
        proc->kill();
    }

    delete d;
}

bool IODeviceSocket::canReadLine()
{
    return d->canReadLine();
}

QByteArray IODeviceSocket::read( qint64 maxSize )
{
    return d->read( maxSize );
}

QByteArray IODeviceSocket::readLine( qint64 maxSize )
{
    return d->readLine( maxSize );
}

bool IODeviceSocket::waitForReadyRead( int msec )
{
    return d->waitForReadyRead( msec );
}

bool IODeviceSocket::waitForBytesWritten( int msec )
{
    return d->waitForBytesWritten( msec );
}

qint64 IODeviceSocket::write( const QByteArray& byteArray )
{
    return d->write( byteArray );
}

void IODeviceSocket::startTls()
{
    QSslSocket* sock = qobject_cast<QSslSocket*>( device() );
    if ( ! sock ) {
        throw InvalidArgument( "This IODeviceSocket is not a QSslSocket, and therefore doesn't support STARTTLS." );
    } else {
        sock->startClientEncryption();
        sock->waitForEncrypted();
    }
}

QIODevice* IODeviceSocket::device() const
{
    return d;
}

}

#include "IODeviceSocket.moc"
