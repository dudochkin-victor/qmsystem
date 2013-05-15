/*!
 * @file qmsysteminformation_p.h
 * @brief QmSystemInformation private header

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
#ifndef QMSYSTEMINFORMATION_P_H
#define QMSYSTEMINFORMATION_P_H

#include <QObject>
#include "system_global.h"
#include "qmsysteminformation.h"

#if HAVE_SYSINFO
    #include <sysinfo.h>
#endif

namespace MeeGo
{

class QmSystemInformationPrivate : public QObject
{
    Q_OBJECT;
    MEEGO_DECLARE_PUBLIC(QmSystemInformation)

public:
    QmSystemInformationPrivate();
    ~QmSystemInformationPrivate();
    QString valueForKey(const QString &key) const;

#if HAVE_SYSINFO
private:
    struct system_config *systemConfig;
#endif
};

} //MeeGo namespace

#endif /* QMSYSTEMINFORMATION_P_H */
