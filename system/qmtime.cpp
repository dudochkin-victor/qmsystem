/*!
 * @file qmtime.cpp
 * @brief QmTime

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas@nokia.com>
   @author Yang Yang <ext-yang.25.yang@nokia.com>
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
 */

#include <QDBusReply>
#include <QDebug>
#include <QDateTime>

#if !defined(HAVE_QMLOG)
    #define log_debug(...) do {} while (0)
    #define log_info(...) do {} while (0)
    #define log_notice(...) do {} while (0)
    #define log_warning(...) do {} while (0)
    #define log_error(...) do {} while (0)
    #define log_critical(...) do {} while (0)
#else
    #include <qmlog>
#endif

#include "qmtime.h"
#include "qmtime_p.h"

MeeGo::QmTime::QmTime(QObject *parent) : QObject(parent)
{
  p = QmTimePrivate2::get_object() ;

  connect(p, p_signal(), this, signal()) ;
#if F_SUPPORT_DEPRECATED
  connect(p, p_signal_o(), this, signal_o()) ;
#endif
}

MeeGo::QmTime::~QmTime()
{
  disconnect(p, p_signal(), this, signal()) ;
#if F_SUPPORT_DEPRECATED
  disconnect(p, p_signal_o(), this, signal_o()) ;
#endif

  QmTimePrivate2::unref_object() ;
}

QMutex MeeGo::QmTimePrivate2::object_mutex ;
QMutex MeeGo::QmTimePrivate2::TzWrapper::mutex ;

MeeGo::QmTimePrivate2::TzWrapper::TzWrapper(const char *tz) // NULL means "don't change", empty string means "unset"
{
  mutex.lock() ;
  valid = true ;
  set_timezone = tz!=NULL ;
  if (set_timezone)
  {
    saved_tz = getenv("TZ") ?: "" ; // if set and empty, then will be unset at exit
    if (not set_tz_env(tz))
      set_timezone = valid = false ;
  }
}

MeeGo::QmTimePrivate2::TzWrapper::~TzWrapper()
{
  if (set_timezone)
  {
    if (not set_tz_env(saved_tz.c_str()))
      log_critical("can't restore TZ environment '%s'", saved_tz.c_str()) ;
  }
  mutex.unlock() ;
}

bool MeeGo::QmTimePrivate2::TzWrapper::set_tz_env(const char *tz)
{
  bool res ;
  if (*tz=='\0') // empty string -> unset
    res = unsetenv("TZ")==0 ;
  else
    res = setenv("TZ", tz, true)==0 ;
  tzset() ;
  return res ;
}

bool MeeGo::QmTimePrivate2::remote_time(const char *tz, time_t t, QDateTime *qdatetime, struct tm *tm)
{
  MeeGo::QmTimePrivate2::TzWrapper tmp_tz(tz) ;
  if (not tmp_tz.is_valid())
  {
    log_error("can't set TZ environment '%s'", tz ?: "[null]") ;
    return false ;
  }
  struct tm local_tm, *p_tm = tm ?: &local_tm ;
  bool res = localtime_r(&t, p_tm) == p_tm ;
  if (not res)
    return false ;

  if (qdatetime!=NULL)
  {
    QDate qdate(p_tm->tm_year+1900, p_tm->tm_mon+1, p_tm->tm_mday) ;
    QTime qtime(p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec) ;
    *qdatetime = QDateTime(qdate, qtime, Qt::LocalTime) ;
  }

  return true ;
}

bool MeeGo::QmTime::getTimezone(QString &s)
{
  if (p->timed_info_valid)
    s = p->info.humanReadableTz() ;

  return p->timed_info_valid ;
}

bool MeeGo::QmTime::setTimezone(const QString tz)
{
  // Let's explicitly check it: timed will accept an empty string here,
  //   which means: the time zone will be switched to the
  //   last used manual time zone
  // But it seems QmTime user should not submit an empty string
  if (tz.isEmpty())
    return false ;

  Maemo::Timed::WallClock::Settings s;
  s.setTimezoneManual(tz);

  if (not s.check())
    return false;

  QDBusMessage reply = p->timed->wall_clock_settings_sync(s) ;
  QDBusReply<bool> reply_bool = reply ;

  if (not reply_bool.isValid() or not reply_bool.value())
  {
    log_error("can't set timezone, error message: '%s'", reply.errorMessage().toStdString().c_str()) ;
    return false ;
  }

  log_notice("timezome set to '%s'", tz.toStdString().c_str()) ;

  if (p->sync_policy==SynchronizeOnWrite)
  {
    log_notice("syncronizing time settings") ;
    return p->syncronize_timed_info() ;
  }

  return true ;
}

