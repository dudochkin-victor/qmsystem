/*!
 * @file qmheartbeat.h
 * @brief Contains QmHeartbeat which provides system heartbeat services.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>

   @scope Nokia Meego

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
#ifndef QMHEARTBEAT_H
#define QMHEARTBEAT_H
#include <QtCore/qobject.h>
#include <QtCore/qdatetime.h>

#include "system_global.h"

QT_BEGIN_HEADER

namespace MeeGo {

class QmHeartbeatPrivate;

/*!
 * @scope Nokia Meego
 *
 * @class QmHeartbeat
 * @brief QmHeartbeat provides the system heartbeat service.
 * @details System heartbeat is a service for applications to synchronize their
 * activities to save the battery use time. 
 * <br>
 * The main idea is that applications that must do periodic activity – after being
 * in sleep a certain period – do that at the same time: for example 
 * send network “alive” messages at the same time (for example turn the wireless radio on 
 * at the same time). 
 * <br>
 * The service is not only for network-aware applications: in fact it must be used 
 * by any applications that need to periodic wake-ups.
 * <br>
 * Note: the Qt Mobility System Information API provides the class <A HREF="http://doc.qt.nokia.com/qt-mobility-snapshot/qsystemalignedtimer.html">QSystemAlignedTimer</a>
*  with similar functionality.
 */
class QmHeartbeat : public QObject
{
    Q_OBJECT
    Q_ENUMS(SignalNeed)
    Q_ENUMS(WaitMode)

public:
    //! Signal needs
    enum SignalNeed
    {
        NoSignalNeeded = 0,      //!< No signal needed
        SignalNeeded             //!< Signal is needed
    };

    //! Wait modes
    enum WaitMode
    {
        DoNotWaitHeartbeat = 0,  //!< Do not wait for heart beat, use the wakeUp signal instead
        WaitHeartbeat            //!< Wait for heart beat
    };

public:
    /*!
     * @brief Constructor
     * @param parent The parent object
     */
    QmHeartbeat(QObject *parent = 0);
    ~QmHeartbeat();


    /*!
     * @brief Opens the heartbeat service.
     * @param signalNeed True if wakeUp signal is to be used
     * @return True if success
     */
    bool open(QmHeartbeat::SignalNeed signalNeed);


    /*!
     * @brief Closes the heartbeat service.
     */
    void close(void);


    /*!
     * @brief Gets the file descriptor for heartbeat (for use with QSocketNotifier)
     * @return Descriptor that can be used for QSocketNotifier, -1 if error (check errno)
     */
    int  getFD();

    /*!
     * @brief Waits for the next heartbeat.
     *
     * @param mintime   Time in seconds that MUST be waited before heartbeat is reacted to.
     *                  Value 0 means 'wake me up when someboy else is woken'. 
     *                  It  is recommended that the first wait (if possible) uses minvalue as 0 to "jump to the train"
     * @param maxtime   Time in seconds when the wait MUST end. It is wise to have maxtime-mintime
     *                  quite big so all users of this service get synced.
     *                  For example if you preferred wait is 120 seconds, use minval 110 and maxval 130.
     * @param wait      WaitHeartbeat if this method waits for heartbeat, DoNotWaitHeartbeat 
     *                  if the wakeUp signal or file descriptor for QSocketNotifier is used
     *
     * @return          Time waited
    */
    QTime wait(unsigned short mintime, unsigned short maxtime, QmHeartbeat::WaitMode wait);



    /*!
     * @brief Called if the application woke up by itself.
     * @details This method should be called if the application
     * has woken up by some other method than via system heartbeat
     * to prevent unnecessary wakeup signals.
     *
     * @return		True if success, false if error
    */
    bool  IWokeUp(void);

Q_SIGNALS:
    /*!
     * @brief Signals the wake up.
     * @param time The wakeup time
     */
    void wakeUp(QTime time);

private:
    Q_DISABLE_COPY(QmHeartbeat)
    MEEGO_DECLARE_PRIVATE(QmHeartbeat)
};

} // MeeGo namespace

QT_END_HEADER

#endif /* QMHEARTBEAT_H */

// End of file
