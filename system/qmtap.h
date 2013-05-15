/*!
 * @file qmtap.h
 * @brief Contains QmTap, which provides device tapping events.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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
#ifndef QMTAP_H
#define QMTAP_H

#include <QtCore/qobject.h>
#include <qmsensor.h>


QT_BEGIN_HEADER

namespace MeeGo {

    class QmTapReading;

    /**
     * @scope Internal
     *
     * @brief Provides device tapping events.
     *
     * Provides the device tapping events. Taps can either be single or double taps.
     * See #TapData::Direction and #TapData::Type for description of different
     * tap events.
     *
     * Following image displays the device side naming.
     * @verbatim
      Z
      ^
      |
      |
      |
      |                    Top
      |            Y   ----------
      |           ^   /  NOKIA  /|
      |          /   /-------- / |
      |         /   //       //  /
      |  Left  /   //       //  /
      |       /   // Face  //  /
      |      /   //       //  /   Right
      |     /   //       //  /
      |    /   /---------/  /
      |   /   /    O    /  /
      |  /   /         /  /
      | /    ----------  /
      |/     |_________!/
      -------------------------->X

             Bottom
       @endverbatim
     *
     * To get measurements and change sensor settings, the client must
     * open a session (and call start() for data). Details can be found
     * from documentation of #QmSensor.
     */

    class MEEGO_SYSTEM_EXPORT QmTap : public QmSensor
    {
        Q_OBJECT;

    public:
        enum Direction {
            X = 0,                 /**< Left or right side tapped */
            Y,                 /**< Top or down side tapped */
            Z,                 /**< Face or bottom tapped */
            LeftRight, /**< Tapped from left to right */
            RightLeft, /**< Tapped from right to left */
            TopBottom, /**< Tapped from top to bottom */
            BottomTop, /**< Tapped from bottom to top */
            FaceBack,  /**< Tapped from face to back */
            BackFace   /**< Tapped from back to face */
        };

        enum Type {
            DoubleTap = 0, /**< Double Tap */
            SingleTap  /**< Single Tap */
        };

        /**
         * Constructor
         * @param parent Parent QObject
         */
        QmTap(QObject *parent = 0);
        /**
         * Destructor
         */
        ~QmTap();

    Q_SIGNALS:
        /**
         * Sent when a tapping event has occurred.
         * @param direction Direction of the tap
         * @param type Type of the tap
         */
        void tapped(const MeeGo::QmTapReading);

    };

    /**
     * Device tap event
     */
    class QmTapReading : public QmSensorReading
    {
    public:
        QmTap::Direction direction;
        QmTap::Type type;
    };

} // MeeGo namespace

QT_END_HEADER

#endif