int MeeGo::QmTime::getTimeDiff(time_t t, const QString &tz1, const QString &tz2)
{
  string s1 = tz1.toStdString(), s2 = tz2.toStdString() ;

  struct tm tm1, tm2;
  bool res1 = p->remote_time(s1.c_str(), t, NULL, &tm1) ;

  if (not res1)
  {
    log_error("can't calculate gmt offset for time zone '%s'", s1.c_str()) ;
    return -1 ;
  }

  bool res2 = p->remote_time(s2.c_str(), t, NULL, &tm2) ;

  if (not res2)
  {
    log_error("can't calculate gmt offset for time zone '%s'", s2.c_str()) ;
    return -1 ;
  }

  long diff = tm1.tm_gmtoff- tm2.tm_gmtoff ;

  return diff ;
}

enum MeeGo::QmTime::AutoSystemTimeStatus MeeGo::QmTime::autoSystemTime()
{
  if (not p->timed_info_valid)
    return AutoSystemTimeUnknown ;
  else
    return p->info.flagTimeNitz() ? AutoSystemTimeOn : AutoSystemTimeOff ;
}

bool MeeGo::QmTime::setAutoSystemTime(enum MeeGo::QmTime::AutoSystemTimeStatus new_status)
{
  Maemo::Timed::WallClock::Settings s ;

  if (new_status==AutoSystemTimeOn)
    s.setTimeNitz() ;
  else if (new_status==AutoSystemTimeOff)
    s.setTimeManual() ;
  else
    return false ;

  QDBusMessage reply = p->timed->wall_clock_settings_sync(s) ;
  QDBusReply<bool> reply_bool = reply ;

  if (not reply_bool.isValid() or not reply_bool.value())
  {
    log_error("can't set auto time status, error message: '%s'", reply.errorMessage().toStdString().c_str()) ;
    return false ;
  }

  log_notice("auto time status set to '%s'", new_status==AutoSystemTimeOn ? "on" : "off") ;

  if (p->sync_policy==SynchronizeOnWrite)
  {
    log_notice("syncronizing time settings") ;
    return p->syncronize_timed_info() ;
  }
  else
    return true ;
}

enum MeeGo::QmTime::AutoTimeZoneStatus MeeGo::QmTime::autoTimeZone()
{
  if (not p->timed_info_valid)
    return AutoTimeZoneUnknown ;
  else
    return p->info.flagLocalCellular() ? AutoTimeZoneOn : AutoTimeZoneOff ;
}

bool MeeGo::QmTime::setAutoTimeZone(enum MeeGo::QmTime::AutoTimeZoneStatus new_status)
{
  Maemo::Timed::WallClock::Settings s ;

  if (new_status==AutoTimeZoneOn)
    s.setTimezoneCellular() ;
  else if (new_status==AutoTimeZoneOff)
    s.setTimezoneManual("") ;
  else
    return false ;

  QDBusMessage reply = p->timed->wall_clock_settings_sync(s) ;
  QDBusReply<bool> reply_bool = reply ;

  if (not reply_bool.isValid() or not reply_bool.value())
  {
    log_error("can't set auto time zone status, error message: '%s'", reply.errorMessage().toStdString().c_str()) ;
    return false ;
  }

  log_notice("auto time zone status set to '%s'", new_status==AutoTimeZoneOn ? "on" : "off") ;

  if (p->sync_policy==SynchronizeOnWrite)
  {
    log_notice("syncronizing time settings") ;
    return p->syncronize_timed_info() ;
  }
  else
    return true ;
}

bool MeeGo::QmTime::isOperatorTimeAccessible(bool &result)
{
  if (p->timed_info_valid)
    result = p->info.nitzSupported() ;

  return p->timed_info_valid ;
}

bool MeeGo::QmTime::deviceDefaultTimezone(QString &default_timezone)
{
  if (p->timed_info_valid)
    default_timezone = p->info.defaultTimezone() ;

  return p->timed_info_valid ;
}

MeeGo::QmTimePrivate2* MeeGo::QmTimePrivate2::object = 0 ;

MeeGo::QmTimePrivate2 *MeeGo::QmTimePrivate2::get_object()
{
  QMutexLocker locker(&object_mutex) ;
  if (!object)
    object = new QmTimePrivate2(0) ;
  ++ object->counter ;
  if (not object->initialized)
    object->initialize() ;
  return object ;
}

