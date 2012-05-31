/* Copyright (C) 2007 - 2011 Jan Kundrát <jkt@flaska.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or version 3 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#include "CopyMoveMessagesTask.h"
#include "ExpungeMessagesTask.h"
#include "ItemRoles.h"
#include "KeepMailboxOpenTask.h"
#include "UpdateFlagsTask.h"
#include "Model.h"
#include "MailboxTree.h"

namespace Imap
{
namespace Mailbox
{


CopyMoveMessagesTask::CopyMoveMessagesTask(Model *model, const QModelIndexList &messages_, const QString &targetMailbox,
                                           const CopyMoveOperation op):
    ImapTask(model), targetMailbox(targetMailbox), shouldDelete(op == MOVE)
{
    if (messages_.isEmpty()) {
        throw CantHappen("CopyMoveMessagesTask called with empty message set");
    }
    Q_FOREACH(const QModelIndex& index, messages_) {
        messages << index;
    }
    QModelIndex mailboxIndex = model->findMailboxForItems(messages_);
    conn = model->findTaskResponsibleFor(mailboxIndex);
    conn->addDependentTask(this);
}

void CopyMoveMessagesTask::perform()
{
    parser = conn->parser;
    markAsActiveTask();

    IMAP_TASK_CHECK_ABORT_DIE;

    Sequence seq;
    bool first = true;

    Q_FOREACH(const QPersistentModelIndex& index, messages) {
        if (! index.isValid()) {
            // FIXME: add proper fix
            log("Some message got removed before we could copy them");
        } else {
            TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
            Q_ASSERT(item);
            TreeItemMessage *message = dynamic_cast<TreeItemMessage *>(item);
            Q_ASSERT(message);
            if (first) {
                seq = Sequence(message->uid());
                first = false;
            } else {
                seq.add(message->uid());
            }
        }
    }

    if (first) {
        // No valid messages
        _failed("All messages disappeared before we could have copied them");
        return;
    }

    if (shouldDelete && model->accessParser(parser).capabilities.contains(QLatin1String("XMOVE"))) {
        moveTag = parser->uidXMove(seq, targetMailbox);
    } else {
        copyTag = parser->uidCopy(seq, targetMailbox);
    }
}

bool CopyMoveMessagesTask::handleStateHelper(const Imap::Responses::State *const resp)
{
    if (resp->tag.isEmpty())
        return false;

    if (resp->tag == copyTag) {
        if (resp->kind == Responses::OK) {
            if (shouldDelete) {
                if (_dead) {
                    // Yeah, that's bad -- the COPY has succeeded, yet we cannot update the flags :(
                    log("COPY succeeded, but cannot update flags due to received die()");
                    _failed("Asked to die");
                    return true;
                }
                // We ignore the _aborted status here, though -- we just want to finish in an "atomic" manner
                ImapTask *flagTask = new UpdateFlagsTask(model, this, messages, FLAG_ADD_SILENT, QLatin1String("\\Deleted"));
                if (model->accessParser(parser).capabilities.contains(QLatin1String("UIDPLUS"))) {
                    new ExpungeMessagesTask(model, flagTask, messages);
                }
            }
            _completed();
        } else {
            _failed("The COPY operation has failed");
        }
        return true;
    } else if (resp->tag == moveTag) {
        if (resp->kind == Responses::OK) {
            _completed();
        } else {
            _failed("The UID MOVE operation has failed");
        }
        return true;
    } else {
        return false;
    }
}

QVariant CopyMoveMessagesTask::taskData(const int role) const
{
    return role == RoleTaskCompactName ? QVariant(tr("Copying messages")) : QVariant();
}

}
}
