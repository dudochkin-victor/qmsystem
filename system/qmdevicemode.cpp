/*!
 * @file qmdevicemode.cpp
 * @brief QmDeviceMode

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

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
#include "qmdevicemode.h"
#include "qmdevicemode_p.h"

#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMessage>

/*
 * PSM stuff (spec)
 *
 * Signal emitted from the com.nokia.mce.signal interface
 *
 *    Name        powersave_mode_ind
 *   Parameters  dbus_bool_t  mode TRUE (=on)/ FALSE(=off)
 *   Description Sent when the powersave mode is changed
 *
 *
 * Generic method calls provided by the com.nokia.mce.request interface
 *
 *   Name                   get_powersave_mode
 *   Parameters             -
 *   Errors / Return value  dbus_bool_t inactivity TRUE / FALSE
 *   Description            Get the current powersave mode. TRUE if the powersave mode is on,  FALSE if the powersave mode is off.
 *
 *
 *   Name                   set_powersave_mode
 *   Parameters             dbus_bool_t  mode TRUE (=on)/ FALSE(=off)
 *   Errors / Return value  dbus_bool_t  status TRUE=success / FALSE=failure
 *   Description            Set the current powersave mode. TRUE if the powersave mode is on,  FALSE if the powersave mode is off.
 *
 *
 * We also need to set the automatic PSM, GConf would be good as this is persistent setting (see qmdisplaystate.cpp)
 *
 */

