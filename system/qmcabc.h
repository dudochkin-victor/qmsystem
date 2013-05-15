/**
 * @file qmcabc.h
 * @brief Contains QmCABC which provides methods to indicate Content Adaptive Backlight Control (CABC) mode.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Rongas <ext-timo.rongas@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

   @scope Nokia Meego

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

#ifndef __QMCABC_H__
#define __QMCABC_H__

#include <QObject>
#include "system_global.h"

QT_BEGIN_HEADER

namespace MeeGo
{

/**
 *
 * @scope Nokia Meego
 *
 * @brief Provides methods to indicate the Content Adaptive Backlight Control (CABC) mode.
 */
class MEEGO_SYSTEM_EXPORT QmCABC : public QObject
{
    Q_OBJECT
    Q_ENUMS(Mode)

public:
    enum Mode
    {
        Off=0,          /**< CABC is not activated */
        Ui=1,           /**< Best quality. */
        StillImage=2,   /**< Medium quality, suitable for images */
        MovingImage=3   /**< Suitable for video */
    };

    QmCABC(QObject *parent = 0) : QObject(parent) {};

    /**
     * Gets the current CABC mode.
     * @return Mode CABC mode.
     */
    Mode get() const;

    /**
     * Requests to set the CABC mode. The system does not guarantee that
     * the CABC mode will be set according to the request. The applications
     * should not depend on a particular CABC mode but can use this method
     * to indicate that a certain CABC mode would be desired.
     *
     * @param mode CABC mode
     * @return bool TRUE if a valid mode was requested, FALSE otherwise
     */
    bool set(Mode mode);
};

} // namespace MeeGo

QT_END_HEADER

#endif
