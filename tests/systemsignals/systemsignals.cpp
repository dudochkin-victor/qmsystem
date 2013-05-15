/**
 * @file systemsignals.cpp
 * @brief QmSystemState tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Tuomo Tanskanen <ext-tuomo.1.tanskanen@nokia.com>

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
#include <QTest>
#include <QDebug>
#include <QProcess>
#include <QDBusConnection>
#include <QDBusMessage>

#include <qmsystemstate.h>
#include <msystemdbus_p.h>

class SignalDump : public QObject {
    Q_OBJECT

public:
	MeeGo::QmSystemState::StateIndication state;

public:
    SignalDump(QObject *parent = NULL) : QObject(parent) {}

public slots:
	void systemStateChanged(MeeGo::QmSystemState::StateIndication newState) {
		qDebug() << "received systemStateChanged" << newState;
		state = newState;
	}
};


/*
 * You need to comment the line \"/usr/sbin/dsmetool --start-dbus\" from
 * /etc/init/sysconfig-late.conf and reboot before running this test"
 *
 */
class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmSystemState *systemstate;
    SignalDump signaldump;
    QString bus_name;

private slots:
    void initTestCase() {
        systemstate = new MeeGo::QmSystemState();
        QVERIFY(systemstate);
    }

    void testConnectSignals() {
        QVERIFY(connect(systemstate, SIGNAL(systemStateChanged(MeeGo::QmSystemState::StateIndication)),
                        &signaldump, SLOT(systemStateChanged(MeeGo::QmSystemState::StateIndication))));
    }

	void testRegisterThermalmanager() {
		QDBusConnection system_bus = QDBusConnection::systemBus();
		QVERIFY2(system_bus.registerService(SYS_THERMALMANAGER_SERVICE),
			"Registering 'com.nokia.thermalmanager' failed! You probably still have the real DSME "
                        "running. Please comment the line \"/usr/sbin/dsmetool --start-dbus\" from /etc/init/sysconfig-late.conf and reboot to run tests!");
	}

	void testThermalFatalSignal() {
		signaldump.state = MeeGo::QmSystemState::Shutdown; // initial state != wanted result

		QDBusMessage dbus_signal = QDBusMessage::createSignal(
			SYS_THERMALMANAGER_PATH,
			SYS_THERMALMANAGER_SERVICE,
			SYS_THERMALMANAGER_STATE_SIG);

		QList<QVariant> arguments;
		arguments << QString("fatal");
		dbus_signal.setArguments(arguments);
		
		QVERIFY(QDBusConnection::systemBus().send(dbus_signal));
		QTest::qWait(1000);

		QVERIFY(signaldump.state == MeeGo::QmSystemState::ThermalStateFatal);
	}

	void testRegisterDsme() {
		QDBusConnection system_bus = QDBusConnection::systemBus();
		QVERIFY2(system_bus.registerService(SYS_DSME_SERVICE),
                        "Registering 'com.nokia.dsme' failed! You probably still have the real DSME "
                        "running. Please comment the line \"/usr/sbin/dsmetool --start-dbus\" from /etc/init/sysconfig-late.conf and reboot to run tests!");
	}

	void testBatteryEmptySignal() {
		signaldump.state = MeeGo::QmSystemState::Shutdown; // initial state != wanted result

		QDBusMessage dbus_signal = QDBusMessage::createSignal(
			SYS_DSME_SIG_PATH,
			SYS_DSME_SIG_INTERFACE,
			SYS_DSME_BATTERYEMPTY_SIG);
		
		QVERIFY(QDBusConnection::systemBus().send(dbus_signal));
		QTest::qWait(1000);

		QVERIFY(signaldump.state == MeeGo::QmSystemState::BatteryStateEmpty);
	}

	void testSaveDataSignal() {
		signaldump.state = MeeGo::QmSystemState::Shutdown; // initial state != wanted result

		QDBusMessage dbus_signal = QDBusMessage::createSignal(
			SYS_DSME_SIG_PATH,
			SYS_DSME_SIG_INTERFACE,
			SYS_DSME_UNSAVEDDATA_SIG);
		
		QVERIFY(QDBusConnection::systemBus().send(dbus_signal));
		QTest::qWait(1000);

		QVERIFY(signaldump.state == MeeGo::QmSystemState::SaveData);
	}

	void testShutdownSignal() {
                signaldump.state = MeeGo::QmSystemState::SaveData; // initial state != wanted result

		QDBusMessage dbus_signal = QDBusMessage::createSignal(
			SYS_DSME_SIG_PATH,
			SYS_DSME_SIG_INTERFACE,
			SYS_DSME_SHUTDOWN_SIG);
		
		QVERIFY(QDBusConnection::systemBus().send(dbus_signal));
		QTest::qWait(1000);

                QVERIFY(signaldump.state == MeeGo::QmSystemState::Shutdown);
	}
	
    void cleanupTestCase() {
        delete systemstate;
    }
};

QTEST_MAIN(TestClass)
#include "systemsignals.moc"
