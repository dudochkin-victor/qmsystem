/*!
 * @file qmcallstate.h
 * @brief Contains QmCallState which provides information and actions to call state and  type.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>

   @scope Internal

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
 *
 */
#ifndef QMCALLSTATE_H
#define QMCALLSTATE_H

#include <QtCore/qobject.h>
#include "system_global.h"

QT_BEGIN_HEADER

namespace MeeGo {

class QmCallStatePrivate;

/**
 *
 * @scope Internal
 *
 * @brief Provides information and actions to call state and type.
 */
class QmCallState : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_ENUMS(Type)
    Q_PROPERTY(State state READ getState)
    Q_PROPERTY(Type type READ getType)

public:

    /** Possible call states */
    enum State
    {
        Error = -1,  /**< State could not be figured out. */
        None = 0,    /**< No call */
        Active,      /**< Active call */
        Service      /**< Service */
    };

    /** Possible call types */
    enum Type
    {
        Normal = 0,  /**< Normal call */
        Emergency,   /**< Emergency call */
        Unknown      /**< State is not Active or Service */
    };

public:
    QmCallState(QObject *parent = 0);
    ~QmCallState();

    /**
     * Gets the current call state.
     * @return Current call state
     */
    QmCallState::State getState() const;

    /**
     * Gets the current call type.
     * @return Current call type
     */
    QmCallState::Type getType() const;

    /**
     * Sets the current call state and type.
     * @credential mce::CallStateControl Resource token required to set the call state.
     * @param state State to set
     * @param type  Type to set
     * @return True if a valid mode was requested, false otherwise
     */
    bool setState(QmCallState::State state, QmCallState::Type type);

Q_SIGNALS:
    /**
     * Sent when call state has changed.
     * @param state Current call state
     * @param type  Current call type
     */
    void stateChanged(MeeGo::QmCallState::State state, MeeGo::QmCallState::Type type);

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private:
    Q_DISABLE_COPY(QmCallState)
    MEEGO_DECLARE_PRIVATE(QmCallState)
};

} // namespace MeeGo

QT_END_HEADER

#endif // QMCALLSTATE_H
