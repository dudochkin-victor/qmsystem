/*!
 * @file qmled.cpp
 * @brief QmLed

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>

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
#include "qmled.h"

#include "qmipcinterface_p.h"

#if HAVE_MCE
    #include <mce/dbus-names.h>
    #include <mce/mode-names.h>
#endif

namespace MeeGo {

class QmLEDPrivate
{
    MEEGO_DECLARE_PUBLIC(QmLED)

public:
    QmLEDPrivate() {
        #if HAVE_MCE
             requestIf = new QmIPCInterface(MCE_SERVICE,
                                            MCE_REQUEST_PATH,
                                            MCE_REQUEST_IF);
        #endif
    }

    ~QmLEDPrivate() {
        #if HAVE_MCE
            delete requestIf, requestIf = 0;
        #endif
    }

    QmIPCInterface *requestIf;
};

QmLED::QmLED(QObject *parent)
              : QObject(parent) {
    MEEGO_INITIALIZE(QmLED)
}

QmLED::~QmLED() {
    MEEGO_UNINITIALIZE(QmLED);
}

bool QmLED::activate(const QString &pattern) {
    #if HAVE_MCE
        MEEGO_PRIVATE(QmLED);
        priv->requestIf->callAsynchronously(MCE_ACTIVATE_LED_PATTERN, pattern);
        return true;
    #else
        Q_UNUSED(pattern);
        return false;
    #endif
}

bool QmLED::deactivate(const QString &pattern) {
    #if HAVE_MCE
         MEEGO_PRIVATE(QmLED);
         priv->requestIf->callAsynchronously(MCE_DEACTIVATE_LED_PATTERN, pattern);
         return true;
    #else
        Q_UNUSED(pattern);
        return false;
    #endif
}

bool QmLED::enable(void) {
    #if HAVE_MCE
        MEEGO_PRIVATE(QmLED);
        priv->requestIf->callAsynchronously(MCE_ENABLE_LED);
        return true;
    #else
        return false;
    #endif
}

bool QmLED::disable(void) {
    #if HAVE_MCE
        MEEGO_PRIVATE(QmLED);
        priv->requestIf->callAsynchronously(MCE_DISABLE_LED);
        return true;
    #else
        return false;
    #endif
}

} //namespace MeeGo
