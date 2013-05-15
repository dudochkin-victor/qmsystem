/**
 * @file time.cpp
 * @brief QmTime tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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
#include <string>
#include <QObject>
#include <qmtime.h>
#include <QTest>
#include <QDebug>

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

using namespace std;
using namespace MeeGo;

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent), signalReceived(false) {}

    bool signalReceived;
    QmTimeWhatChanged whatChanged;

public slots:
    void timeOrSettingsChanged(MeeGo::QmTimeWhatChanged whatChanged) {
        qDebug() << "timeOrSettingsChanged:"<<whatChanged;
        signalReceived = true;
        this->whatChanged = whatChanged;
    }
};

void waitTwice(int ms) {
    QTest::qWait(ms);
    QTest::qWait(ms);
}

struct testTZStruct {
    QString tz;
    QString tzname;
    int secsTo;
    QString zone; // the main Olson name of the time zone (if 'tz' is just an alias)
};

class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmTime *time;
    SignalDump signalDump;

    QDateTime oldTime;
    QString oldTz;
#if F_TIME_FORMAT
    QmTime::TimeFormat oldTimeFormat;
#endif

    static struct pre_init_t // do it before main()
    {
      pre_init_t() {
          #if defined(HAVE_QMLOG)
          new qmlog::log_file("/var/log/time-test.log") ;
          #endif
      }
    } pre_init ;
private slots:
    void initTestCase() {
        string name = qApp->arguments().join("\\").toStdString() ;
        #if defined(HAVE_QMLOG)
        qmlog::process_name(name.c_str()) ;
        #endif
        time = new MeeGo::QmTime();
        QVERIFY(time);
        oldTime = QDateTime::currentDateTime();
        QVERIFY(time->getTimezone(oldTz));
#if F_TIME_FORMAT
        oldTimeFormat = time->getTimeFormat();
#endif
        QVERIFY(connect(time, SIGNAL(timeOrSettingsChanged(MeeGo::QmTimeWhatChanged)), &signalDump, SLOT(timeOrSettingsChanged(MeeGo::QmTimeWhatChanged))));
    }

    void testGetNetTime() {
        bool ret = true;
        // TODO : to simulate in case that autosync is not
        // enabled and a network time change indication has received)
        //ret = time->getNetTime(t, tz);
        QVERIFY(ret);
    }

    void testSetTime() {
        QDateTime dateTime = QDateTime::fromString("M1d1y9800:01:02",
                                             "'M'M'd'd'y'yyhh:mm:ss");
        signalDump.signalReceived = false;
        bool ret = time->setTime(dateTime);
        QVERIFY(ret);
        waitTwice(500);
        QVERIFY(signalDump.signalReceived);
        QVERIFY(signalDump.whatChanged == QmTimeTimeChanged);

        QVERIFY(dateTime.secsTo(QDateTime::currentDateTime()) <= 3);
    }


    void testSetGetTimezone() {
        // First init to "dummy"
        QVERIFY(time->setTimezone("Asia/Thimphu"));
        waitTwice(500);

        // Then change to Helsinki, so we get the change
        signalDump.signalReceived = false;
        QString tz = "Europe/Helsinki";
        QVERIFY(time->setTimezone(tz));
        waitTwice(500);
        QVERIFY(signalDump.signalReceived);
        // By a time zone change system time is NOT changing
        QVERIFY(signalDump.whatChanged == QmTimeOnlySettingsChanged);

        QString s;
        QVERIFY(time->getTimezone(s));
        QCOMPARE(s, tz);

        QDateTime dateTime = QDateTime::fromString("M1d1y9800:01:02",
                                             "'M'M'd'd'y'yyhh:mm:ss");
        signalDump.signalReceived = false;
        bool ret = time->setTime(dateTime);
        QVERIFY(ret) ;
        waitTwice(500);
        QVERIFY(signalDump.signalReceived);
        QVERIFY(signalDump.whatChanged == QmTimeTimeChanged);
#if F_SUPPORT_UNUSED
        //QVERIFY(time->getTZName(s));
        //QCOMPARE(s, QString("EET"));
#else
        struct tm tm ;
        QDateTime dt ;
        QVERIFY(time->remoteTime(s, ::time(NULL), dt, &tm)) ;
        QCOMPARE((QString)tm.tm_zone, (QString)"EET") ;
#endif
    }

    void testSetGetTimeFormat() {
        log_notice("Skipping 12/24h test %s", __PRETTY_FUNCTION__) ;
#if 0
        MeeGo::QmTime::TimeFormat tf = time->getTimeFormat();
        QVERIFY((tf == QmTime::format24h) || (tf == QmTime::format12h));
        qDebug() <<"Current format is "<<tf;

        //qDebug() <<"Wait for gconftool to change the time format key";
        //waitTwice(10000);
        signalDump.signalReceived = false;
        if (tf == QmTime::format12h) {
            QVERIFY(time->setTimeFormat(QmTime::format24h));
        } else {
            QVERIFY(time->setTimeFormat(QmTime::format12h));
        }

        qDebug() <<"Format is changed to "<<time->getTimeFormat();
        waitTwice(600);
        QVERIFY(signalDump.signalReceived);
        QVERIFY(signalDump.whatChanged == QmTimeOnlySettingsChanged);
        QVERIFY(time->getTimeFormat() != tf);
#endif
    }

    int get_utc_offset_now(QmTime *t, const char *location)
    {
        struct tm tm ;
        QDateTime dt ;
        bool ok = t->remoteTime(location, ::time(NULL), dt, &tm) ;
        if (not ok)
            return -1 ;
        return (int) tm.tm_gmtoff ;
    }

    void testGetUTCOffset() {

        QVERIFY(time->setTimezone("UTC"));
        QDateTime dateTime(QDate(2000, 1, 1));
        QVERIFY(time->setTime(dateTime));

#if F_SUPPORT_UNUSED
        QCOMPARE(time->getUTCOffset(":Europe/Helsinki"), 2*60*60);
        QCOMPARE(time->getUTCOffset(":US/Central"), -6*60*60);
        QCOMPARE(time->getUTCOffset(":Australia/Melbourne"), 11*60*60);
        QCOMPARE(time->getUTCOffset(":America/Sao_Paulo"), -2*60*60);
#else
        const int hours = 3600 ;
        QCOMPARE(get_utc_offset_now(time, "Europe/Helsinki"), 2*hours);
        QCOMPARE(get_utc_offset_now(time, "US/Central"), -6*hours);
        QCOMPARE(get_utc_offset_now(time, "Australia/Melbourne"), 11*hours);
        QCOMPARE(get_utc_offset_now(time, "America/Sao_Paulo"), -2*hours);
#endif

        dateTime.setDate(QDate(2000, 6, 6));
        QVERIFY(time->setTime(dateTime));

#if F_SUPPORT_UNUSED
        QCOMPARE(time->getUTCOffset(":Europe/Helsinki"), 3*60*60);
        QCOMPARE(time->getUTCOffset(":US/Central"), -5*60*60);
        QCOMPARE(time->getUTCOffset(":Australia/Melbourne"), 10*60*60);
        QCOMPARE(time->getUTCOffset(":America/Sao_Paulo"), -3*60*60);
#else
        QCOMPARE(get_utc_offset_now(time, "Europe/Helsinki"), 3*hours);
        QCOMPARE(get_utc_offset_now(time, "US/Central"), -5*hours);
        QCOMPARE(get_utc_offset_now(time, "Australia/Melbourne"), 10*hours);
        QCOMPARE(get_utc_offset_now(time, "America/Sao_Paulo"), -3*hours);
#endif
    }

    int get_dst_usage(QmTime *t, time_t at, QString location)
    {
        struct tm tm ;
        QDateTime dt ;
        bool ok = t->remoteTime(location, at, dt, &tm) ;
        if (not ok)
            return -1 ;
        return (int) tm.tm_isdst ;
    }

    void testGetDSTUsage() {
#if F_SUPPORT_UNUSED
        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 1, 1)), ":Europe/Helsinki"), 0);
        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 7, 7)), ":Europe/Helsinki"), 1);
        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 1, 1)), ":Australia/Melbourne"), 1);
        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 7, 7)), ":Australia/Melbourne"), 0);
        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 1, 1)), ":America/Sao_Paulo"), 1);
        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 7, 7)), ":America/Sao_Paulo"), 0);
#else
        time_t jan_2000 = 946722896 ; // Sat Jan  1 12:34:56 EET 2000
        time_t jul_2000 = 962444096 ; // Sat Jul  1 12:34:56 EEST 2000
        QCOMPARE(get_dst_usage(time, jan_2000, "Europe/Helsinki"), 0);
        QCOMPARE(get_dst_usage(time, jul_2000, "Europe/Helsinki"), 1);
        QCOMPARE(get_dst_usage(time, jan_2000, "Australia/Melbourne"), 1);
        QCOMPARE(get_dst_usage(time, jul_2000, "Australia/Melbourne"), 0);
        QCOMPARE(get_dst_usage(time, jan_2000, "America/Sao_Paulo"), 1);
        QCOMPARE(get_dst_usage(time, jul_2000, "America/Sao_Paulo"), 0);
#endif
    }

    void testSetAutosync() {
        signalDump.signalReceived = false;
#if F_SUPPORT_UNUSED
        bool ret = time->setAutosync(true);
#else
        bool ret1 = time->setAutoSystemTime(MeeGo::QmTime::AutoSystemTimeOn) ;
        bool ret2 = time->setAutoTimeZone(MeeGo::QmTime::AutoTimeZoneOn) ;
        bool ret = ret1 and ret2 ;
#endif
        QVERIFY(ret);
#if F_SUPPORT_UNUSED
        ret = time->setAutosync(false);
#else
        ret1 = time->setAutoSystemTime(MeeGo::QmTime::AutoSystemTimeOff) ;
        ret2 = time->setAutoTimeZone(MeeGo::QmTime::AutoTimeZoneOff) ;
        ret = ret1 and ret2 ;
#endif
        QVERIFY(ret);
        waitTwice(500);
        QVERIFY(signalDump.signalReceived);
    }

    void testGetAutosync() {
#if F_SUPPORT_UNUSED
        int ret = time->getAutosync();
        QVERIFY(ret != -1);
#else
        bool ret1 = time->autoSystemTime() != MeeGo::QmTime::AutoSystemTimeUnknown ;
        QVERIFY(ret1);
        bool ret2 = time->autoTimeZone() != MeeGo::QmTime::AutoTimeZoneUnknown ;
        QVERIFY(ret2);
#endif
    }

    void testSetGetAutosync() {
#if F_SUPPORT_UNUSED
        QVERIFY(time->setAutosync(true));
        waitTwice(10*1000);
        QCOMPARE(time->getAutosync(), 1);
        QVERIFY(time->setAutosync(false));
        waitTwice(10*1000);
        QCOMPARE(time->getAutosync(), 0);
#else
        QVERIFY(time->setAutoSystemTime(MeeGo::QmTime::AutoSystemTimeOn));
        QVERIFY(time->setAutoTimeZone(MeeGo::QmTime::AutoTimeZoneOn));
        waitTwice(1000);
        QCOMPARE(time->autoSystemTime(), MeeGo::QmTime::AutoSystemTimeOn);
        QCOMPARE(time->autoTimeZone(), MeeGo::QmTime::AutoTimeZoneOn);

        QVERIFY(time->setAutoSystemTime(MeeGo::QmTime::AutoSystemTimeOff));
        QVERIFY(time->setAutoTimeZone(MeeGo::QmTime::AutoTimeZoneOff));
        waitTwice(1000);
        QCOMPARE(time->autoSystemTime(), MeeGo::QmTime::AutoSystemTimeOff);
        QCOMPARE(time->autoTimeZone(), MeeGo::QmTime::AutoTimeZoneOff);
#endif
    }

    void testIsOperatorTimeAccessible() {
#if F_SUPPORT_UNUSED
        int ret = time->isOperatorTimeAccessible();
        QVERIFY(ret != -1);
#else
        bool result, ret = time->isOperatorTimeAccessible(result) ;
        QVERIFY(ret) ;
#endif
    }

    /** The following tests are from old dms-tests.
     *  TODO:
     *
     *  test 120050 does not make sense, as there is no time_mktime() in QmTime?
     *
     *  test 120080 is not probably applicable, because formatting
     *  should be done with QDateTime? Why QmTime has TimeFormats?
     *
     *  test 120090 does not make sense, as there is no time_get_synced() in QmTime?
     */
    void testSetTime120010() {
        QDateTime current = QDateTime::currentDateTime();
        QVERIFY(time->setTime(current.addSecs(5)));
        //waitTwice(500);
        QDateTime current2 = QDateTime::currentDateTime();
        int secsTo = current.secsTo(current2);
        QString str = QString("Time difference: %1").arg(secsTo);
        QVERIFY2(secsTo >= 4, str.toAscii().data());
        QVERIFY2(secsTo <= 5, str.toAscii().data());
    }

    QString  get_zone_abbreviation_now(QmTime *t, QString location)
    {
        struct tm tm ;
        QDateTime dt ;
        bool ok = t->remoteTime(location, ::time(NULL), dt, &tm) ;
        if (not ok)
            return "N/A" ;
        return (QString) tm.tm_zone ;
    }


    // TODO: GMT* timezones do not work with libtimed0?
    void testSetTimezone120020() {
        QVERIFY(time->setTimezone("UTC"));
        QDateTime dateTime(QDate(2000, 1, 1));
        QVERIFY(time->setTime(dateTime));

        // testTZStruct: { QString tz; QString tzname; int secsTo; QString zone; }
        struct testTZStruct tzs[] = { //{"GMT-2GMT-3,0,365", "GMT", 3*60*60},
                                      //{"GMT+8", "GMT", -8*60*60},
                                      //{"GMT-10:30", "GMT", 10*60*60 + 30*60},
                                      {"Africa/Algiers", "CET", 60*60, ""},
                                      {"Africa/Cairo", "EET", 2*60*60, ""},
                                      {"Africa/Casablanca", "WET", 0, ""},
                                      {"Asia/Shanghai", "CST", 8*60*60, ""},
                                      {"Asia/Tokyo", "JST", 9*60*60, ""},
                                      {"Australia/Melbourne", "EST", 11*60*60, ""}, // DST on southern summer
                                      {"Canada/Central", "CST", -6*60*60, "America/Winnipeg"},
                                      {"Europe/Helsinki", "EET", 2*60*60, ""},
                                      {"Europe/London", "GMT", 0, ""},
                                      {"Europe/Moscow", "MSK", 3*60*60, ""},
                                      {"Europe/Paris", "CET", 1*60*60, ""},
                                      {"Pacific/Honolulu", "HST", -10*60*60, ""},
                                      {"US/Alaska", "AKST", -9*60*60, "America/Anchorage"},
                                      {"US/Central", "CST", -6*60*60, "America/Chicago"},
                                      {"US/Mountain", "MST", -7*60*60, "America/Denver"},
                                      {"US/Pacific", "PST", -8*60*60, "America/Los_Angeles"}};

        for (unsigned i=0; i < sizeof(tzs)/sizeof(*tzs); i++) {

            QVERIFY2(time->setTimezone(tzs[i].tz), tzs[i].tz.toAscii().data());

            QString tz2;
            QVERIFY2(time->getTimezone(tz2), tzs[i].tz.toAscii().data());
            QCOMPARE(tzs[i].zone.isEmpty() ? tzs[i].tz : tzs[i].zone, tz2);
            qDebug()<<tz2;

#if F_SUPPORT_UNUSED
            //QVERIFY2(time->getTZName(tz2), tzs[i].tz.toAscii().data());
#else
            QVERIFY(get_zone_abbreviation_now(time, tz2) == tzs[i].tzname);
#endif
            QString abbreviation = get_zone_abbreviation_now(time, tz2) ;
            log_debug("tsz[%d].tzname='%s' tz2=='%s', abbreviation='%s'", i, tzs[i].tzname.toStdString().c_str(), tz2.toStdString().c_str(), abbreviation.toStdString().c_str()) ;

            QCOMPARE(tzs[i].tzname, abbreviation);

            int diff = dateTime.secsTo(QDateTime::currentDateTime());
            QVERIFY2(diff <= tzs[i].secsTo + 5, tzs[i].tz.toAscii());
            QVERIFY2(diff >= tzs[i].secsTo - 5, tzs[i].tz.toAscii());

        }
    }

    void testSetWrongTimezone120021() {
        QVERIFY(!time->setTimezone("MT-8"));
        QVERIFY(!time->setTimezone("CC"));
    }

    void testMakeAndCheckTime120030() {


        QVERIFY(time->setTimezone("Europe/Helsinki"));
        waitTwice(500);

        /* Set time to 1.7.2008 14:00 and DST on*/
        struct tm tm;
        tm.tm_sec = 0;          /* seconds [0,61] */
        tm.tm_min = 0;          /* minutes [0,59] */
        tm.tm_hour = 14;        /* hour [0,23] */
        tm.tm_mday = 1;         /* day of month [1,31] */
        tm.tm_mon = 6;          /* month of year [0,11] */
        tm.tm_year = 108;       /* years since 1900 */
        tm.tm_wday = 2;         /* day of week [0,6] (Sunday = 0) */
        tm.tm_isdst = 1;        /* daylight savings flag */
        QDateTime dateTime = QDateTime::fromTime_t(mktime(&tm));
        QVERIFY(time->setTime(dateTime));

        QDateTime utc = QDateTime::currentDateTime().toUTC();
        QCOMPARE(utc.time().hour(), 11);

        QDateTime current = QDateTime::currentDateTime();
        QVERIFY(dateTime.secsTo(current) <= 2);
        QVERIFY(dateTime.secsTo(current) >= -2);

        QString tz;
        QVERIFY(time->getTimezone(tz));
#if F_SUPPORT_UNUSED
        QCOMPARE(time->getDSTUsage(current, tz), 1);
#else
        QCOMPARE(get_dst_usage(time, ::time(NULL), tz.toStdString().c_str()), 1);
#endif
    }

    void testCheckRemoteTime120040() {
        QString timezone = "Europe/Helsinki";
        QVERIFY(time->setTimezone(timezone));
        /* Set time to 1.7.2008 14:00 and DST on*/
        struct tm tm;
        tm.tm_sec = 0;          /* seconds [0,61] */
        tm.tm_min = 0;          /* minutes [0,59] */
        tm.tm_hour = 14;        /* hour [0,23] */
        tm.tm_mday = 1;         /* day of month [1,31] */
        tm.tm_mon = 6;          /* month of year [0,11] */
        tm.tm_year = 108;       /* years since 1900 */
        tm.tm_wday = 2;         /* day of week [0,6] (Sunday = 0) */
        tm.tm_isdst = 1;        /* daylight savings flag */
        QDateTime dateTime = QDateTime::fromTime_t(mktime(&tm));
        QVERIFY(time->setTime(dateTime));

        QDateTime remoteTime;
#if F_SUPPORT_UNUSED
        QVERIFY(time->getRemoteTime(QDateTime::currentDateTime(), ":Europe/Paris", remoteTime));
#else
        QVERIFY(time->remoteTime("Europe/Paris", ::time(NULL), remoteTime));
#endif
        QCOMPARE(remoteTime.time().hour(), 13);

#if F_SUPPORT_UNUSED
        QVERIFY(time->getRemoteTime(QDateTime::currentDateTime(), ":Europe/London", remoteTime));
#else
        QVERIFY(time->remoteTime("Europe/London", ::time(NULL), remoteTime));
#endif
        QCOMPARE(remoteTime.time().hour(), 12);

#if F_SUPPORT_UNUSED
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QVERIFY(time->getRemoteTime(currentDateTime.addSecs(60*60), "Europe/London", remoteTime));
#else
        QVERIFY(time->remoteTime("Europe/London", ::time(NULL)+3600, remoteTime));
#endif
        QCOMPARE(remoteTime.time().hour(), 13);

        QString tz;
        QVERIFY(time->getTimezone(tz));
        QCOMPARE(tz, timezone);
#if F_SUPPORT_UNUSED
        QCOMPARE(time->getDSTUsage(QDateTime::currentDateTime(), tz), 1);
        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 1, 1)), tz), 0);
