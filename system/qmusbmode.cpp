/*!
 * @file qmusbmode.cpp
 * @brief QmUSBMode

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

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
#include "qmusbmode.h"
#include "qmusbmode_p.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>

#include <QStringList>
#include <mntent.h>
#include <stdio.h>

#if HAVE_USB_MODED_DEV
    #include <usb_moded-dbus.h>
    #include <usb_moded-modes.h>
#else
    /* Use QmSystem D-Bus i/f declarations because usb-moded-dev is not available */
    #include "msystemdbus_p.h"
#endif

#define USB_MODE_GCONF          "/Meego/System/UsbMode"
#define SIGNAL_USB_MODE 0
#define SIGNAL_USB_ERROR 1

namespace MeeGo {

QmUSBMode::QmUSBMode(QObject *parent) : QObject(parent) {
    MEEGO_INITIALIZE(QmUSBMode);

    connect(priv, SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode)), this, SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode)));
    connect(priv, SIGNAL(fileSystemWillUnmount(MeeGo::QmUSBMode::MountPath)), this, SIGNAL(fileSystemWillUnmount(MeeGo::QmUSBMode::MountPath)));
    connect(priv, SIGNAL(error(const QString&)), this, SIGNAL(error(const QString&)));
}

QmUSBMode::~QmUSBMode() {
    MEEGO_PRIVATE(QmUSBMode);

    disconnect(priv, SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode)), this, SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode)));
    disconnect(priv, SIGNAL(fileSystemWillUnmount(MeeGo::QmUSBMode::MountPath)), this, SIGNAL(fileSystemWillUnmount(MeeGo::QmUSBMode::MountPath)));
    disconnect(priv, SIGNAL(error(const QString&)), this, SIGNAL(error(const QString&)));

    MEEGO_UNINITIALIZE(QmUSBMode);
}

void QmUSBMode::connectNotify(const char *signal) {
    MEEGO_PRIVATE(QmUSBMode)

    /* QObject::connect() needs to be thread-safe */
    QMutexLocker locker(&priv->connectMutex);

    if ((QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode))))) ||
        (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(fileSystemWillUnmount(MeeGo::QmUSBMode::MountPath)))))) {
        if (0 == priv->connectCount[SIGNAL_USB_MODE]) {
            QDBusConnection::systemBus().connect(USB_MODE_SERVICE,
                                                 USB_MODE_OBJECT,
                                                 USB_MODE_INTERFACE,
                                                 USB_MODE_SIGNAL_NAME,
                                                 priv,
                                                 SLOT(modeChanged(const QString&)));
        }
        priv->connectCount[SIGNAL_USB_MODE]++;
    } else if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(error(const QString&))))) {
        if (0 == priv->connectCount[SIGNAL_USB_ERROR]) {
            QDBusConnection::systemBus().connect(USB_MODE_SERVICE,
                                                 USB_MODE_OBJECT,
                                                 USB_MODE_INTERFACE,
                                                 USB_MODE_ERROR_SIGNAL_NAME,
                                                 priv,
                                                 SLOT(didReceiveError(const QString&)));
        }
        priv->connectCount[SIGNAL_USB_ERROR]++;
    }
}

void QmUSBMode::disconnectNotify(const char *signal) {
    MEEGO_PRIVATE(QmUSBMode)

    /* QObject::disconnect() needs to be thread-safe */
    QMutexLocker locker(&priv->connectMutex);

    if ((QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode))))) ||
        (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(fileSystemWillUnmount(MeeGo::QmUSBMode::MountPath)))))) {
        priv->connectCount[SIGNAL_USB_MODE]--;

        if (0 == priv->connectCount[SIGNAL_USB_MODE]) {
            QDBusConnection::systemBus().disconnect(USB_MODE_SERVICE,
                                                    USB_MODE_OBJECT,
                                                    USB_MODE_INTERFACE,
                                                    USB_MODE_SIGNAL_NAME,
                                                    priv,
                                                    SLOT(modeChanged(const QString&)));
        }
    } else if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(error(const QString&))))) {
        priv->connectCount[SIGNAL_USB_ERROR]--;

        if (0 == priv->connectCount[SIGNAL_USB_ERROR]) {
            QDBusConnection::systemBus().disconnect(USB_MODE_SERVICE,
                                                    USB_MODE_OBJECT,
                                                    USB_MODE_INTERFACE,
                                                    USB_MODE_ERROR_SIGNAL_NAME,
                                                    priv,
                                                    SLOT(didReceiveError(const QString&)));
        }
    }
}

