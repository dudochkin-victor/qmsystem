/*!
 * @file qmsysteminformation.h
 * @brief Contains QmSystemInformation.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

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
#ifndef QMSYSTEMINFORMATION_H
#define QMSYSTEMINFORMATION_H

#include "system_global.h"

#include <QObject>

namespace MeeGo {

class QmSystemInformationPrivate;

class MEEGO_SYSTEM_EXPORT QmSystemInformation : public QObject
{
    Q_OBJECT

public:
    QmSystemInformation();
    virtual ~QmSystemInformation();

    /*!
     * @brief Gets the value of a key from system information.
     * @return Returns the value as a QString. Note: the value can be empty if the key contains an empty value or there was an error retrieving the value.
     */
    QString valueForKey(const QString &key) const;

    /*!
     * @brief Gets the value of a key from the kernel command line.
     * @return Returns the kernel command line value as a QString
     */
    static QString kernelCommandLineValueForKey(const QString &key);

private:
    Q_DISABLE_COPY(QmSystemInformation)
    MEEGO_DECLARE_PRIVATE(QmSystemInformation)
};

} // MeeGo namespace

#endif // QMSYSTEMINFORMATION_H
