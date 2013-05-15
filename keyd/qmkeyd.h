/*!
 * @file qmkeyd.h
 * @brief QmKeyd

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Yang Yang <ext-yang.25.yang@nokia.com>

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
#ifndef QMKEYD_H
#define QMKEYD_H

#include <QLocalServer>
#include <QCoreApplication>
#include <QSocketNotifier>
#include <QLocalSocket>

#include <linux/input.h>
#include <stdint.h>

#define SERVER_NAME "/tmp/qmkeyd"

class QmKeyd : public QCoreApplication
{
    Q_OBJECT

public:
    QmKeyd(int argc, char** argv);
    ~QmKeyd();

private slots:
    void newConnection();
    void disconnected();
    void didReceiveKeyEventFromFile(int);
    void clientSocketReadyRead();
    void detectBT(int);

private:
    void cleanSocket();
    bool isKeySupported(struct input_event &ev);
    bool isHeadset(int fd);
    void openHandles();
    void closeHandles();
    void closeBT();
    void removeInotifyWatch();
    void failStart(const char *fmt, ...);
    bool isKeyPressed(int fd, int key);

    QLocalServer *server;
    QVector<QLocalSocket*> connections;

    int gpioFile, keypadFile, eciFile, powerButtonFile, btFile;
    QSocketNotifier *gpioNotifier, *keypadNotifier, *eciNotifier, *powerButtonNotifier, *btNotifier, *inputNotifier;

    int inotifyWd, inotifyFd;
    char *btfname;
    int users;
};

#endif // QMKEYD_H
