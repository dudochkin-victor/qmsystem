/*!
 * @file qmtime.h
 * @brief Contains QmTime which provides interface for interacting with system clock.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

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

#ifndef MEEGO_QMTIME_H
#define MEEGO_QMTIME_H

#include "system_global.h"
#include <QtCore/qobject.h>
#include <QTime>
#include <QDate>
#include <QDateTime>
#include <time.h>

QT_BEGIN_HEADER

namespace MeeGo { class QmTime ; }
namespace MeeGo { class QmTimePrivate2 ; }

/* these macros are temporary, that's why no proper "MEEGO_QMTIME_" prefix */
#define F_SUPPORT_DEPRECATED 1
#define F_SUPPORT_UNUSED     1

#if F_SUPPORT_DEPRECATED
/* to be replaces by MeeGo::QmTime::WhatChanged */
namespace MeeGo
{
  /**
   *
   * Enumeration for change events for QmTime.
   * @deprecated
  */
  enum QmTimeWhatChanged
  {
      QmTimeTimeChanged = 0,      /**< Time (and settings) changed */
      QmTimeOnlySettingsChanged   /**< Settings changed */
  };
  Q_ENUMS(QmTimeWhatChanged);
}
#endif

/**
 * @scope Nokia Meego
 *
 * @brief Provides interface for interacting with system clock.
 *
 * Setting the time should always be done through QmTime to ensure
 * proper signalling to other applications.
 */

class MeeGo::QmTime : public QObject
{
  QmTimePrivate2 *p ;

  Q_OBJECT ;
  Q_ENUMS(TimeFormat);
  Q_ENUMS(WhatChanged) ;
  Q_ENUMS(DisconnectionPolicy) ;
  Q_ENUMS(SettingsSynchronizationPolicy) ;
  Q_ENUMS(AutoSystemTimeStatus) ;
  Q_ENUMS(AutoTimeZoneStatus) ;

public:
  /** Enumeration for change events for QmTime. */
  enum WhatChanged
  {
    TimeChanged,         /**< System time (and possibly settings) changed */
    OnlySettingsChanged  /**< Settings changed, system time is not changed */
  } ;

  /** Policy regarding disconnection from time daemon */
  enum DisconnectionPolicy
  {
    DisconnectWhenPossible, /**< Time/settings change notification will be dropped if no QmTime object exists (default) */
    KeepConnected           /**< The connection will be kept until policy change or process exit */
  } ;

  /** Policy regarding disconnection from time and gconf daemons */
  enum SettingsSynchronizationPolicy
  {
    SynchronizeOnWrite, /**< Every call changing time settings will be followed by settings synchronization D-Bus call */
    WaitForSignal       /**< Settings will be retrieved only after arrival of the D-Bus signal */
  } ;

  /** Status of the automatic system time setting */
  enum AutoSystemTimeStatus
  {
    AutoSystemTimeOff,     /**< System time is set manually by a user */
    AutoSystemTimeOn,      /**< System time is set automatically */
    AutoSystemTimeUnknown  /**< Status unknown */
  } ;

  /** Status of the automatic time zone guess */
  enum AutoTimeZoneStatus
  {
    AutoTimeZoneOff,     /**< Time zone of the device guessed automatically */
    AutoTimeZoneOn,      /**< Time zone of the device is set manually by a user */
    AutoTimeZoneUnknown  /**< Status unknown */
  } ;

  QmTime(QObject *parent=NULL) ;
  virtual ~QmTime() ;

  /**
   * Set the policy of disconnection from daemon signals
   *
   * @note Disconnection policy is defined application-wide
   * @note Default policy is QmTime::KeepConnected
   *
   * @param p <tt>QmTime::DisconnectWhenPossible</tt> if the application is not interested in status updates during its life time
   *          <tt>QmTime::KeepConnected</tt> to reduce amount of D-Bus traffic during QmTime object construction
   *
   * @return true if successfully set
   */
  static bool setDisconnectionPolicy(DisconnectionPolicy p) ;

  /**
   * Set settings synchronization policy
   *
   * @note Synchronization policy is defined application-wide
   * @note Default policy is QmTime::WaitForSignal
   *
   * @param p <tt>QmTime::SynchronizeOnWrite</tt> to execute synchronize() after every call changing settings
   *          <tt>QmTime::WaitForSignal</tt> to avoid it
   *
   * @return true if successfully set
   */
  static bool setSynchronizationPolicy(SettingsSynchronizationPolicy p) ;

  /**
   * Requests settings from time daemon
   *
   * @note Usually you don't have to call this function
   *
   * @return true if successfully done
   */
  bool synchronize() ;