#else
        time_t jan_2000 = 946722896 ; // Sat Jan  1 12:34:56 EET 2000
        QCOMPARE(get_dst_usage(time, ::time(NULL), tz), 1);
        QCOMPARE(get_dst_usage(time, jan_2000, tz), 0);
#endif
    }

    void testGetTimeDiff120051() {
        time->setTimezone(":Europe/Helsinki");
#if F_SUPPORT_UNUSED
        int diff = time->getTimeDiff(QDateTime::currentDateTime(), ":Europe/Helsinki", ":Europe/London");
#else
        int diff = time->getTimeDiff(::time(NULL), "Europe/Helsinki", "Europe/London");
#endif
        QCOMPARE(diff, 7200);
    }

    void testGetTimeDiffDST120052() {
        struct tm tm;
        /* 10.3.2009 14:00 and DST on */
        tm.tm_sec = 0;          /* seconds [0,61] */
        tm.tm_min = 0;          /* minutes [0,59] */
        tm.tm_hour = 14;        /* hour [0,23] */
        tm.tm_mday = 10;        /* day of month [1,31] */
        tm.tm_mon = 2;          /* month of year [0,11] */
        tm.tm_year = 109;       /* years since 1900 */
        tm.tm_wday = 2;         /* day of week [0,6] (Sunday = 0) */
        tm.tm_isdst = 1;        /* daylight savings flag */
        QDateTime t = QDateTime::fromTime_t(mktime(&tm));

#if F_SUPPORT_UNUSED
        int diff = time->getTimeDiff(t, ":Europe/Helsinki", ":GMT");
#else
        time_t tt = t.toTime_t() ;
        int diff = time->getTimeDiff(tt, "Europe/Helsinki", "GMT");
#endif
        QCOMPARE(diff, 7200);

#if F_SUPPORT_UNUSED
        t = t.addDays(30);
        diff = time->getTimeDiff(t, ":Europe/Helsinki", ":GMT");
#else
        time_t days = 24*60*60 ;
        diff = time->getTimeDiff(tt+30*days, "Europe/Helsinki", "GMT");
#endif
        QCOMPARE(diff, 10800);
    }

    void testAutosync120060() {
#if F_SUPPORT_UNUSED
        QVERIFY(time->setAutosync(false));
        QCOMPARE(time->getAutosync(), 0);
        QVERIFY(time->setAutosync(true));
        QCOMPARE(time->getAutosync(), 1);
        QVERIFY(time->setAutosync(false));
        QCOMPARE(time->getAutosync(), 0);
#else
        QVERIFY (time->setAutoSystemTime(MeeGo::QmTime::AutoSystemTimeOff));
        QCOMPARE(time->autoSystemTime(), MeeGo::QmTime::AutoSystemTimeOff);
        QVERIFY (time->setAutoSystemTime(MeeGo::QmTime::AutoSystemTimeOn ));
        QCOMPARE(time->autoSystemTime(), MeeGo::QmTime::AutoSystemTimeOn );
        QVERIFY (time->setAutoSystemTime(MeeGo::QmTime::AutoSystemTimeOff));
        QCOMPARE(time->autoSystemTime(), MeeGo::QmTime::AutoSystemTimeOff);

        QVERIFY (time->setAutoTimeZone(MeeGo::QmTime::AutoTimeZoneOff));
        QCOMPARE(time->autoTimeZone(), MeeGo::QmTime::AutoTimeZoneOff);
        QVERIFY (time->setAutoTimeZone(MeeGo::QmTime::AutoTimeZoneOn ));
        QCOMPARE(time->autoTimeZone(), MeeGo::QmTime::AutoTimeZoneOn );
        QVERIFY (time->setAutoTimeZone(MeeGo::QmTime::AutoTimeZoneOff));
        QCOMPARE(time->autoTimeZone(), MeeGo::QmTime::AutoTimeZoneOff);
#endif
    }

    // XXX: it's a copy of testIsOperatorTimeAccessible
    void testOperatorTime120070() {
#if F_SUPPORT_UNUSED
        int ret = time->isOperatorTimeAccessible();
        QVERIFY(ret == 0 || ret == 1);
#else
        bool result, ret = time->isOperatorTimeAccessible(result) ;
        QVERIFY(ret) ; // call not failed
#endif
    }

    // There is no libtime time_format_time() in QmTime.
    void testTimeFormat120080() {
        log_notice("Skipping 12/24h test %s", __PRETTY_FUNCTION__) ;
#if 0
        log_debug("setting to Format_12") ;
        QVERIFY(time->setTimeFormat(QmTime::format12h));
        log_debug("checking Format_12") ;
        QCOMPARE(time->getTimeFormat(), QmTime::format12h);
        log_debug("setting to Format_24") ;
        QVERIFY(time->setTimeFormat(QmTime::format24h));
        log_debug("checking Format_24") ;
        QCOMPARE(time->getTimeFormat(), QmTime::format24h);
        log_debug("setting to Format_12") ;
        QVERIFY(time->setTimeFormat(QmTime::format12h));
        log_debug("setting to Format_12") ;
        QCOMPARE(time->getTimeFormat(), QmTime::format12h);
#endif
    }

    // TESTCASES FROM OLD clocktest.c

    // TODO: MT is not in /usr/share/zoneinfo
    // TODO: timezone EST+5EDT,M4.1.0/2,M10.5.0/2 is not supported?
    void test_clocktest_start() {
        QVERIFY(!time->setTimezone(""));
        QVERIFY(!time->setTimezone("1"));
        QVERIFY(!time->setTimezone("AA"));
        QVERIFY(!time->setTimezone("BB-"));
        QVERIFY(!time->setTimezone("MT-8"));

#if F_SUPPORT_UNUSED
        QCOMPARE(time->getTimeDiff(QDateTime::currentDateTime(), ":Europe/Helsinki", ":Europe/London"), 7200);
#else
        QCOMPARE(time->getTimeDiff(::time(NULL), "Europe/Helsinki", "Europe/London"), 7200) ;
#endif
        /*
        QCOMPARE(time->getTimeDiff(QDateTime::currentDateTime(), "GMT-2", "MT+2"), 0);
        QCOMPARE(time->getTimeDiff(QDateTime::currentDateTime(), "GMT+3", "MT+3"), -6*3600);
        */

        QVERIFY(time->setTime(QDateTime::fromTime_t(1221472803)));

        /*
        QString setTimezone = "EST+5EDT,M4.1.0/2,M10.5.0/2";
        QVERIFY(time->setTimezone(setTimezone));
        QString gotTimezone;
        QVERIFY(time->getTimezone(gotTimezone));
        QCOMPARE(setTimezone, gotTimezone);
        */
    }


    // TODO: the loop test will fail without a//waitTwice(500) call in the clocktest_loop.
    void test_clocktest_loop() {
        for (int i=0; i < 10; i++) {
            clocktest_loop();
        }
    }

    void clocktest_loop() {

#include "zone.inc"

        QDateTime currentDateTime = QDateTime::currentDateTime();
        QVERIFY(time->setTime(currentDateTime.addSecs(2)));

        QVERIFY(currentDateTime.secsTo(QDateTime::currentDateTime()) < 6);

        currentDateTime = QDateTime::currentDateTime();
        QString timezone;
        QVERIFY(time->getTimezone(timezone));


        for (int i = 0; i < zonecnt; i++) {
            QDateTime remoteTime;
#if F_SUPPORT_UNUSED
            QVERIFY(time->getRemoteTime(currentDateTime, zone[i], remoteTime));
#else
            QVERIFY(time->remoteTime(zone[i], ::time(NULL), remoteTime));
#endif
        }

#if F_SUPPORT_UNUSED
        QVERIFY(time->isOperatorTimeAccessible() == 0 || time->isOperatorTimeAccessible() == 1);
#else
        bool result, ret = time->isOperatorTimeAccessible(result) ;
        QVERIFY(ret) ; // call not failed
#endif


        currentDateTime = QDateTime::currentDateTime();
        qDebug() << "currentDateTime: " << currentDateTime;
        QVERIFY(currentDateTime.secsTo(QDateTime::currentDateTime()) < 6);

        qDebug() << "timezone: " << zone[0];
        QVERIFY2(time->setTimezone(zone[0]), zone[0]);
        // TODO: Without wait here the test will fail.
        //waitTwice(500);
        currentDateTime = QDateTime::currentDateTime();
        qDebug() << "currentDateTime: " << currentDateTime;
        QVERIFY(currentDateTime.secsTo(QDateTime::currentDateTime()) < 6);

        currentDateTime = QDateTime::currentDateTime();
        qDebug() << "currentDateTime: " << currentDateTime;
        QVERIFY(time->setTime(currentDateTime.addSecs(3)));
        int secsTo = currentDateTime.secsTo(QDateTime::currentDateTime());
        qDebug() << "secsTo: " << secsTo;
        QVERIFY(secsTo < 6 && secsTo >= 2);
        QVERIFY(time->setTime(currentDateTime.addSecs(2)));
        QVERIFY(currentDateTime.secsTo(QDateTime::currentDateTime()) < 10);

        for (int i = 0; i < zonecnt; i++) {
            QVERIFY(time->setTimezone(zone[i]));
        }
    }

    void cleanupTestCase() {
        QVERIFY(time->setTime(oldTime));
        QVERIFY(time->setTimezone(oldTz));
        //QVERIFY(time->setTimeFormat(oldTimeFormat));
        delete time;
    }
};

TestClass::pre_init_t TestClass::pre_init ;

QTEST_MAIN(TestClass)
#include "time.moc"