namespace MeeGo
{
    QmDeviceMode::QmDeviceMode(QObject *parent) : QObject(parent) {
        MEEGO_INITIALIZE(QmDeviceMode)

        connect(priv, SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState)), this,
                SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState)));
        connect(priv, SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode)), this,
                SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode)));
    }

    QmDeviceMode::~QmDeviceMode() {
        MEEGO_PRIVATE(QmDeviceMode)

        disconnect(priv, SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState)), this,
                   SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState)));
        disconnect(priv, SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode)), this,
                   SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode)));

        MEEGO_UNINITIALIZE(QmDeviceMode);
    }

    void QmDeviceMode::connectNotify(const char *signal) {
        MEEGO_PRIVATE(QmDeviceMode)

        /* QObject::connect() needs to be thread-safe */
        QMutexLocker locker(&priv->connectMutex);

        if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode))))) {
            if (0 == priv->connectCount[SIGNAL_DEVICE_MODE]) {
                #if HAVE_MCE
                    QDBusConnection::systemBus().connect(MCE_SERVICE,
                                                         MCE_SIGNAL_PATH,
                                                         MCE_SIGNAL_IF,
                                                         MCE_RADIO_STATES_SIG,
                                                         priv,
                                                         SLOT(deviceModeChangedSlot(const quint32)));
                #endif
            }
            priv->connectCount[SIGNAL_DEVICE_MODE]++;
        } else if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState))))) {
            if (0 == priv->connectCount[SIGNAL_PSM_MODE]) {
                #if HAVE_MCE
                    QDBusConnection::systemBus().connect(MCE_SERVICE,
                                                         MCE_SIGNAL_PATH,
                                                         MCE_SIGNAL_IF,
                                                         MCE_PSM_STATE_SIG,
                                                         priv,
                                                         SLOT(devicePSMChangedSlot(bool)));
                #endif
            }
            priv->connectCount[SIGNAL_PSM_MODE]++;
        }
    }

    void QmDeviceMode::disconnectNotify(const char *signal) {
        MEEGO_PRIVATE(QmDeviceMode)

        /* QObject::disconnect() needs to be thread-safe */
        QMutexLocker locker(&priv->connectMutex);

        if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode))))) {
            priv->connectCount[SIGNAL_DEVICE_MODE]--;

            if (0 == priv->connectCount[SIGNAL_DEVICE_MODE]) {
                #if HAVE_MCE
                    QDBusConnection::systemBus().disconnect(MCE_SERVICE,
                                                            MCE_SIGNAL_PATH,
                                                            MCE_SIGNAL_IF,
                                                            MCE_RADIO_STATES_SIG,
                                                            priv,
                                                            SLOT(deviceModeChangedSlot(const quint32)));
                #endif
            }
        } else if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState))))) {
            priv->connectCount[SIGNAL_PSM_MODE]--;

            if (0 == priv->connectCount[SIGNAL_PSM_MODE]) {
                #if HAVE_MCE
                    QDBusConnection::systemBus().disconnect(MCE_SERVICE,
                                                            MCE_SIGNAL_PATH,
                                                            MCE_SIGNAL_IF,
                                                            MCE_PSM_STATE_SIG,
                                                            priv,
                                                            SLOT(devicePSMChangedSlot(bool)));
                #endif
            }
        }
    }

    QmDeviceMode::DeviceMode QmDeviceMode::getMode() const {
        QmDeviceMode::DeviceMode deviceMode = Error;
        #if HAVE_MCE
            MEEGO_PRIVATE_CONST(QmDeviceMode)

            QDBusReply<quint32> radioStatesReply = QDBusConnection::systemBus().call(
                                                       QDBusMessage::createMethodCall(MCE_SERVICE, MCE_REQUEST_PATH,
                                                                                      MCE_REQUEST_IF, MCE_RADIO_STATES_GET));
            if (radioStatesReply.isValid()) {
                deviceMode = priv->radioStateToDeviceMode(radioStatesReply.value());
            }
        #endif
        return deviceMode;
    }

    QmDeviceMode::PSMState QmDeviceMode::getPSMState() const {
        QmDeviceMode::PSMState psmState = PSMError;
        #if HAVE_MCE
            MEEGO_PRIVATE_CONST(QmDeviceMode)

            QDBusReply<bool> psmModeReply = QDBusConnection::systemBus().call(
                                                QDBusMessage::createMethodCall(MCE_SERVICE, MCE_REQUEST_PATH,
                                                                               MCE_REQUEST_IF, MCE_PSM_STATE_GET));
            if (psmModeReply.isValid()) {
                psmState = priv->psmStateToModeEnum(psmModeReply.value());
            }
        #endif
        return psmState;
    }

    bool QmDeviceMode::setMode(QmDeviceMode::DeviceMode mode) {
        #if HAVE_MCE
            MEEGO_PRIVATE(QmDeviceMode)

            quint32 state, mask;

            switch (mode) {
            case Normal:
                state = 1;
                mask = MCE_RADIO_STATE_MASTER;
                break;
            case Flight:
                state = 0;
                mask = MCE_RADIO_STATE_MASTER;
                break;
            default:
                return false;
            }

            priv->requestIf->callAsynchronously(MCE_RADIO_STATES_CHANGE_REQ, state, mask);
            return true;
        #else
            Q_UNUSED(mode);
        #endif
        return false;
    }

    bool QmDeviceMode::setPSMState(QmDeviceMode::PSMState state) {
        MEEGO_PRIVATE(QmDeviceMode)

        gboolean val = FALSE;
        if (state == PSMStateOff) {
            val = FALSE;
        } else if (state == PSMStateOn) {
            val = TRUE;
        } else {
            return false;
        }

        gboolean ret = gconf_client_set_bool(priv->gcClient, FORCE_POWER_SAVING, val, NULL);
        if (ret == TRUE) {
            return true;
        } else {
            return false;
        }
    }

    bool QmDeviceMode::setPSMBatteryMode(int percentages) {
        MEEGO_PRIVATE(QmDeviceMode)

        if (percentages < 0 || percentages > 100) {
            return false;
        }

        int value = 0;
        if (percentages > 0) {
            GSList *list = gconf_client_get_list(priv->gcClient, THRESHOLDS, GCONF_VALUE_INT, NULL);
            if (!list) {
                return false;
            }
            GSList *elem = list;
            do {
                int data = GPOINTER_TO_INT(elem->data);
                if (percentages <= data || !elem->next) {
                    value = data;
                    break;
                }
            } while ((elem = g_slist_next(elem)));
            g_slist_free(list);
        }

        gboolean ret = FALSE;
        if (value == 0) {
            ret = gconf_client_set_bool(priv->gcClient, ENABLE_POWER_SAVING, FALSE, NULL);
        } else {
            ret = gconf_client_set_bool(priv->gcClient, ENABLE_POWER_SAVING, TRUE, NULL);
            if (ret == TRUE) {
                ret = gconf_client_set_int(priv->gcClient, THRESHOLD, value, NULL);
            }
        }

        if (ret == TRUE) {
            return true;
        } else {
            return false;
        }
    }

    int QmDeviceMode::getPSMBatteryMode() {
        MEEGO_PRIVATE(QmDeviceMode)

        GError *error = NULL;
        gboolean ret = gconf_client_get_bool(priv->gcClient, ENABLE_POWER_SAVING, &error);
        if (error) {
            g_error_free(error);
            return -1;
        }
        if (ret == FALSE) {
            return 0;
        }
        int retVal = gconf_client_get_int(priv->gcClient, THRESHOLD, &error);
        if (error) {
            g_error_free(error);
            return -1;
        }
        return retVal;
    }

} // namespace MeeGo
