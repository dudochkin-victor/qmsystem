/*!
 * @file qmled.h
 * @brief Contains QmLed which provides information and actions on device LED.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
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
#ifndef QMLED_H
#define QMLED_H
#include "system_global.h"
#include <QtCore/qobject.h>

QT_BEGIN_HEADER

namespace MeeGo{

class QmLEDPrivate;

/**
 * @scope Internal
 *
 * @brief Provides information and actions on device LED.
 */
class QmLED : public QObject
{
    Q_OBJECT

public:
    QmLED(QObject *parent = 0);
    ~QmLED();

    /**
     * Activates the pattern in device LED.
     * @credential mce::LEDControl Resource token required to activate the pattern.
     * @param pattern Pattern to activate
     * @return False if LED functionality is not available or cannot be set
     * @todo Add description of strings for patterns.
     */
    bool activate(const QString &pattern);

    /**
     * Deactivates the pattern in device LED.
     * @credential mce::LEDControl Resource token required to deactivate the pattern.
     * @param pattern Pattern to deactivate
     * @return False if LED functionality is not available or cannot be set
     * @todo Add description of string for patterns.
     */
    bool deactivate(const QString &pattern);

    /**
     * Enables the device LED.
     * @credential mce::LEDControl Resource token required to enable LED.
     * @return False if LED functionality is not available or cannot be set
     */
    bool enable(void);

    /**
     * Disables the device LED.
     * @credential mce::LEDControl Resource token required to disable LED.
     * @return False if LED functionality is not available or cannot be set
     */
    bool disable(void);

private:
    Q_DISABLE_COPY(QmLED)
    MEEGO_DECLARE_PRIVATE(QmLED)
};

} //namespace MeeGo

QT_END_HEADER

#endif /*QMLED_H*/

