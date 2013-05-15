/**
 * @file host_system.cpp
 * @brief QmSystemState tests

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
#include <QDebug>
#include <qmsystemstate.h>
#include <iostream>

using namespace std;
using namespace MeeGo;

int main(int argc, char** argv)
{
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " reboot | shutdown" << endl;
        return 1;
    }

    QmSystemState *systemstate = new MeeGo::QmSystemState();
    //systemstate->set(QmSystemState::Powerup);

    if (strcmp(argv[1], "reboot") == 0) {
        systemstate->set(QmSystemState::Reboot);
    } else {
        systemstate->set(QmSystemState::ShuttingDown);
    }

    delete systemstate;

    return 0;
};

