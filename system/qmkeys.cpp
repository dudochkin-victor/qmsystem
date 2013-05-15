/*!
 * @file qmkeys.cpp
 * @brief QmKeys

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas@nokia.com>
   @author Yang Yang <ext-yang.25.yang@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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
#include "qmkeys.h"
#include "qmkeys_p.h"

namespace MeeGo
{
    QmKeysPrivate::QmKeysPrivate(QObject *parent) : QObject(parent) {
        socket = new QLocalSocket(this);
        socket->connectToServer(SERVER_NAME);
        if (!socket->waitForConnected()) {
            qWarning() << "Could not connect to " << SERVER_NAME;
        }
        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        cameraFocusDown = false;
    }
    QmKeysPrivate::~QmKeysPrivate() {
        socket->disconnect();
        delete socket;
    }

    QmKeys::Key QmKeysPrivate::codeToKey(__u16 code) {
        switch (code) {
        case KEY_CAMERA:
            return QmKeys::Camera;
        case KEY_VOLUMEUP:
            return QmKeys::VolumeUp;
        case KEY_VOLUMEDOWN:
            return QmKeys::VolumeDown;
        case KEY_UP:
            return QmKeys::UpKey;
        case KEY_DOWN:
            return QmKeys::DownKey;
        case KEY_LEFT:
            return QmKeys::LeftKey;
        case KEY_RIGHT:
            return QmKeys::RightKey;
        case KEY_END:
            return QmKeys::End;
        case KEY_MUTE:
            return QmKeys::Mute;
        case KEY_STOPCD:
        case KEY_STOP:
            return QmKeys::Stop;
        case KEY_FASTFORWARD:
        case KEY_FORWARD:
            return QmKeys::Forward;
        case KEY_NEXTSONG:
            return QmKeys::NextSong;
        case KEY_PLAYPAUSE:
            return QmKeys::PlayPause;
        case KEY_PLAYCD:
            return QmKeys::Play;
        case KEY_REWIND:
            return QmKeys::Rewind;
        case KEY_PREVIOUSSONG:
            return QmKeys::PreviousSong;
        case KEY_PHONE:
            return QmKeys::Phone;
        case KEY_PAUSECD:
            return QmKeys::Pause;
        case KEY_RIGHTCTRL:
            return QmKeys::RightCtrl;
        case KEY_POWER:
            return QmKeys::PowerKey;
        default:
            return QmKeys::UnknownKey;
        }
    }

    struct input_event QmKeysPrivate::keyToEvent(QmKeys::Key key) {
        struct input_event ev;
        memset(&ev, 0, sizeof(struct input_event));

        switch (key) {
        case QmKeys::KeyboardSlider:
            ev.type = EV_SW;
            ev.code = SW_KEYPAD_SLIDE;
            break;
        case QmKeys::VolumeUp:
            ev.type = EV_KEY;
            ev.code = KEY_VOLUMEUP;
            break;
        case QmKeys::VolumeDown:
            ev.type = EV_KEY;
            ev.code = KEY_VOLUMEDOWN;
            break;
        case QmKeys::PowerKey:
            ev.type = EV_KEY;
            ev.code = KEY_POWER;
            break;
        case QmKeys::Phone:
        case QmKeys::PlayPause:
        case QmKeys::Play:
        case QmKeys::Stop:
        case QmKeys::Forward:
        case QmKeys::Rewind:
        case QmKeys::Pause:
        case QmKeys::RightCtrl:
        default:
            break;
        }
        return ev;
    }
    QmKeys::State QmKeysPrivate::getKeyState(QmKeys::Key key) {
        QmKeys::State state = QmKeys::KeyInvalid;
        if (keyMap.find(key) != keyMap.end()) {
            state = keyMap.value(key);
            goto EXIT;
        }
        if (key == QmKeys::Camera) {
            struct input_event query;
            query.type = EV_KEY;
            query.code = KEY_CAMERA_FOCUS;
            int focus = getKeyValue(query);
            query.code = KEY_CAMERA;
            int camera = getKeyValue(query);
            if (focus == 0 && camera == 0) {
                state = QmKeys::KeyUp;
            } else if (focus == 1 && camera == 0) {
                state = QmKeys::KeyHalfDown;
            } else if (focus == 1 && camera == 1) {
                state = QmKeys::KeyDown;
            } else if (focus == 0 && camera == 1) {
                state = QmKeys::KeyDown;
            }
        } else {
            struct input_event query = keyToEvent(key);
            int value = getKeyValue(query);
            if (value == 0) {
                state = QmKeys::KeyUp;
            } else if (value == 1) {
                state = QmKeys::KeyDown;
            }
        }
        EXIT:
        return state;
    }

    int QmKeysPrivate::getKeyValue(const struct input_event &query) {

        // Try to connect to qmkeyd.
        QLocalSocket socket;
        socket.connectToServer(SERVER_NAME);
        if (!socket.waitForConnected(1000)) {
            return -1;
        }

        // Query for the given key
        if (socket.write((char*)&query, sizeof(query)) != sizeof(query)) {
            return -1;
        }
        if (!socket.waitForReadyRead(1000)) {
            return -1;
        }
        struct input_event response;
        int ret = 0;

        // A loop because we might receive other events as well.
        do {
            ret = socket.read((char*)&response, sizeof(response));
            if (ret == sizeof(response)) {
                if (response.type == query.type && response.code == query.code) {
                    break;
                }
            }
        } while (ret == sizeof(response));
        socket.disconnect();

        return response.value;
    }

    /* The logic in camera keys is as follows:
     * We have two events: KEY_CAMERA and KEY_CAMERA_FOCUS.
     * KeyUp: KEY_CAMERA == 0 && KEY_CAMERA_FOCUS == 0
     * KeyHalfDown: KEY_CAMERA == 0 && KEY_CAMERA_FOCUS == 0
     * KeyDown: KEY_CAMERA == 1 && KEY_CAMERA_FOCUS 1
     *
     * So we have few invalid states. If we receive these, we print a warning.
     * If KEY_CAMERA_FOCUS == 0 and we receive KEY_CAMERA == 1, goto KeyDown.
     * If KEY_CAMERA == 1 and we receive KEY_CAMERA_FOCUS == 0, goto KeyUp
     * If KEY_CAMERA == 1 || KEY_CAMERA_FOCUS == 1 and we receive KEY_CAMERA_FOCUS == 1, do nothing.
     */
    void QmKeysPrivate::readyRead() {

        int ret=0;
        do {
            struct input_event ev;
            ret = socket->read((char*)&ev, sizeof(ev));
            if (ret == sizeof(ev)) {
                if (ev.type == EV_KEY) {
                    switch (ev.code) {
                    case KEY_PAUSECD:
                    case KEY_UP:
                    case KEY_LEFT:
                    case KEY_RIGHT:
                    case KEY_END:
                    case KEY_DOWN:
                    case KEY_MUTE:
                    case KEY_STOP:
                    case KEY_FORWARD:
                    case KEY_PLAYPAUSE:
                    case KEY_REWIND:
                    case KEY_PREVIOUSSONG:
                    case KEY_PHONE:
                    case KEY_PLAYCD:
                    case KEY_NEXTSONG:
                    case KEY_STOPCD:
                    case KEY_FASTFORWARD:
                    case KEY_RIGHTCTRL:
                    case KEY_POWER:
                        {
                            QmKeys::Key key = codeToKey(ev.code);
                            QmKeys::State state;
                            if (ev.value == 0) {
                                state = QmKeys::KeyUp;
                            } else {
                                state = QmKeys::KeyDown;
                            }
                            keyMap[key] = state;
                            emit keyEvent(key, state);
                        }
                        break;
                    case KEY_CAMERA:
                        if (ev.value == 0) {
                            if (cameraFocusDown) {
                                keyMap[QmKeys::Camera] = QmKeys::KeyHalfDown;
                                emit cameraLauncherMoved(QmKeys::Down);
                            } else {
                                keyMap[QmKeys::Camera] = QmKeys::KeyUp;
                                emit cameraLauncherMoved(QmKeys::Up);
                            }
                        } else {
                            keyMap[QmKeys::Camera] = QmKeys::KeyDown;
                            emit cameraLauncherMoved(QmKeys::Through);
                            if (!cameraFocusDown) {
                                qWarning() << "Received a Camera down event without being half down.";
                            }
                        }
                        emit keyEvent(QmKeys::Camera, keyMap.value(QmKeys::Camera));
                        break;
                    case KEY_CAMERA_FOCUS:
                        if (ev.value == 0) {
                            cameraFocusDown = false;
                            if (!keyMap.contains(QmKeys::Camera) || keyMap.value(QmKeys::Camera) != QmKeys::KeyHalfDown) {
                                qWarning() << "Received a KEY_CAMERA_FOCUS up event without being in HalfDown state.";
                            }
                            keyMap[QmKeys::Camera] = QmKeys::KeyUp;
                            emit cameraLauncherMoved(QmKeys::Up);
                        } else {
                            cameraFocusDown = true;
                            if (!keyMap.contains(QmKeys::Camera) || keyMap.value(QmKeys::Camera) == QmKeys::KeyUp) {
                                keyMap[QmKeys::Camera] = QmKeys::KeyHalfDown;
                                emit cameraLauncherMoved(QmKeys::Down);
                            } else {
                                qWarning() << "Received a KEY_CAMERA_FOCUS down event in state " << keyMap.value(QmKeys::Camera);
                            }
                        }
                        emit keyEvent(QmKeys::Camera, keyMap.value(QmKeys::Camera));
                        break;
                    case KEY_VOLUMEUP:
                        if (ev.value == 0) {
                            keyMap[QmKeys::VolumeUp] = QmKeys::KeyUp;
                            emit volumeUpMoved(false);
                        } else  if (ev.value == 1 ) {
                            keyMap[QmKeys::VolumeUp] = QmKeys::KeyDown;
                            emit volumeUpMoved(true);
                        }
                        emit keyEvent(QmKeys::VolumeUp, keyMap.value(QmKeys::VolumeUp));
                        break;
                    case KEY_VOLUMEDOWN:
                        if (ev.value == 0) {
                            keyMap[QmKeys::VolumeDown] = QmKeys::KeyUp;
                            emit volumeDownMoved(false);
                        } else if (ev.value == 1) {
                            keyMap[QmKeys::VolumeDown] = QmKeys::KeyDown;
                            emit volumeDownMoved(true);
                        }
                        emit keyEvent(QmKeys::VolumeDown, keyMap.value(QmKeys::VolumeDown));
                        break;
                    }
                } else if (ev.type == EV_SW) {
                    switch (ev.code) {
                        case SW_KEYPAD_SLIDE:
                            if (ev.value == 0) {
                                keyMap[QmKeys::KeyboardSlider] = QmKeys::KeyUp;
                                emit keyboardSliderMoved(QmKeys::KeyboardSliderOut);
                            } else {
                                keyMap[QmKeys::KeyboardSlider] = QmKeys::KeyDown;
                                emit keyboardSliderMoved(QmKeys::KeyboardSliderIn);
                            }
                            emit keyEvent(QmKeys::KeyboardSlider, keyMap.value(QmKeys::KeyboardSlider));
                        break;
                    }
                }
            }
        } while (ret > 0);
    }

    QmKeys::QmKeys(QObject *parent) : QObject(parent) {
        priv = new QmKeysPrivate();
        connect(priv, SIGNAL(keyEvent(MeeGo::QmKeys::Key, MeeGo::QmKeys::State)), this, SIGNAL(keyEvent(MeeGo::QmKeys::Key, MeeGo::QmKeys::State)));
        connect(priv, SIGNAL(volumeDownMoved(bool)), this, SIGNAL(volumeDownMoved(bool)));
        connect(priv, SIGNAL(volumeUpMoved(bool)), this, SIGNAL(volumeUpMoved(bool)));
        connect(priv, SIGNAL(cameraLauncherMoved(QmKeys::CameraKeyPosition)), this, SIGNAL(cameraLauncherMoved(QmKeys::CameraKeyPosition)));
        connect(priv, SIGNAL(lensCoverMoved(QmKeys::LensCoverPosition)), this, SIGNAL(lensCoverMoved(QmKeys::LensCoverPosition)));
        connect(priv, SIGNAL(keyboardSliderMoved(QmKeys::KeyboardSliderPosition)), this, SIGNAL(keyboardSliderMoved(QmKeys::KeyboardSliderPosition)));
    }

    QmKeys::~QmKeys() {
        delete priv;
    }

    QmKeys::State QmKeys::getKeyState(Key key) {
        return priv->getKeyState(key);
    }

    QmKeys::KeyboardSliderPosition QmKeys::getSliderPosition() {
        if (getKeyState(KeyboardSlider) == KeyDown) {
            return KeyboardSliderIn;
        } else {
            return KeyboardSliderOut;
        }
    }

    QmKeys::LensCoverPosition QmKeys::getLensCoverPosition() {
        return Unknown;
    }

    QmKeys::CameraKeyPosition QmKeys::getCameraLauncherPosition() {
        State state = getKeyState(Camera);
        switch (state) {
        case KeyUp:
            return Up;
        case KeyHalfDown:
            return Down;
        case KeyDown:
            return Through;
        case KeyInvalid:
        default:
            return Invalid;
        }
    }

    bool QmKeys::isVolumeUpDown() {
        if (getKeyState(VolumeUp) == KeyDown) {
            return true;
        } else {
            return false;
        }
    }

    bool QmKeys::isVolumeDownDown() {
        if (getKeyState(VolumeDown) == KeyDown) {
            return true;
        } else {
            return false;
        }
    }

} // namespace MeeGo
