/*!
 * @file qmkeys_p.h
 * @brief Contains QmKeysPrivate

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>

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
#ifndef QMKEYS_P_H
#define QMKEYS_P_H

#include "qmkeys.h"
#include <linux/input.h>
#include <QLocalSocket>

#define SERVER_NAME "/tmp/qmkeyd"
#ifndef KEY_CAMERA_FOCUS
#define KEY_CAMERA_FOCUS 0x210
#endif
#ifndef SW_KEYPAD_SLIDE
#define SW_KEYPAD_SLIDE 0x0a
#endif

namespace MeeGo {

class QmKeysPrivate : public QObject
{
    Q_OBJECT

public:
    QmKeysPrivate(QObject *parent = NULL);
    ~QmKeysPrivate();

    struct input_event keyToEvent(QmKeys::Key key);
    QmKeys::State getKeyState(QmKeys::Key key);
    int getKeyValue(const struct input_event &query);
    QmKeys::Key codeToKey(__u16 code);

public Q_SLOTS:
    void readyRead();


Q_SIGNALS:
  void keyboardSliderMoved(QmKeys::KeyboardSliderPosition where);

  void lensCoverMoved(QmKeys::LensCoverPosition where);

  void cameraLauncherMoved(QmKeys::CameraKeyPosition);

  void volumeUpMoved(bool);

  void volumeDownMoved(bool);

  void keyEvent(MeeGo::QmKeys::Key key, MeeGo::QmKeys::State state);

private:

    QLocalSocket *socket;
    QMap<QmKeys::Key, QmKeys::State> keyMap;
    bool cameraFocusDown;
};

}

#endif // QMKEYS_P_H