void MeeGo::QmTimePrivate2::unref_object()
{
  QMutexLocker locker(&object_mutex) ;
  if (!object)
    return ;
  if (--object->counter==0 and object->disconn_policy==MeeGo::QmTime::DisconnectWhenPossible) {
    object->uninitialize() ;
    delete object, object = 0 ;
  }
}

MeeGo::QmTimePrivate2::QmTimePrivate2(QObject *parent) : QObject(parent)
{
  sync_policy = MeeGo::QmTime::WaitForSignal ;
  sync_policy = MeeGo::QmTime::SynchronizeOnWrite ; // this line to be removed later
  disconn_policy = MeeGo::QmTime::KeepConnected ;
  counter = 0 ;
  initialized = false ;
  timed_info_valid = false ;
  timed = NULL ;
}

void MeeGo::QmTimePrivate2::initialize()
{
  // we don't know anything yet
  timed_info_valid = false ;

  // to be notified by timed
  timed = new Maemo::Timed::Interface ;
  bool connected = timed->settings_changed_connect(this, p_slot_timed()) ;

  if (not connected)
    log_error("connection to timed signal failed, message: '%s'", Maemo::Timed::bus().lastError().message().toStdString().c_str()) ;
  else
    log_notice("connected to timed signal") ;

  syncronize_timed_info() ;

  initialized = true ;
}

bool MeeGo::QmTimePrivate2::syncronize_timed_info()
{
  QDBusMessage reply = timed->get_wall_clock_info_sync() ;
  QDBusReply<Maemo::Timed::WallClock::Info> reply_info = reply ;
  if (not reply_info.isValid())
  {
    log_error("synconizing timed info failed: error '%s'", reply.errorMessage().toStdString().c_str()) ;
    return false ;
  }
  else // we have a valid reply
  {
    log_notice("syncronizing timed info: a valid reply received") ;
    process_timed_info(reply_info.value(), false, false) ;
    return true ;
  }
}

void MeeGo::QmTimePrivate2::uninitialize_v2()
{
  initialized = false ;
  if (false)
  {
    bool disconnected = timed->settings_changed_disconnect(this, p_slot_timed()) ;

    if (not disconnected)
      log_error("disconnection from timed signal failed, message: '%s'", Maemo::Timed::bus().lastError().message().toStdString().c_str()) ;
    else
      log_notice("disconnected from timed signal") ;
  }

  if (timed)
    delete timed, timed = NULL ;

  timed_info_valid = false ;
}

void MeeGo::QmTimePrivate2::uninitialize()
{
  initialized = false ;
  bool disconnected = timed->settings_changed_disconnect(this, p_slot_timed()) ;

  if (not disconnected)
    log_error("disconnection from timed signal failed, message: '%s'", Maemo::Timed::bus().lastError().message().toStdString().c_str()) ;
  else
    log_notice("disconnected from timed signal") ;

  if (timed)
    delete timed, timed = NULL ;

  timed_info_valid = false ;
}

bool MeeGo::QmTime::localTime(time_t t, QDateTime &qdatetime, struct tm *tm)
{
  return MeeGo::QmTimePrivate2::remote_time(NULL, t, &qdatetime, tm) ;
}


bool MeeGo::QmTime::remoteTime(QString const &tz, time_t t, QDateTime &qdatetime, struct tm *tm)
{
  bool empty = tz.isEmpty() ;
  const char *p = "" ;
  if (not empty)
  {
    string timezone = (string)":" + tz.toStdString() ;
    p = timezone.c_str() ;
    while(*p!='\0' and p[1]==':') // Make sure we have exactly one ':'
      ++ p ;
  }
  return MeeGo::QmTimePrivate2::remote_time(p, t, &qdatetime, tm) ;
}

MeeGo::QmTimePrivate2::~QmTimePrivate2()
{
  if (counter>0)
    log_warning("memory leak detected: %d QmTime object(s) not deleted", counter) ;
  if (initialized)
    uninitialize_v2() ;
}

void MeeGo::QmTimePrivate2::process_timed_info(const Maemo::Timed::WallClock::Info &info, bool system_time_changed, bool need_signal)
{
  timed_info_valid = true ;
  this->info = info ;
  if (need_signal)
    emit_signal(system_time_changed) ;
}

