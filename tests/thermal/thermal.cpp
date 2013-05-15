/**
 * @file thermal.cpp
 * @brief QmThermal tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
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

#include <QObject>
#include <qmthermal.h>
#include <QTest>
#include <QDebug>
#include <QFile>

#define BME_TEMP_CONF_FILE "/etc/dsme/temp_surface"

extern "C" {
#include <sys/types.h>
typedef __uint8_t  uint8;
typedef __uint16_t uint16;
typedef __uint32_t uint32;
typedef uint8      byte;
typedef __int16_t  int16;
typedef __int32_t  int32;

/* TODO: submit bug report to bme: */
/* these need to be #defined in order to pick up thermal stuff from bme */
#define TESTSERVER

#include <bme/bmemsg.h>
#include <bme/bmeipc.h>
#include <bme/em_isi.h>
}
int bme_get_battery_temperature(void)
{
    int temperature = -1;

    /* read battery temperature from BME over IPC */

    if (bme_connect() >= 0) {
        int res;
        int nb = 0;
        union emsg_battery_info msg;

        msg.request.type = EM_BATTERY_INFO_REQ;
        msg.request.subtype = 0;
        msg.request.flags = EM_BATTERY_TEMP;
        res = bme_send_get_reply(&msg, sizeof(msg.request), &msg, sizeof(msg.reply), &nb);
        if (res >= 0 && nb == sizeof(msg.reply)) {
            temperature = msg.reply.temp;
        } else {
            return -1;
        }
        bme_disconnect();
    }

    /* convert from kelvin to degrees celsius and -1 to get outside temperature*/
    temperature = temperature - 273 -1;

    return temperature;
}
int dsme_adjust_thermal_limit_values(const char* filename, MeeGo::QmThermal::ThermalState thermal_state, int current_temp)
{
    char * buf;

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return -1;
    }


    buf=(char*)malloc(60*sizeof(char));
    memset(buf, 0, sizeof(buf));
    /* modify file input */
    switch (thermal_state){
    case MeeGo::QmThermal::Normal:
            sprintf(buf, "%d, %d, 2\n%d, %d, 2\n%d, %d, 2\n%d, %d, 2\n", current_temp-5, current_temp+5,
                    current_temp+10, current_temp+15, current_temp+20, current_temp+25, current_temp+30, current_temp+35);
            break;
    case MeeGo::QmThermal::Warning:
            sprintf(buf, "%d, %d, 2\n%d, %d, 2\n%d, %d, 2\n%d, %d, 2\n", current_temp-15, current_temp-10,
                    current_temp-5, current_temp+5, current_temp+10, current_temp+15, current_temp+20, current_temp+25);
            break;
    case MeeGo::QmThermal::Alert:
            sprintf(buf, "%d, %d, 2\n%d, %d, 2\n%d, %d, 2\n%d, %d, 2\n", current_temp-25, current_temp-20,
                    current_temp-15, current_temp-10, current_temp-5, current_temp+5, current_temp+10, current_temp+15);
            break;
    default:
            free(buf);
            return -1;
        }
    if(file.write(buf, strlen(buf))<0)
    {
       free(buf);
       return -1;
    }

    free(buf);

    return 0;
}

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent) {}

public slots:
    void thermalChanged(MeeGo::QmThermal::ThermalState){}
};


class TestClass : public QObject
{
    Q_OBJECT

public slots:
    void thermalChanged(MeeGo::QmThermal::ThermalState newState) {
        state = newState;
    }

private:
    MeeGo::QmThermal *thermal;
    MeeGo::QmThermal::ThermalState state;
    QString thermalStateToString(int i) {
        switch ((MeeGo::QmThermal::ThermalState)i) {
        case MeeGo::QmThermal::Normal:
            return "Normal";
            break;
        case MeeGo::QmThermal::Alert:
            return "Alert";
            break;
        case MeeGo::QmThermal::Warning:
            return "Warning";
            break;
        default:
            return QString::number(i);
        }
    }

private slots:
    void initTestCase() {
        thermal = new MeeGo::QmThermal();
        QVERIFY(thermal);
    }

    void testConnectSignals() {
        QVERIFY(connect(thermal, SIGNAL(thermalChanged(MeeGo::QmThermal::ThermalState)),
                        this, SLOT(thermalChanged(MeeGo::QmThermal::ThermalState))));
    }

    void testGet() {
        MeeGo::QmThermal::ThermalState result = thermal->get();
        (void)result;
    }

    void testSignals() {
        int temp = bme_get_battery_temperature();
        QVERIFY(temp > 0);
        for (int i=2; i >= 0; i--) {
            qDebug() << "Setting ThermalState " << thermalStateToString(i);
            QVERIFY(dsme_adjust_thermal_limit_values(BME_TEMP_CONF_FILE, (MeeGo::QmThermal::ThermalState)i, temp) == 0);
            QTest::qWait(65*1000);
            QCOMPARE((MeeGo::QmThermal::ThermalState)i, state);
            QCOMPARE(state, thermal->get());
        }
    }

    void cleanupTestCase() {
        delete thermal;
    }
};

QTEST_MAIN(TestClass)
#include "thermal.moc"
