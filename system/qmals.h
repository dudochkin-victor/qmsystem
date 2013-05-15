/*!
 * @file qmals.h
 * @brief Contains QmALS, which provides internal ambient light sensor measurements.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

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
 */

#ifndef QMALS_H
#define QMALS_H
#include "system_global.h"
#include <QtCore/qobject.h>
#include "qmsensor.h"

QT_BEGIN_HEADER

namespace MeeGo {

    typedef QmIntReading QmAlsReading;

    /**
     *
     * @scope Internal
     *
     * @brief Provides internal ambient light sensor measurements.
     *
     * The unit of the value returned is lux.
     * 
     * To get measurements and change sensor settings, the client must
     * open a session (and call start() for data). Details can be found
     * from documentation of #QmSensor.
     */
    class MEEGO_SYSTEM_EXPORT QmALS : public QmSensor
    {
        Q_OBJECT;
        Q_PROPERTY(QmAlsReading value READ get);

    public:
        /**
         * Constructor
         * @param parent Parent QObject
         */
        QmALS(QObject *parent = 0);

        /**
         * Destructor
         */
        ~QmALS();

        /**
         * Gets the current ambient light intensity value.
         * @return Current ambient light intensity (lux)
         */
        QmAlsReading get();

    Q_SIGNALS:

        /**
         * Sent when ambient light intensity has changed.
         * @param value Observed ambient light intensity (lux)
         */
        void ALSChanged(const MeeGo::QmAlsReading data);

    };

} // MeeGo namespace

QT_END_HEADER

#endif
