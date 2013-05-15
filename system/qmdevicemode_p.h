/*!
* @file qmdevicemode_p.h
* @brief Contains QmDeviceModePrivate

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

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
#ifndef QMDEVICEMODE_P_H
#define QMDEVICEMODE_P_H

#include "qmdevicemode.h"
#include "qmipcinterface_p.h"

#include <QMutex>

#if HAVE_MCE
    #include "mce/dbus-names.h"
    #include "mce/mode-names.h"
#endif

#include <gconf/gconf-client.h>
#define PATH "/system/osso/dsm/energymanagement"
#define FORCE_POWER_SAVING PATH"/force_power_saving"
#define ENABLE_POWER_SAVING PATH"/enable_power_saving"
#define THRESHOLDS PATH"/possible_psm_thresholds"
#define THRESHOLD PATH"/psm_threshold"

#define SIGNAL_DEVICE_MODE 0
#define SIGNAL_PSM_MODE 1

namespace MeeGo
{
    class QmDeviceModePrivate : public QObject
    {
        Q_OBJECT
        MEEGO_DECLARE_PUBLIC(QmDeviceMode)

    public:
        QmDeviceModePrivate() {
            requestIf = 0;

            #if HAVE_MCE
                requestIf = new QmIPCInterface(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF);
            #endif

            connectCount[SIGNAL_DEVICE_MODE] = connectCount[SIGNAL_PSM_MODE] = 0;

            g_type_init();
            gcClient = gconf_client_get_default();
        }

        ~QmDeviceModePrivate() {
            #if HAVE_MCE
                if (requestIf)
                    delete requestIf, requestIf = 0;
            #endif
            g_object_unref(gcClient), gcClient = 0;
        }

        static QmDeviceMode::DeviceMode radioStateToDeviceMode(quint32 radioStateFlags) {
            QmDeviceMode::DeviceMode deviceMode = QmDeviceMode::Error;
            #if HAVE_MCE
                if (radioStateFlags & ~(MCE_RADIO_STATE_WLAN | MCE_RADIO_STATE_BLUETOOTH)) {
                    deviceMode = QmDeviceMode::Normal;
                } else {
                    deviceMode = QmDeviceMode::Flight;
                }
            #else
                Q_UNUSED(radioStateFlags);
            #endif
            return deviceMode;
        }

        static QmDeviceMode::PSMState psmStateToModeEnum(bool on) {
            if (on) {
                return QmDeviceMode::PSMStateOn;
            } else {
                return QmDeviceMode::PSMStateOff;
            }
        }

        QMutex connectMutex;
        size_t connectCount[2];
        QmIPCInterface *requestIf;
        GConfClient *gcClient;

    Q_SIGNALS:

        void devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState);
        void deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode);

    private Q_SLOTS:

        void devicePSMChangedSlot(bool on) {
            emit devicePSMStateChanged(psmStateToModeEnum(on));
        }

        void deviceModeChangedSlot(const quint32 state) {
            emit deviceModeChanged(radioStateToDeviceMode(state));
        }
    };
}

#endif // QMDEVICEMODE_P_H
