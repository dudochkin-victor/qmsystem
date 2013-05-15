/*!
 * @file qmtime_p.h
 * @brief Contains QmTimePrivate

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Yang Yang <ext-yang.25.yang@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

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
#ifndef QMTIME_P_H
#define QMTIME_P_H

#include <string>
using namespace std ;

#include <QDebug>
#include <QMutex>
#include <timed/interface>

#include "qmtime.h"

// Signal names
static inline const char *p_signal() { return SIGNAL(change_signal(MeeGo::QmTime::WhatChanged)) ; }
static inline const char *signal() { return SIGNAL(timeOrSettingsChanged(MeeGo::QmTime::WhatChanged)) ; }

// Obsolete signal names
#if F_SUPPORT_DEPRECATED
static inline const char *p_signal_o() { return SIGNAL(change_signal(MeeGo::QmTimeWhatChanged)) ; }
static inline const char *signal_o() { return SIGNAL(timeOrSettingsChanged(MeeGo::QmTimeWhatChanged)) ; }
#endif

// Slot name
static inline const char *p_slot_timed() { return SLOT(timed_signal(const Maemo::Timed::WallClock::Info &, bool)) ; }

namespace MeeGo { class QmTimePrivate2 ; class QmTime ; }

class MeeGo::QmTimePrivate2 : public QObject
{
  Q_OBJECT ;
  friend class MeeGo::QmTime ;

  QmTimePrivate2(QObject *parent) ;
  virtual ~QmTimePrivate2() ;

  // objects
  static QmTimePrivate2* object ;
  static QMutex object_mutex ;
  static QmTimePrivate2 *get_object() ;
  static void unref_object() ;

  int counter ;
  bool initialized ;

  void initialize() ;
  void uninitialize() ;
  void uninitialize_v2() ;
  void first_run_init() ;

  // Policies
  QmTime::SettingsSynchronizationPolicy sync_policy ;
  QmTime::DisconnectionPolicy disconn_policy ;

  // Cached values
  bool timed_info_valid ;
  Maemo::Timed::WallClock::Info info ;

  // Timed stuff
  Maemo::Timed::Interface *timed ;
  void process_timed_info(const Maemo::Timed::WallClock::Info &wc_info, bool system_time, bool need_signal) ;
  bool syncronize_timed_info() ;

  // helpers
  class TzWrapper ;
  static bool remote_time(const char *tz, time_t t, QDateTime *qdt, struct tm *tm) ;

  void emit_signal(bool) ;
signals:
#if F_SUPPORT_DEPRECATED
  void change_signal(MeeGo::QmTimeWhatChanged) ;
#endif
  void change_signal(MeeGo::QmTime::WhatChanged) ;
private slots:
  void timed_signal(const Maemo::Timed::WallClock::Info &wc_info, bool system_time_changed) { process_timed_info(wc_info, system_time_changed, true) ; }
} ;

class MeeGo::QmTimePrivate2::TzWrapper
{
  static QMutex mutex ;
  string saved_tz ;
  bool set_timezone ;
  bool valid ;
  static bool set_tz_env(const char *tz) ;
public:
  TzWrapper(const char *tz) ;
 ~TzWrapper() ;
  bool is_valid() { return valid ; }
} ;

#endif // QMTIME_P_H
