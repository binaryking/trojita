/* Copyright (C) 2007 - 2010 Jan Kundrát <jkt@flaska.net>

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


#include "NoopTask.h"
#include "KeepMailboxOpenTask.h"
#include "Model.h"
#include "MailboxTree.h"

namespace Imap {
namespace Mailbox {

NoopTask::NoopTask( Model* _model, ImapTask* parentTask ) :
    ImapTask( _model )
{
    conn = parentTask;
    parentTask->addDependentTask( this );
}

void NoopTask::perform()
{
    parser = conn->parser;
    Q_ASSERT( parser );
    model->_parsers[ parser ].activeTasks.append( this );

    // we do not want to use _onlineMessageFetch because it contains UID and FLAGS
    tag = parser->noop();
    model->_parsers[ parser ].commandMap[ tag ] = Model::Task( Model::Task::NOOP, 0 );
    emit model->activityHappening( true );
}

bool NoopTask::handleStateHelper( Imap::Parser* ptr, const Imap::Responses::State* const resp )
{
    if ( resp->tag.isEmpty() )
        return false;

    if ( resp->tag == tag ) {
        IMAP_TASK_ENSURE_VALID_COMMAND( tag, Model::Task::NOOP );

        if ( resp->kind == Responses::OK ) {
            // nothing should be needed here
        } else {
            // FIXME: error handling
        }
        _completed();
        IMAP_TASK_CLEANUP_COMMAND;
        return true;
    } else {
        return false;
    }
}

}
}