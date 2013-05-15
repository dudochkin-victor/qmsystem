/*!
 * @file qmcallstate_p.h
 * @brief Contains QmCallStatePrivate

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>

   @scope Private

   This file is part of SystemSW QtAPI.

   SystemSW QtAPI is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   SystemSW QtAPI is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with SystemSW QtAPI.  If not, see <http://www.gnu.org/licenses/>.
   </p>
*/

#ifndef QMCALLSTATE_P_H
#define QMCALLSTATE_P_H

#include "qmcallstate.h"
#include "qmipcinterface_p.h"

#include <QMutex>

#if HAVE_MCE
    #include "mce/dbus-names.h"
    #include "mce/mode-names.h"
#endif

#define SIGNAL_CALL_STATE 0

namespace MeeGo
{
    class QmCallStatePrivate : public QObject {
        Q_OBJECT
        MEEGO_DECLARE_PUBLIC(QmCallState)

    public:
        QmCallStatePrivate() {
            #if HAVE_MCE
                requestIf = new QmIPCInterface(MCE_SERVICE,
                                               MCE_REQUEST_PATH,
                                               MCE_REQUEST_IF);
            #endif
            connectCount[SIGNAL_CALL_STATE] = 0;
        }

        ~QmCallStatePrivate() {
            #if HAVE_MCE
                delete requestIf, requestIf = 0;
            #endif
        }

        QmIPCInterface *requestIf;
        QMutex connectMutex;
        size_t connectCount[1];

    Q_SIGNALS:
        void stateChanged(MeeGo::QmCallState::State state, MeeGo::QmCallState::Type type);

    public Q_SLOTS:
        void callStateChanged(const QString& state, const QString& type) {
            #if HAVE_MCE
                QmCallState::State mState = QmCallState::Error;
                QmCallState::Type mType = QmCallState::Unknown;

                if (state == MCE_CALL_STATE_ACTIVE)
                    mState = QmCallState::Active;
                else if (state == MCE_CALL_STATE_SERVICE)
                    mState = QmCallState::Service;
                else if (state == MCE_CALL_STATE_NONE)
                    mState = QmCallState::None;

                if (type == MCE_NORMAL_CALL)
                    mType = QmCallState::Normal;
                else if (type == MCE_EMERGENCY_CALL)
                    mType = QmCallState::Emergency;

                emit stateChanged(mState, mType);
            #else
                Q_UNUSED(state);
                Q_UNUSED(type);
            #endif
        }
    };
}
#endif // QMCALLSTATE_P_H
