/*!
* @file qmdevicemode.h
* @brief Contains QmDeviceMode which provides information and actions on device operation mode and powersave mode

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>
   @author Yang Yang <ext-yang.25.yang@nokia.com>

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

#ifndef QMDEVICEMODE_H
#define QMDEVICEMODE_H
#include <QtCore/qobject.h>
#include "system_global.h"

QT_BEGIN_HEADER

namespace MeeGo {

class QmDeviceModePrivate;

/*!
 * @scope Nokia Meego
 *
 * @class QmDeviceMode
 * @brief QmDeviceMode Provides information and actions on device operation mode and power save mode
 */
class QmDeviceMode : public QObject
{
    Q_OBJECT
    Q_ENUMS(DeviceMode)
    Q_ENUMS(PSMState);
    Q_PROPERTY(DeviceMode mode READ getMode WRITE setMode)
    Q_PROPERTY(PSMState state READ getPSMState WRITE setPSMState)

public:
    //! Possible device operation modes
    enum DeviceMode
    {
        Error = -1,       //!< Operation mode could not be figured out
        Normal = 0,       //!< Normal mode
        Flight            //!< Flight mode
    };

    //! Possible power save modes for the device
    enum PSMState
    {
        PSMError = -1,    //!< Power save mode could not be figured out
        PSMStateOff = 0,  //!< Power save mode is off
        PSMStateOn        //!< Power save mode is on
    };

public:

    /*!
     * @brief Constructor
     * @param parent The parent object
     */
    QmDeviceMode(QObject *parent = 0);
    ~QmDeviceMode();

    /*!
     * @brief Gets the current operation mode.
     * @return The current operation mode
     */
    DeviceMode getMode() const;

    /*!
     * @brief Gets the current power save mode.
     * @return the Current power save mode
     */
    PSMState getPSMState() const;

    /*!
     * @brief Sets the device operation mode.
     * @credential mce::DeviceModeControl Resource token required to set the device (normal/flight) mode.
     * @param state Operation mode to set
     * @return True if a valid mode was requested, false otherwise
     */
    bool setMode(DeviceMode mode);

    /*!
     * @brief Sets the device power save state.
     * @credential mce::DeviceModeControl Resource token required to set the powersave state.
     * @param state Power state mode to set
     * @return True on success, false otherwise
     */
    bool setPSMState(PSMState state);



    /*!
     * @brief Sets the device battery-level based power save mode.
     * @credential mce::DeviceModeControl Resource token required to set the powersave automode.
     * @param percentages The battery charging level (0-100) in percentages after powersave mode is automatically enabled. Value 0 means never.
     * @return True on success, false otherwise
     */
    bool setPSMBatteryMode(int percentages);



    /*!
     * @brief Gets the device battery-level based power save mode.
     * @return The battery charging level, -1 is error, 0 is 'never', 1-100 is the charging level (percentages)
     */
    int getPSMBatteryMode();

Q_SIGNALS:
    /*!
     * @brief Sent when device operation mode has changed.
     * @param mode Current operation mode
     */
    void deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode mode);

    /*!
     * @brief Sent when device power save mode has changed.
     * @param state Current power save mode
     */
    void devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState state);

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private:
    Q_DISABLE_COPY(QmDeviceMode)
    MEEGO_DECLARE_PRIVATE(QmDeviceMode)
};

} // namespace MeeGo

QT_END_HEADER

#endif /* QMDEVICEMODE_H */

// End of file
