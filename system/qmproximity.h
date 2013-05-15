/*!
 * @file qmproximity.h
 * @brief Contains QmProximity, which provides proximity sensor measurements.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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
 */

#ifndef QMPROXIMITY_H
#define QMPROXIMITY_H

#include <QtCore/qobject.h>
#include <qmsensor.h>

QT_BEGIN_HEADER

namespace MeeGo {

    typedef QmIntReading QmProximityReading;

    /**
     * @scope Internal
     *
     * @brief Provides proximity sensor measurements.
     *
     * To get measurements and change sensor settings, the client must
     * open a session (and call start() for data). Details can be found
     * from documentation of #QmSensor.
     *
     *
     */
    class MEEGO_SYSTEM_EXPORT QmProximity : public QmSensor
    {
        Q_OBJECT;
        Q_PROPERTY(QmProximityReading proximity READ get);

    public:
        /**
         * Constructor
         * @param parent Parent QObject.
         */
        QmProximity(QObject *parent = 0);

        /**
         * Destructor
         */
        ~QmProximity();

        /**
         * Gets the current proximity value.
         * @return Current proximity value
         */
        QmProximityReading get();

    Q_SIGNALS:

        /**
         * Sent when the value of proximity sensor has changed.
         * @param value Observed proximity value. \c 0: sensor not covered, \c 1: sensor covered
         *
         */
        void ProximityChanged(const MeeGo::QmProximityReading value);

    };

} // MeeGo namespace

QT_END_HEADER

#endif
