/**
 * @file manual_keys.cpp
 * @brief QmKeys manual tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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
#include <QDebug>

using namespace MeeGo;


class TestClass : public QObject
{
    Q_OBJECT

public slots:
    void cameraChanged(QmKeys::CameraKeyPosition newPos) {
        // The initial Invalid is ok.
        if (cameraPos == QmKeys::Invalid) {
            QVERIFY(newPos != QmKeys::Invalid);
        } else {
            switch (newPos) {
            case QmKeys::Up:
                QVERIFY(cameraPos == QmKeys::Down);
                break;
            case QmKeys::Through:
                QVERIFY(cameraPos == QmKeys::Down);
                break;
            case QmKeys::Down:
                QVERIFY(cameraPos == QmKeys::Up || cameraPos == QmKeys::Through);
                break;
            default:
                QFAIL("The camera is in invalid state!");
            }
        }

        cameraPos = newPos;
        printf("\tCurrent camera position is: %s\n", cameraToString(cameraPos).toAscii().data());
    }

    void sliderChanged(QmKeys::KeyboardSliderPosition newPos) {
        sliderPos = newPos;

        printf("\tCurrent keyboard slider position is: %s\n", sliderToString(sliderPos).toAscii().data());
    }

    void volumeUpPressed(bool pressed) {
        if (volumeUp != pressed) {
            printf("\tCurrent volumeUp position is: %s\n", pressed ? "pressed" : "up");
            volumeUp = pressed;
        }
    }

    void volumeDownPressed(bool pressed) {
        if (volumeDown != pressed) {
            printf("\tCurrent volumeDown position is: %s\n", pressed ? "pressed": "up");
            volumeDown = pressed;
        }
    }

    void keyEvent(MeeGo::QmKeys::Key key, MeeGo::QmKeys::State state) {
        if (keyMap.find(key) == keyMap.end() || keyMap.value(key) != state) {
            printf("\tkeyEvent: %s in state %s\n",keyToString(key).toAscii().data(), stateToString(state).toAscii().data());
        }
        keyMap[key] = state;
    }

private:
    QmKeys *keys;
    QmKeys::CameraKeyPosition cameraPos;
    QmKeys::KeyboardSliderPosition sliderPos;
    bool volumeUp;
    bool volumeDown;
    QMap<QmKeys::Key, QmKeys::State> keyMap;

    QString keyToString(QmKeys::Key key) {
        switch (key) {
        case QmKeys::KeyboardSlider:
            return "KeyboardSlider";
        case QmKeys::Camera:
            return "Camera";
        case QmKeys::VolumeUp:
            return "VolumeUp";
        case QmKeys::VolumeDown:
            return "VolumeDown";
        case QmKeys::Phone:
            return "Phone";
        case QmKeys::PlayPause:
            return "PlayPause";
        case QmKeys::Play:
            return "Play";
        case QmKeys::Stop:
            return "Stop";
        case QmKeys::Forward:
            return "Forward";
        case QmKeys::Rewind:
            return "Rewind";
        case QmKeys::PreviousSong:
            return "PreviousSong";
        case QmKeys::NextSong:
            return "NextSong";
        case QmKeys::Pause:
            return "Pause";
        case QmKeys::RightCtrl:
            return "RightCtrl";
        case QmKeys::PowerKey:
            return "PowerKey";
        default:
            return "Unknown";
        }
    }

    QString stateToString(QmKeys::State state) {
        switch (state) {
        case QmKeys::KeyUp:
            return "Up";
        case QmKeys::KeyDown:
            return "Down";
        case QmKeys::KeyHalfDown:
            return "HalfDown";
        case QmKeys::KeyInvalid:
            return "Invalid";
        default:
            return "Unknown";

        }
    }

    QString sliderToString(QmKeys::KeyboardSliderPosition pos) {
        switch(pos) {
        case QmKeys::KeyboardSliderIn:
            return "in";
            break;
        case QmKeys::KeyboardSliderOut:
            return "Out";
            break;
        default:
            return "Undefined";
        }
    }

    QString cameraToString(QmKeys::CameraKeyPosition pos) {
        switch (pos) {
        case QmKeys::Up:
            return "Up";
            break;
        case QmKeys::Down:
            return "Half down";
            break;
        case QmKeys::Through:
            return "Fully down";
            break;
        case QmKeys::Invalid:
            return "Invalid";
            break;
        default:
            return "Undefined";
        }
    }

    void doTest(QmKeys::CameraKeyPosition pos) {
        printf("Please press/release the camera button to be %s.\nYou have 10 seconds...\n", cameraToString(pos).toAscii().data());
        QTest::qWait(10*1000);
        QCOMPARE(cameraPos, pos);
        QCOMPARE(cameraPos, keys->getCameraLauncherPosition());

    }

    void doTestNew(QmKeys::Key key, QmKeys::State state) {
        printf("Please move %s to be in the state %s.\nYou have 10 seconds...\n", keyToString(key).toAscii().data(), stateToString(state).toAscii().data());
        QTest::qWait(10*1000);
        QCOMPARE(keyMap.value(key), state);
        QCOMPARE(state, keys->getKeyState(key));
    }

private slots:
    void initTestCase() {
        cameraPos = QmKeys::Invalid;
        sliderPos = QmKeys::KeyboardSliderIn;
        volumeUp = false;
        volumeDown = false;
        keys = new QmKeys();
        QVERIFY(connect(keys, SIGNAL(cameraLauncherMoved(QmKeys::CameraKeyPosition)), this, SLOT(cameraChanged(QmKeys::CameraKeyPosition))));
        QVERIFY(connect(keys, SIGNAL(keyboardSliderMoved(QmKeys::KeyboardSliderPosition)), this, SLOT(sliderChanged(QmKeys::KeyboardSliderPosition))));
        QVERIFY(connect(keys, SIGNAL(volumeUpMoved(bool)), this, SLOT(volumeUpPressed(bool))));
        QVERIFY(connect(keys, SIGNAL(volumeDownMoved(bool)), this, SLOT(volumeDownPressed(bool))));
        QVERIFY(connect(keys, SIGNAL(keyEvent(MeeGo::QmKeys::Key, MeeGo::QmKeys::State)),
                        this, SLOT(keyEvent(MeeGo::QmKeys::Key, MeeGo::QmKeys::State))));

    }

    void testCameraBasic() {
        printf("This test will ask you to press and keep the camera key in different positions.\nGet ready! The test starts in 10 seconds...\n\n");
        QTest::qWait(10*1000);
        doTest(QmKeys::Down);
        printf("\n\n");
        doTest(QmKeys::Through);
        printf("\n\n");
        doTest(QmKeys::Down);
        printf("\n\n");
        doTest(QmKeys::Up);
    }

    void testCameraBasicNew() {
        printf("This test will ask you to press and keep the camera key in different positions.\nGet ready! The test starts in 10 seconds...\n\n");
        QTest::qWait(10*1000);
        doTestNew(QmKeys::Camera, QmKeys::KeyHalfDown);
        doTestNew(QmKeys::Camera, QmKeys::KeyDown);
        doTestNew(QmKeys::Camera, QmKeys::KeyHalfDown);
        doTestNew(QmKeys::Camera, QmKeys::KeyUp);
    }

    void testCameraStress() {
        printf("This test will ask you to press the camera button in a rapid fashion.\nGet ready! The test starts in 10 seconds...\n\n");
        QTest::qWait(10*1000);
        printf("Please press and relase the camera button again and again as fast as you can for 10 seconds!\n");
        QTest::qWait(10*1000);
        printf("\n\n\nPlease release the camera button...\n\n\n");
        QTest::qWait(1000);
        QCOMPARE(cameraPos, QmKeys::Up);
    }

    void testKeypadSlideBasicNew() {
        printf("This test will ask you to slide keyboard. Please close the keyboard now.\nGet ready! The test starts in 10 seconds...\n\n");
        QTest::qWait(10*1000);
        doTestNew(QmKeys::KeyboardSlider, QmKeys::KeyUp);
        doTestNew(QmKeys::KeyboardSlider, QmKeys::KeyDown);
    }

    void testKeypadSlideBasic() {
        printf("This test will ask you to slide keyboard. Please close the keyboard now.\nGet ready! The test starts in 10 seconds...\n\n");
        QTest::qWait(10*1000);
        printf("Please open the keyboard. You have 10 seconds...\n");
        QTest::qWait(10*1000);
        QCOMPARE(sliderPos, QmKeys::KeyboardSliderOut);
        QCOMPARE(sliderPos, keys->getSliderPosition());
        printf("Please close the keyboard. You have 10 seconds...\n");
        QTest::qWait(10*1000);
        QCOMPARE(sliderPos, QmKeys::KeyboardSliderIn);
        QCOMPARE(sliderPos, keys->getSliderPosition());
    }

    void testVolumeBasicNew() {
        printf("This test will ask you to press the volume keys.\nGet ready! The test starts in 10 seconds...\n\n");
        QTest::qWait(10*1000);
        doTestNew(QmKeys::VolumeUp, QmKeys::KeyDown);
        doTestNew(QmKeys::VolumeUp, QmKeys::KeyUp);
        doTestNew(QmKeys::VolumeDown, QmKeys::KeyDown);
        doTestNew(QmKeys::VolumeDown, QmKeys::KeyUp);
    }

    void testVolumeBasic() {
        printf("This test will ask you to press the volume keys.\nGet ready! The test starts in 10 seconds...\n\n");
        QTest::qWait(10*1000);
        printf("Please press and hold volumeUp. You have 10 seconds...\n");
        QTest::qWait(10*1000);
        QVERIFY(volumeUp);
        QCOMPARE(volumeUp, keys->isVolumeUpDown());

        printf("Please release volumeUp. You have 10 seconds...\n");
        QTest::qWait(10*1000);
        QVERIFY(!volumeUp);
        QCOMPARE(volumeUp, keys->isVolumeUpDown());

        printf("Please press and hold volumeDown. You have 10 seconds...\n");
        QTest::qWait(10*1000);
        QVERIFY(volumeDown);
        QCOMPARE(volumeDown, keys->isVolumeDownDown());

        printf("Please release volumeDown. You have 10 seconds...\n");
        QTest::qWait(10*1000);
        QVERIFY(!volumeDown);
        QCOMPARE(volumeDown, keys->isVolumeDownDown());
    }

    void testECI() {
        printf("This test will ask you to press the ECI keys.\nPlease attach a ECI device.\nThe test starts in 10 seconds...\n\n");
        doTestNew(QmKeys::Phone, QmKeys::KeyDown);
        doTestNew(QmKeys::Phone, QmKeys::KeyUp);
        doTestNew(QmKeys::PlayPause, QmKeys::KeyDown);
        doTestNew(QmKeys::PlayPause, QmKeys::KeyUp);
        doTestNew(QmKeys::Stop, QmKeys::KeyDown);
        doTestNew(QmKeys::Stop, QmKeys::KeyUp);
        doTestNew(QmKeys::Forward, QmKeys::KeyDown);
        doTestNew(QmKeys::Forward, QmKeys::KeyUp);
        doTestNew(QmKeys::Rewind, QmKeys::KeyDown);
        doTestNew(QmKeys::Rewind, QmKeys::KeyUp);
        doTestNew(QmKeys::VolumeUp, QmKeys::KeyDown);
        doTestNew(QmKeys::VolumeUp, QmKeys::KeyUp);
        doTestNew(QmKeys::VolumeDown, QmKeys::KeyDown);
        doTestNew(QmKeys::VolumeDown, QmKeys::KeyUp);
    }

    void testBT() {
        printf("This test will ask you to press the BT keys.\nPlease pair a BT device.\n\n");
        printf("Please press and release PlayPause key.\n");
        QCOMPARE(keyMap.value(QmKeys::PlayPause), QmKeys::KeyDown);
        QCOMPARE(QmKeys::KeyDown, keys->getKeyState(QmKeys::PlayPause));
        QCOMPARE(keyMap.value(QmKeys::PlayPause), QmKeys::KeyUp);
        QCOMPARE(QmKeys::KeyUp, keys->getKeyState(QmKeys::PlayPause));

        doTestNew(QmKeys::Stop, QmKeys::KeyDown);
        doTestNew(QmKeys::Stop, QmKeys::KeyUp);

        printf("Please press Forward key.\n");
        QCOMPARE(keyMap.value(QmKeys::NextSong), QmKeys::KeyDown);
        QCOMPARE(QmKeys::KeyDown, keys->getKeyState(QmKeys::NextSong));
        QCOMPARE(keyMap.value(QmKeys::NextSong), QmKeys::KeyUp);
        QCOMPARE(QmKeys::KeyUp, keys->getKeyState(QmKeys::NextSong));

        doTestNew(QmKeys::Forward, QmKeys::KeyDown);
        doTestNew(QmKeys::Forward, QmKeys::KeyUp);

        printf("Please press and release Rewind key.\n");
        doTestNew(QmKeys::Rewind, QmKeys::KeyDown);
        doTestNew(QmKeys::Rewind, QmKeys::KeyUp);

        printf("Please press and release Rewind key.\n");
        QCOMPARE(keyMap.value(QmKeys::PreviousSong), QmKeys::KeyDown);
        QCOMPARE(QmKeys::KeyDown, keys->getKeyState(QmKeys::PreviousSong));
        QCOMPARE(keyMap.value(QmKeys::PreviousSong), QmKeys::KeyUp);
        QCOMPARE(QmKeys::KeyUp, keys->getKeyState(QmKeys::PreviousSong));

        doTestNew(QmKeys::Rewind, QmKeys::KeyDown);
        doTestNew(QmKeys::Rewind, QmKeys::KeyUp);
    }

    void testAllKeys() {
        printf("This test will ask you to press and hold different keys.\nYou might need the fingers of a pianist. :)\nGet ready! The test starts in 10 seconds...\n\n");

        printf("Please close keyboard slider, and press and hold volumeUp, volumeDown and camera button.\nYou have 10 seconds...\n");
        QTest::qWait(10*1000);
        QVERIFY(volumeUp);
        QCOMPARE(volumeUp, keys->isVolumeUpDown());
        QVERIFY(volumeDown);
        QCOMPARE(volumeDown, keys->isVolumeDownDown());
        QCOMPARE(cameraPos, QmKeys::Through);
        QCOMPARE(cameraPos, keys->getCameraLauncherPosition());
        QCOMPARE(sliderPos, QmKeys::KeyboardSliderIn);
        QCOMPARE(sliderPos, keys->getSliderPosition());

        printf("Please hold the buttons down and open the keyboard. :)\nYou have 10 seconds...\n");
        QTest::qWait(10*1000);
        QVERIFY(volumeUp);
        QCOMPARE(volumeUp, keys->isVolumeUpDown());
        QVERIFY(volumeDown);
        QCOMPARE(volumeDown, keys->isVolumeDownDown());
        QCOMPARE(cameraPos, QmKeys::Through);
        QCOMPARE(cameraPos, keys->getCameraLauncherPosition());
        QCOMPARE(sliderPos, QmKeys::KeyboardSliderOut);
        QCOMPARE(sliderPos, keys->getSliderPosition());
    }

    void testPowerButton()
    {
        printf("This test will ask you to press the power button.\nGet ready! The test starts in 10 seconds...\n\n");
        QTest::qWait(10*1000);
        printf("Please press and release the powerButton. Listening events for 10 seconds\n");
        QTest::qWait(10*1000);
        printf("Did you see the PowerKey events?\n");
        doTestNew(QmKeys::PowerKey, QmKeys::KeyDown);
        doTestNew(QmKeys::PowerKey, QmKeys::KeyUp);

    }

    void testCamera4ever()
    {
        while (true) {
            QTest::qWait(10*1000);
        }
    }

    void cleanupTestCase() {
        delete keys;
    }
};

QTEST_MAIN(TestClass)
#include "manual_keys.moc"
