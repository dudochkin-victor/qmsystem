/*!
 * @file qmsysteminformation.cpp
 * @brief QmSystemInformation

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

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
#include "qmsysteminformation.h"
#include "qmsysteminformation_p.h"

#include <stdint.h>
#include <stdlib.h>

#include <QFile>
#include <QTextStream>
#include <QStringList>

namespace MeeGo
{

QmSystemInformationPrivate::QmSystemInformationPrivate()
{
    #if HAVE_SYSINFO
    systemConfig = 0;
    if (sysinfo_init(&systemConfig) != 0) {
        /* Failed to initialize the system configuration object */
        if (systemConfig) {
            sysinfo_finish(systemConfig), systemConfig = 0;
        }
    }
    #endif /* HAVE_SYSINFO */
}

QmSystemInformationPrivate::~QmSystemInformationPrivate()
{
    #if HAVE_SYSINFO
    if (systemConfig) {
        sysinfo_finish(systemConfig), systemConfig = 0;
    }
    #endif /* HAVE_SYSINFO */
}

QString QmSystemInformationPrivate::valueForKey(const QString &key) const
{
    QString value("");
    uint8_t *data = 0;
    #if HAVE_SYSINFO
    unsigned long size = 0;

    if (!systemConfig) {
        goto EXIT;
    }

    if (sysinfo_get_value(systemConfig, key.toStdString().c_str(), &data, &size) != 0) {
        /* Failed to read the key from the system configuration */
        goto EXIT;
    }

    for (unsigned long k=0; k < size; k++) {
        /* Values can contain non-ascii data -> escape those */
        int c = data[k];
        if (c < 32 || c > 126)
            continue;
        value += (char)c;
    }
    #endif /* HAVE_SYSINFO */
    EXIT:
    /* Free allocation done by sysinfo_get_value() */
    if (data) {
        free(data), data = 0;
    }
    return value;
}

QmSystemInformation::QmSystemInformation()
{
     MEEGO_INITIALIZE(QmSystemInformation);
}

QmSystemInformation::~QmSystemInformation()
{
    MEEGO_UNINITIALIZE(QmSystemInformation);
}

QString QmSystemInformation::valueForKey(const QString &key) const
{
    MEEGO_PRIVATE_CONST(QmSystemInformation)
    return priv->valueForKey(key);
}

QString QmSystemInformation::kernelCommandLineValueForKey(const QString &key)
{
    QString value("");
    QFile cmdlineFile("/proc/cmdline");

    if (!cmdlineFile.open(QFile::ReadOnly | QFile::Text)) {
        return value;
    }

    QTextStream in(&cmdlineFile);
    QString keyAndValue;
    foreach (keyAndValue, in.readAll().split(" ")) {
        QStringList kernelArgument = keyAndValue.split("=");
        if (kernelArgument.at(0) == key) {
            value = kernelArgument.at(1);
            break;
        }
    }
    return value;
}


} //MeeGo namespace