  /**
   * Get the state of automatic system time setting
   *
   * @note   Currently only automatic time setting based on network operator NITZ signal is supported, more time sources may be added in the future
   *
   * @return  AutoSystemTimeOn if enabled, AutoSystemTimeOff if disabled, AutoSystemTimeUnknown on errors
   */
  enum AutoSystemTimeStatus autoSystemTime() ;

  /**
   * Set the state of automatic system time setting
   *
   * @credential timed::TimeControl Resource token required to set the network time autosync mode.
   *
   * @note   This method doesn't affect automatic setting of time zones
   *
   * @note   Currently only automatic time setting based on network operator NITZ signal is supported, more time sources may be added in the future
   *
   * @param  new_status AutoSystemTimeOn to enable, AutoSystemTimeOff to disable
   *
   * @return True if successfully set
   */
  bool setAutoSystemTime(enum AutoSystemTimeStatus new_status) ;

  /**
   * Get the state of automatic time zone guessing
   *
   * @note   Currently only automatic time zone setting based on cellular network information is supported, more time zone sources may be added in the future
   *
   * @return  AutoTimeZoneOn if enabled, AutoTimeZoneOff if disabled, AutoTimeZoneUnknown on errors
   */
  enum AutoTimeZoneStatus autoTimeZone() ;

  /**
   * Set the state of automatic time zone guessing
   *
   * @credential timed::TimeControl Resource token required to set the network time autosync mode.
   *
   * @note   This method doesn't affect automatic setting of system time
   *
   * @note   Currently only automatic time zone setting based on cellular network information is supported, more time zone sources may be added in the future
   *
   * @param  new_status AutoTimeZoneOn to enable, AutoTimeZoneOff to disable
   *
   * @return True if successfully set
   */
  bool setAutoTimeZone(enum AutoTimeZoneStatus new_status) ;

  /**
   * @brief       Set current system and RTC date and time.
   *
   * @credential  timed::TimeControl Resource token required to set the device time.
   *
   * @param  time The new system time.
   *
   * @return      True if time is successfully set.
   */
  bool setTime(time_t time) ;

  /**
   * Set current time zone.
   * @credential timed::TimeControl Resource token required to set the device time zone.
   *
   * @param tz Time zone
   *
   *           The following two formats are accepted:
   *
   *           1) Geographical entry in the Olson database (almost all of them).
   *           2) Zones with a fixed GMT offset as described by ISO 8601
   *
   *           The full list of all valid time zones can be found in
   *           /usr/share/tzdata-timed/zone.alias file
   *           (binary package 'tzdata-timed', source package 'tzdata')
   *
   *           Even if a string is accepted as input for this method, it
   *           doesn't mean that the timezone will be set accordingly: If a
   *           string is just an alias (listed as a non-first word of line in
   *           the above file), then the time zone will be set to the main name
   *           of the time zone (listed as the first word in the same line).
   *
   *           Examples:
   *             setTimezone('Europe/Helsinki') sets the time zone to 'Europe/Helsinki'
   *             setTimezone('Asia/Calcutta') sets the time zone to 'Asia/Kolkata'
   *             setTimezone('Israel') sets the time zone to 'Asia/Jerusalem'
   *             setTimezone('Universal') sets the time zone to 'Iso8601/+0000'
   *             setTimezone('Etc/GMT-3') sets the time zone to 'Iso8601/+0300'
   *             setTimezone('Iso8601/-03:45') sets the time zone to 'Iso8601/-0345'
   *
   *           The above examples are purely informative. The content of the zone.alias reflects
   *           the set of supported time zones, and can be changed with a new tzdata version.
   *
   * @return True if zone was succesfully set, or false if not.
   */
  bool setTimezone(const QString tz) ;

  /**
   * Get current time zone.
   *
   * @param   tz a reference to a string variable to store the result
   *
   *          Returnes a current time zone.
   *          See the description of setTimezone() method above.
   *
   * @return  True if time zone is set
   */
  bool getTimezone(QString &tz) ;

  /**
   * Get automatically guessed time zone
   *
   * @param tz  a reference to a string variable to store the result
   *
   * @note   Currently only automatic time zone guess based on cellular network information is supported, more time zone sources may be added in the future
   *
   * @return true if information succesfully retrieved and stored in the variable
   */
  bool getAutoTimezone(QString &tz) ;

  /**
   * Get time received from automatic time source
   *
   * @return time_t value: amount of seconds after unix epoch begin
   *         (time_t)(-1) if automatic time is not available
   */
  time_t getAutoTime() ;