QmUSBMode::Mode QmUSBMode::getMode() {
    MEEGO_PRIVATE(QmUSBMode);

    QDBusReply<QString> usbModeReply = QDBusConnection::systemBus().call(
                                           QDBusMessage::createMethodCall(USB_MODE_SERVICE, USB_MODE_OBJECT, USB_MODE_INTERFACE,
                                                                          USB_MODE_STATE_REQUEST));
    if (usbModeReply.isValid()) {
        return priv->stringToMode(usbModeReply.value());
    }
    return QmUSBMode::Undefined;
}

bool QmUSBMode::setMode(QmUSBMode::Mode mode) {
    MEEGO_PRIVATE(QmUSBMode);

    // The OviSuite, MassStorage and ChargingOnly modes can be requested
    if (!(OviSuite == mode || MassStorage == mode || ChargingOnly == mode)) {
        return false;
    }

    QString usbModeString = priv->modeToString(mode);
    if (usbModeString.isEmpty()) {
        return false;
    }

    QDBusMessage usbModeSetCall = QDBusMessage::createMethodCall(USB_MODE_SERVICE, USB_MODE_OBJECT, USB_MODE_INTERFACE, USB_MODE_STATE_SET);
    usbModeSetCall << usbModeString;

    (void)QDBusConnection::systemBus().call(usbModeSetCall, QDBus::NoBlock);
    return true;
}

bool QmUSBMode::setDefaultMode(QmUSBMode::Mode mode) {
    MEEGO_PRIVATE(QmUSBMode);

    // The OviSuite, MassStorage, ChargingOnly and Ask modes can be requested
    if (!(OviSuite == mode || MassStorage == mode || ChargingOnly == mode || Ask == mode)) {
        return false;
    }

    QString str = priv->modeToString(mode);
    if (str.isEmpty()) {
        return false;
    }
    gboolean ret = gconf_client_set_string(priv->gcClient, USB_MODE_GCONF, str.toAscii().data(), NULL);
    if (ret == TRUE) {
        return true;
    } else {
        return false;
    }
}

QmUSBMode::Mode QmUSBMode::getDefaultMode() {
    MEEGO_PRIVATE(QmUSBMode);

    QmUSBMode::Mode mode = QmUSBMode::Undefined;
    gchar *val = gconf_client_get_string(priv->gcClient, USB_MODE_GCONF, NULL);
    if (val) {
        mode = priv->stringToMode(QString(val));
    }
    g_free(val);
    return mode;
}

QmUSBMode::MountOptionFlags QmUSBMode::mountStatus(QmUSBMode::MountPath mountPath) {
    MEEGO_PRIVATE(QmUSBMode);

    QmUSBMode::MountOptionFlags mountOptions;
    QString mount = "/home/user/MyDocs";
    QStringList mountOpts;

    if (QmUSBMode::DocumentDirectoryMount != mountPath) {
        /* We don't currently support other mount paths */
        goto out;
    }

    mountOpts = priv->mountOptions(priv->mountEntries(), mount).split(",");

    if (mountOpts.contains("rw")) {
        mountOptions |= QmUSBMode::ReadWriteMount;
        goto out;
    }

    if (mountOpts.contains("ro")) {
        mountOptions |= QmUSBMode::ReadOnlyMount;
        goto out;
    }

out:
    return mountOptions;
}

// private class

