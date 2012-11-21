/* Copyright (C) 2006 - 2012 Jan Kundrát <jkt@flaska.net>

   This file is part of the Trojita Qt IMAP e-mail client,
   http://trojita.flaska.net/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef IMAP_GENURLAUTHTASK_H
#define IMAP_GENURLAUTHTASK_H

#include "ImapTask.h"
#include "Imap/Model/CatenateData.h"

namespace Imap
{
namespace Mailbox
{

/** @short Obtain an URLAUTH-copatible token for accessing a specified message */
class GenUrlAuthTask : public ImapTask
{
    Q_OBJECT
public:
    GenUrlAuthTask(Model *model, const QString &host, const QString &user, const QString &mailbox, const uint uidValidity,
                   const uint uid, const QString &part, const QString &access);
    virtual void perform();

    virtual bool handleStateHelper(const Imap::Responses::State *const resp);
    virtual bool handleGenUrlAuth(const Responses::GenUrlAuth *const resp);
    virtual bool needsMailbox() const {return false;}
    virtual QVariant taskData(const int role) const;

signals:
    /** @short The GENURLAUTH succeeded, returning a usable URL */
    void gotAuth(const QString & url);

private:
    ImapTask *conn;
    CommandHandle tag;
    QString req;
};

}
}

#endif // IMAP_GENURLAUTHTASK_H