  /**
   * Get info if the device supports time updates from cellular network operator.
   *
   * @param   a reference to a boolean variable to store the result
   *
   * @return  True if information successfully retrieved
   */
  bool isOperatorTimeAccessible(bool &result) ;

  /**
   * Get default time zone defined during device manufacture
   *
   * @param   default_timezone a reference to a string variable to store the result
   *
   * @return  True if information successfully retrieved
   */
  bool deviceDefaultTimezone(QString &default_timezone) ;

  /**
   * Calculate local time in given timezone
   *
   * @param   tz       the time zone
   *          t        time in seconds from the Unix epoch begin
   *          dt       a reference to variable to save broken down time representation
   *          p        pointer to save struct tm providing low level details, NULL if not needed
   *
   * @return  True if information successfully retrieved
   */
  static bool remoteTime(const QString &tz, time_t t, QDateTime &dt, struct tm *p=NULL) ;

  /**
   * Calculate local time (in current timezone)
   *
   * @param   t        time in seconds from the Unix epoch begin
   *          dt       a reference to variable to save broken down time representation
   *          p        pointer to save struct tm providing low level details, NULL if not needed
   *
   * @return  True if information successfully retrieved
   */
  static bool localTime(time_t t, QDateTime &dt, struct tm *p=NULL) ;

  /**
   * Get time difference between two locations at given moment of time
   *
   * @param t          The moment at which the difference should be calculated.
   * @param location1  The time zone representing the first remote location
   * @param location2  The time zone representing the second remote location
   *
   *                   If either first or second location is empty, the current device timezone is used
   *
   * @return           Local time difference between the locations in seconds,
   *                   positive if the location1 is to the east of location2
   *                   -1 is returned on errors
   */
  int getTimeDiff(time_t t, const QString &location1, const QString &location2) ;

  // obsolete deprecated methods //

#if F_SUPPORT_UNUSED
  /**
   * @brief Deprecated, use getAutoTime() and getAutoTimezone() instead.
   * @deprecated
   */
  bool getNetTime(QDateTime& time, QString& tz);
#endif

#if F_SUPPORT_DEPRECATED
  /**
   * @brief Deprecated, use setTime(time_t) instead.
   * @deprecated
   */
  bool setTime(const QDateTime& time);
#endif

#if F_SUPPORT_UNUSED
  /**
   * @brief Deprecated, use remoteTime() instead.
   * @deprecated
   */
  bool getTZName(QString& s);
#endif

#if F_SUPPORT_UNUSED
  /**
   * @brief Deprecated, use remoteTime() instead.
   * @deprecated
   */
  bool getRemoteTime(const QDateTime &moment, const QString &location, QDateTime &remoteTime);
#endif

#if F_SUPPORT_UNUSED
  /**
   * @brief Deprecated, use remoteTime() instead.
   * @deprecated
   */
  int getUTCOffset(const QString &);
#endif

#if F_SUPPORT_UNUSED
  /**
   * @brief Deprecated, use remoteTime() instead.
   * @deprecated
   */
  int getDSTUsage(const QDateTime &moment, const QString &location) ;
#endif

#if F_SUPPORT_UNUSED
  /**
   * @brief Deprecated, use getTimeDiff(time_t,const QString &,const QString &) instead.
   * @deprecated
   */
  int getTimeDiff(const QDateTime &moment, const QString &location1, const QString &location2) ;
#endif

#if F_SUPPORT_UNUSED
  /**
   * @brief Deprecated, use setAutoSystemTime() and setAutoTimeZone() instead.
   * @deprecated
   */
  bool setAutosync(bool enable);
#endif

#if F_SUPPORT_UNUSED
  /**
   * @brief Deprecated, use autoTimeZone() and autoSystemTime() instead.
   * @deprecated
   */
  int getAutosync();
#endif

#if F_SUPPORT_UNUSED
  /**
   * @brief Deprecated, use isOperatorTimeAccessible(bool &) instead.
   * @deprecated
   */
  int isOperatorTimeAccessible(void);
#endif

Q_SIGNALS:
  /**
   * Sent when system time or settings have changed.
   * @param what What has changed
   */
  void timeOrSettingsChanged(MeeGo::QmTime::WhatChanged what);

#if F_SUPPORT_DEPRECATED
  /*
   * @brief Deprecated, use timeOrSettingsChanged(MeeGo::QmTime::WhatChanged) instead.
   * @deprecated
   */
  void timeOrSettingsChanged(MeeGo::QmTimeWhatChanged what);
#endif

private:
    Q_DISABLE_COPY(QmTime)
};

QT_END_HEADER

#endif // MEEGO_QMTIME_H