QmUSBModePrivate::QmUSBModePrivate(QObject *parent) : QObject(parent) {
    connectCount[SIGNAL_USB_MODE] = connectCount[SIGNAL_USB_ERROR] = 0;

    g_type_init();
    gcClient = gconf_client_get_default();
}

QmUSBModePrivate::~QmUSBModePrivate() {
    g_object_unref(gcClient), gcClient = 0;
}

QString QmUSBModePrivate::modeToString(QmUSBMode::Mode mode) {
    switch (mode) {
    case QmUSBMode::Connected:
        return USB_CONNECTED;
    case QmUSBMode::DataInUse:
        return DATA_IN_USE;
    case QmUSBMode::Disconnected:
        return USB_DISCONNECTED;
    case QmUSBMode::MassStorage:
        return MODE_MASS_STORAGE;
    case QmUSBMode::ChargingOnly:
        return MODE_CHARGING;
    case QmUSBMode::OviSuite:
        return MODE_OVI_SUITE;
    case QmUSBMode::Ask:
        return MODE_ASK;
    case QmUSBMode::Undefined:
        return MODE_UNDEFINED;
    case QmUSBMode::ModeRequest:
        return USB_CONNECTED_DIALOG_SHOW;
    default:
        return "";
    }
}

QmUSBMode::Mode QmUSBModePrivate::stringToMode(const QString &str) {
    if (str == USB_CONNECTED) {
        return QmUSBMode::Connected;
    } else if (str == USB_DISCONNECTED) {
        return QmUSBMode::Disconnected;
    } else if (str == DATA_IN_USE) {
        return QmUSBMode::DataInUse;
    } else if (str == MODE_MASS_STORAGE) {
        return QmUSBMode::MassStorage;
    } else if (str == MODE_OVI_SUITE) {
        return QmUSBMode::OviSuite;
    } else if (str == MODE_CHARGING) {
        return QmUSBMode::ChargingOnly;
    } else if (str == MODE_ASK) {
        return QmUSBMode::Ask;
    } else if (str == MODE_UNDEFINED) {
        return QmUSBMode::Undefined;
    } else if (str == USB_CONNECTED_DIALOG_SHOW) {
        return QmUSBMode::ModeRequest;
    } else {
        return QmUSBMode::Undefined;
    }
}

QVector< QPair< QString , QString > > QmUSBModePrivate::mountEntries() {
    QVector< QPair< QString , QString > > entries;
    mntent m;
    char buf[1024];
    FILE *f = 0;

    if (!(f = setmntent("/proc/mounts", "r"))) {
        perror("setmntent /proc/mounts");
        if (!(f = setmntent("/etc/mtab", "r"))) {
            perror("setmntent /etc/mtab");
            goto out;
        }
    }

    while (getmntent_r(f, &m, buf, sizeof(buf)) != 0) {
        entries << qMakePair(QString::fromAscii(m.mnt_dir),
                             QString::fromAscii(m.mnt_opts));
    }

out:
    if (f) {
        endmntent(f);
    }
    return entries;
}

QString QmUSBModePrivate::mountOptions(QVector< QPair< QString , QString > > mountEntries, const QString &fileName) {
    QString mountOptions;
    const size_t max = mountEntries.size();
    for (size_t i=0; i < max; i++) {
        QPair<QString, QString> entry = mountEntries.at(i);
        /* Check if the mnt_dir matches the mount we are looking */
        if (entry.first == fileName) {
            return entry.second;
        }
    }
    return mountOptions;
}

void QmUSBModePrivate::didReceiveError(const QString &errorCode) {
    emit error(errorCode);
}

void QmUSBModePrivate::modeChanged(const QString &mode) {
    if (mode == USB_PRE_UNMOUNT) {
        /* The pre-unmount signal only concerns MyDocs, for now */
        emit fileSystemWillUnmount(QmUSBMode::DocumentDirectoryMount);
    } else {
        emit modeChanged(stringToMode(mode));
    }
}

} // namespace MeeGo