void MeeGo::QmTimePrivate2::emit_signal(bool systime)
{
  emit change_signal(systime ? MeeGo::QmTime::TimeChanged : MeeGo::QmTime::OnlySettingsChanged) ;
  emit change_signal(systime ? MeeGo::QmTimeTimeChanged : MeeGo::QmTimeOnlySettingsChanged) ;
}
time_t MeeGo::QmTime::getAutoTime()
{
  if (not p->timed_info_valid)
    return (time_t)(-1) ;

  if (not p->info.utcAvailable(Maemo::Timed::WallClock::UtcNitz))
    return (time_t)(-1) ;

  return p->info.utc(Maemo::Timed::WallClock::UtcNitz) ;
}

bool MeeGo::QmTime::getAutoTimezone(QString &tz)
{
  if (not p->timed_info_valid)
    return false ;

  if (not p->info.timezoneAvailable(Maemo::Timed::WallClock::TimezoneCellular))
    return false ;

  tz = p->info.timezone(Maemo::Timed::WallClock::TimezoneCellular) ;

  return true ;
}

#if F_SUPPORT_UNUSED
bool MeeGo::QmTime::getNetTime(QDateTime &time, QString &tz)
{
  time_t t = getAutoTime() ;
  if (t==(time_t)(-1))
    return false ;

  bool res = getAutoTimezone(tz) ;
  if (not res)
    return false ;

  time.setTime_t(t) ;

  return true ;
}
#endif

bool MeeGo::QmTime::setTime(time_t t)
{
  Maemo::Timed::WallClock::Settings s ;
  s.setTimeManual(t) ;

  if (not s.check())
    return false ;

  QDBusMessage reply = p->timed->wall_clock_settings_sync(s) ;
  QDBusReply<bool> reply_bool = reply ;

  if (not reply_bool.isValid() or not reply_bool.value())
  {
    log_error("can't set system time, error message: '%s'", reply.errorMessage().toStdString().c_str()) ;
    return false ;
  }

  log_notice("system time set to '%ld'", t) ;

  if (p->sync_policy==SynchronizeOnWrite)
  {
    log_notice("syncronizing time settings") ;
    return p->syncronize_timed_info() ;
  }

  return true ;

}

bool MeeGo::QmTime::setTime(const QDateTime &time)
{
  return setTime(time.toTime_t()) ;
}

#if F_SUPPORT_UNUSED
bool MeeGo::QmTime::getTZName(QString &s)
{
  struct tm tm ;
  QDateTime dt ;
  if (not localTime(time(NULL), dt, &tm))
    return false ;

  s = tm.tm_zone ;
  return true ;
}
#endif

#if F_SUPPORT_UNUSED
bool MeeGo::QmTime::getRemoteTime(const QDateTime &time, const QString &tz, QDateTime &res)
{
  time_t t = time.toTime_t() ;
  return remoteTime(tz, t, res) ;
}
#endif

#if F_SUPPORT_UNUSED
int MeeGo::QmTime::getUTCOffset(const QString &tz)
{
  QDateTime unused ;
  struct tm tm ;
  if (remoteTime(tz, time(NULL), unused, &tm))
    return tm.tm_gmtoff ;

  return 0 ; // But why???
}
#endif

#if F_SUPPORT_UNUSED
int MeeGo::QmTime::getDSTUsage(const QDateTime &time, const QString &tz)
{
  QDateTime unused ;
  struct tm tm ;
  if (remoteTime(tz, time.toTime_t(), unused, &tm))
    return tm.tm_isdst ;

  return 0 ; // But why???
}
#endif

#if F_SUPPORT_UNUSED
int MeeGo::QmTime::getTimeDiff(const QDateTime &time, const QString &tz1, const QString &tz2)
{
  return getTimeDiff(time.toTime_t(), tz1, tz2) ;
}
#endif

#if F_SUPPORT_UNUSED
// Autosync: the guys are expecting that both time and timezone are from cellular network.
bool MeeGo::QmTime::setAutosync(bool enable)
{
  log_info("This method is deprecated, DO NOT USE IT!") ;
  return setAutoSystemTime(enable ? AutoSystemTimeOn : AutoSystemTimeOff)
    and setAutoTimeZone(enable ? AutoTimeZoneOn : AutoTimeZoneOff) ;
}
#endif

#if F_SUPPORT_UNUSED
int MeeGo::QmTime::getAutosync()
{
  log_info("This method is deprecated, DO NOT USE IT!") ;
  return autoSystemTime() and autoTimeZone() ;
}
#endif

#if F_SUPPORT_UNUSED
int MeeGo::QmTime::isOperatorTimeAccessible()
{
  log_info("This method is deprecated, DO NOT USE IT!") ;
  bool status ;
  return isOperatorTimeAccessible(status) ? status : -1 ;
}
#endif
