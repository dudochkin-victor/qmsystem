/**
 * @file hw_keys.cpp
 * @brief QmKeys tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
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
#include <qmkeys.h>
#include <QTest>

using namespace MeeGo;

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent) {}

public slots:
    void cameraLauncherMoved(QmKeys::CameraKeyPosition){}
    void keyboardSliderMoved(QmKeys::KeyboardSliderPosition){}
    void lensCoverMoved(QmKeys::LensCoverPosition){}
    void volumeUpMoved(bool){}
    void volumeDownMoved(bool){}
};


class TestClass : public QObject
{
    Q_OBJECT

private:
    QmKeys *keys;
    SignalDump signalDump;

private slots:
    void initTestCase() {
        keys = new QmKeys();
        QVERIFY(keys);
    }

    void testConnectSignals() {
        QVERIFY(connect(keys, SIGNAL(cameraLauncherMoved(QmKeys::CameraKeyPosition)),
                &signalDump, SLOT(cameraLauncherMoved(QmKeys::CameraKeyPosition))));
        QVERIFY(connect(keys, SIGNAL(keyboardSliderMoved(QmKeys::KeyboardSliderPosition)),
                &signalDump, SLOT(keyboardSliderMoved(QmKeys::KeyboardSliderPosition))));
        QVERIFY(connect(keys, SIGNAL(lensCoverMoved(QmKeys::LensCoverPosition)),
                &signalDump, SLOT(lensCoverMoved(QmKeys::LensCoverPosition))));
        QVERIFY(connect(keys, SIGNAL(volumeUpMoved(bool)),
                        &signalDump, SLOT(volumeUpMoved(bool))));
        QVERIFY(connect(keys, SIGNAL(volumeDownMoved(bool)),
                        &signalDump, SLOT(volumeDownMoved(bool))));
    }

    void testGetSliderPosition(){
        QmKeys::KeyboardSliderPosition result = keys->getSliderPosition();
        (void)result;
    }

    void testGetLensCoverPosition(){
        QmKeys::LensCoverPosition result = keys->getLensCoverPosition();
        (void)result;
    }

    void testGetCameraLauncherPosition(){
        QmKeys::CameraKeyPosition result = keys->getCameraLauncherPosition();
        (void)result;
    }

    void testGetVolumeUpPosition(){
        bool result = keys->isVolumeUpDown();
        (void)result;
    }

    void testGetVolumeDownPosition(){
        bool result = keys->isVolumeDownDown();
        (void)result;
    }

   void cleanupTestCase() {
        delete keys;
    }
};
QTEST_MAIN(TestClass)
#include "hw_keys.moc"
