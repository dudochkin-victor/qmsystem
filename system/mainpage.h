/*!
 * @file mainpage.h
 * @brief Documentation

 * @copyright Nokia Corporation
 * @license LGPL Lesser General Public License

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Timo Rongas <ext-timo.rongas.nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

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

@mainpage QmSystem2 Library

@section intro Introduction

QmSystem2 is a library that provides interfaces to different system functionalities and hardware through the Qt API.
Examples of such functionalities are energy management, system state management, device mode, accessories, display backlight control,
and time management.

To access the sensors such as accelerometer, compass, magnetometer, orientation, proximity, rotation and ambient light sensor,
it is recommended that you use the Qt Mobility <A HREF="http://doc.qt.nokia.com/qtmobility-1.1.0/sensors-api.html">Sensors API</a>.

<STRONG>List of classes/functions the QmSystem API provides</STRONG>:

<TABLE>
<TR>
   <TH>Name</TH>
   <TH>Content</TH>
</TR>
<TR>
    <TD>MeeGo::QmActivity</TD>
    <TD>provides information about device activity.</TD>
</TR>
<TR>
    <TD>MeeGo::QmBattery</TD>
    <TD>provides information on device battery status.</TD>
</TR>
<TR>
    <TD>MeeGo::QmCABC</TD>
    <TD>provides methods to set and get Content Adaptive Display Control settings.</TD>
</TR>
<TR>
    <TD>MeeGo::QmDeviceMode</TD>
    <TD>provides information and actions on device operation mode and power save mode.</TD>
</TR>
<TR>
    <TD>MeeGo::QmDisplayState</TD>
    <TD>provides information and actions on device display state.</TD>
</TR>
<TR>
    <TD>MeeGo::QmHeartbeat</TD>
    <TD>provides system heartbeat service.</TD>
</TR>
<TR>
    <TD>MeeGo::QmKeys</TD>
    <TD>provides access to hardware key states.</TD>
</TR>
<TR>
    <TD>MeeGo::QmLocks</TD>
    <TD>provides information and actions on device locks.</TD>
</TR>
<TR>
    <TD>MeeGo::QmSystemState</TD>
    <TD>provides information and actions on device state.</TD>
</TR>
<TR>
    <TD>MeeGo::QmThermal</TD>
    <TD>provides information on device thermal states.</TD>
</TR>
<TR>
    <TD>MeeGo::QmTime</TD>
    <TD>provides interface for interacting with system clock.</TD>
</TR>
</TABLE>

@section quickstart Getting started

To get library compiler and linker parameters, use the following:<br/>
QMake: \code CONFIG += qmsystem2 \endcode <br/>
Pkg-Config: \code pkg-config --libs --cflags qmsystem2 \endcode <br/>

@section examples Examples

The following example retrieves the display state to check if the
display is on:

\code
#include <qmdisplaystate.h>

MeeGo::QmDisplayState displayState;

MeeGo::QmDisplayState::DisplayState state = displayState.get();
if (MeeGo::QmDisplayState::On == state) {
   // The display is on!
}
\endcode


In this example, the keyEvent() signal in QmKeys is listened to know
when the power key was pressed.

\code
#include <qmkeys.h>

class MyKeyReceiver : public QObject {
    Q_OBJECT

public Q_SLOTS:
    void keyEvent(MeeGo::QmKeys::Key key, MeeGo::QmKeys::State state) {
        if (MeeGo::QmKeys::PowerKey == key &&
            MeeGo::QmKeys::KeyDown == state) {
            // the power key was pressed!
        }
    }
};

void listenKeyPresses()
{
    MyKeyReceiver keyReceiver;
    MeeGo::QmKeys keys;

    QObject::connect(&keys, SIGNAL(keyEvent(MeeGo::QmKeys::Key, MeeGo::QmKeys::State)),
                     &keyReceiver, SLOT(keyEvent(MeeGo::QmKeys::Key, MeeGo::QmKeys::State)));

}
\endcode

*/
