/*!
 * @file qmbattery.cpp
 * @brief QmBattery

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Aliaksei Katovich <aliaksei.katovich@nokia.com>
   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Matti Halme <matthalm@esdhcp04350.research.nokia.com>
   @author Na Fan <ext-na.2.fan@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Matti Halme <matti.halme@nokia.com>
   @author Denis Zalevskiy <denis.zalevskiy@nokia.com>

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

#include "qmbattery.h"
#include "qmbattery_p.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QSocketNotifier>
#include <QTimer>

extern "C" {
#include <errno.h>
#include <time.h>
#include "bme/bmeipc.h"
#include "bme/bmemsg.h"
#include "bme/em_isi.h"
}

#define BMECLI_TIMEOUT 3000  /* ms */
#define BMECURRENT_TIMEOUT 5010
#define STAT_EXPIRATION_TIMEOUT 5 /* seconds */

#define DEFAULT_TALK_CURRENT   300 /* mA */
#define DEFAULT_ACTIVE_CURRENT 150 /* mA */
#define DEFAULT_IDLE_CURRENT     6 /* mA */

/**
 * The use time D-Bus interface copied from usetime/dbus.h as a workaround to a
 * circular build time dependency between qmsystem2 and usetime.
 */

#define USETIME_SERVICE "com.nokia.usetime"
#define USETIME_IF "com.nokia.usetime"
#define USETIME_PATH "/com/nokia/usetime"

#define USETIME_MODE_IDLE   1
#define USETIME_MODE_ACTIVE 2
#define USETIME_MODE_TALK   3

#define USETIME_METHOD_GET_CURRENT "getCurrent"


#define dbg(a) qDebug() << __PRETTY_FUNCTION__ << ": " << a

#if 0
#define DUMP_MSG(msg)                                                   \
qDebug() << "["                                                         \
                 << (msg.timestamp.tv_sec * 1000000UL +	msg.timestamp.tv_usec) \
                 << "]:" << (msg.bat_current / 1000.0) * (msg.bat_voltage / 1000.0) \
                 << (msg.bat_current / 1000.0)                          \
                 << (msg.bat_voltage / 1000.0)                          \
                 << (msg.bat_temp - 273.15);
#else
#define DUMP_MSG(msg) do {} while (0)
#endif



namespace MeeGo {


template <typename T>
class EmHandle
{
public:
    EmHandle() { }

    virtual ~EmHandle() { close(); }

    bool open()
    {
        T *self = static_cast<T*>(this);
        if (!self->is_opened()) {
            self->open_();
        }

        if (!self->is_opened()) {
            qCritical() << "EM: error opening: " << strerror(errno);
        }
        return self->is_opened();
    }

    void close()
    {
        T *self = static_cast<T*>(this);
        if (self->is_opened()) {
            self->close_();
        }
    }

};

#define BMEIPC_MAX_TRIES 2

class EmIpc : public EmHandle<EmIpc>
{
    friend class EmHandle<EmIpc>;
public:

    EmIpc() : EmHandle<EmIpc>(), sd_(-1), restart_count_(0) {}

    bool query(const void *msg1, int len1, void *msg2 = NULL, int len2 = -1)
    {
        if (!is_opened()) {
	    qCritical() << "EM: not open";
            return false;
	}

	int tries = 0;
	while (true) {
	    tries++;
	    if (::bmeipc_query(sd_, msg1, len1, msg2, len2) >= 0)
		return true;
	    if (errno == EIO)
		restart_count_++;
	    if (tries >= BMEIPC_MAX_TRIES)  {
		qCritical() << "EM: Query failed: " << strerror(errno);
		return false;
	    }
	    qWarning() << "EM:" << strerror(errno) << "(trying to reopen)";
	    close();
	    if (!open())
		return false;
        }
    }

    bool is_opened() { return sd_ >= 0; }
    int restart_count() { return restart_count_; }

private:

    inline void open_()
    {
        sd_ = ::bmeipc_open();
    }

    inline void close_()
    {
        ::bmeipc_close(sd_);
        sd_ = -1;
    }


    int sd_;
    int restart_count_;
};


class EmEvents : public EmHandle<EmEvents>
{
    friend class EmHandle<EmEvents>;

public:
    EmEvents(int mask = -1)
        : EmHandle<EmEvents>(),
          sd_(-1),
          mask_(mask)
    { }

    int read()
    {
        if (!is_opened()) {
            return BMEVENT_ERROR;
        }

        int res = ::bmeipc_eread(sd_);
        if (res == BMEVENT_ERROR) {
            qDebug() << "bmeipc_eread returned error" << strerror(errno);
        }
        return res;
    }

    inline bool is_opened() { return sd_ >= 0; }

    QSocketNotifier const* notifier() const { return notifier_.data(); }

private:

    inline void open_()
    {
        sd_ = ::bmeipc_eopen(mask_);
        if (is_opened())
            notifier_.reset(new QSocketNotifier(sd_, QSocketNotifier::Read));
    }

    inline void close_()
    {
        notifier_.reset(0);
        ::bmeipc_eclose(sd_);
        sd_ = -1;
    }

    int sd_;
    int mask_;
    QScopedPointer<QSocketNotifier> notifier_;
};


class EmCurrentMeasurement : public EmHandle<EmCurrentMeasurement>
{
    friend class EmHandle<EmCurrentMeasurement>;

public:
    
    EmCurrentMeasurement(unsigned int period)
        : EmHandle<EmCurrentMeasurement>(),
          period_(period),
          mq_(-1),
          em_ipc_(new EmIpc())
    { }

    inline bool is_opened() { return mq_ >= 0; }

    bool measure(int &current)
    {
        current = 0;

        if (!is_opened())
            return false;

        int n;
        bmeipc_meas_t msg;
        n = mq_receive(mq_, (char *)&msg, sizeof(msg), 0);
        
        if (0 > n) {
            qDebug() << "failed to receive message: "
                     << strerror(errno);
        } else if (n != sizeof(msg)) {
            qDebug() << "bad message size: need "
                     << sizeof (msg) << ", got " << n;
        } else if (MEASUREMENTS_ERROR == msg.state) {
            qDebug() << " error message received";
        } else if (MEASUREMENTS_OFF == msg.state) {
            qDebug() << "measurements are off";
        } else {
            DUMP_MSG(msg);
            current = msg.bat_current;
            return true;
        }
        return false;
    }

    QSocketNotifier const* notifier() const { return notifier_.data(); }

private:

    inline bool request_measurements_(unsigned int period)
    {
        struct emsg_measurement_req req;
        struct emsg_measurement_req_elem req_elem;

        if (!em_ipc_->open()) {
            qCritical() << "failed to contact bme server: "
                        << strerror(errno);
            return false;
        }

        /* Send message header */
        req.type = EM_MEASUREMENT_REQ;
        req.subtype = 0;
        req.measurement_action = EM_MEASUREMENT_ACTION_START;
        req.channel_count = 1;

        if (!em_ipc_->query(&req, sizeof(req))) {
            qCritical() << "failed to request measurments: "
                        << strerror(errno);
            return false;
        }

        req_elem.type = EM_MEASUREMENT_TYPE_CURRENT;
        req_elem.period = period;

        if (!em_ipc_->query(&req_elem, sizeof(req_elem))) {
            qCritical() << "failed to request data: " << strerror(errno);
            return false;
        }

        return true;

    }

    void open_()
    {
        if (!request_measurements_(period_))
            return;

        mq_ = mq_open(BMEIPC_MQNAME, O_RDONLY);
        if (!is_opened())
            return;

        notifier_.reset(new QSocketNotifier(mq_, QSocketNotifier::Read));
    }

    void close_()
    {
        struct emsg_measurement_req req;

        req.type = EM_MEASUREMENT_REQ;
        req.subtype = 0;
        req.measurement_action = EM_MEASUREMENT_ACTION_STOP;
        req.channel_count = 0;

        if (!em_ipc_->query(&req, sizeof(req))) {
            qCritical() << "failed to stop measurements"
                        << strerror(errno);
        }

        em_ipc_->close();

        notifier_.reset(0);
        ::mq_close(mq_);
        mq_ = -1;
    }

    int period_;
    mqd_t mq_;
    QScopedPointer<EmIpc> em_ipc_;
    QScopedPointer<QSocketNotifier> notifier_;
};


/*------------ class QmBatteryPrivate ------------*/
QmBatteryPrivate::QmBatteryPrivate()
	: parent_(0),
      is_data_actual_(false),
      cache_expire_(QDateTime::currentDateTime()),
      cc_offset_(0),
      prev_cc_restart_count_(-1),
      ipc_(new EmIpc()),
      events_(new EmEvents())
{
    memset(&stat_, 0, sizeof(stat_));
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(waitForUSB500mA()));
    usb100ma_emit_delayed = 0;
}

QmBatteryPrivate::~QmBatteryPrivate() {
    delete timer;
}

bool QmBatteryPrivate::init(QmBattery *parent)
{
    parent_ = parent;
    if (!events_->open())
        return false; // error, need to return value

    connect(events_->notifier(), SIGNAL(activated(int))
            , this, SLOT(onEmEvent(int)));

    saveStat_();
    return true;
}

void QmBatteryPrivate::queryStat_() const
{
    QDateTime now(QDateTime::currentDateTime());

    if (!is_data_actual_ || now >= cache_expire_) {
        if (!ipc_->open())
	    return;

	int prev_cc = stat_[COULOMB_COUNTER];
	
	bmeipc_msg_t request;
	request.type = BME_SYSMSG_GETSTAT;
	request.subtype = 0;
	if (!ipc_->query(&request, sizeof(request), &stat_, sizeof(stat_)))
	    return;
	    
	is_data_actual_ = true;
        cache_expire_ = now.addSecs(STAT_EXPIRATION_TIMEOUT);

	if (prev_cc_restart_count_ != ipc_->restart_count()) {
	    /* 
	     * BME has re-started. Adjust the coulomb counter offset to hide
	     * counter reset.
	     *
	     * @note: All the coulombs since the previous call have been lost,
	     *        but let's consider that acceptable.
	     */
	    cc_offset_ += (prev_cc - stat_[COULOMB_COUNTER]);
	    qDebug() << "CC reset, prev_cc:" << prev_cc
		     << "new_cc" << stat_[COULOMB_COUNTER]
		     << "new offset:" << cc_offset_;
	    prev_cc_restart_count_ = ipc_->restart_count();
	}
    }
}

void QmBatteryPrivate::saveStat_()
{
    queryStat_();
    memcpy(&saved_stat_, &stat_, sizeof(saved_stat_));
}

int QmBatteryPrivate::getStat(int index) const
{
    queryStat_();
    return stat_[index];
}

int QmBatteryPrivate::getCumulativeBatteryCurrent()
{
    return getStat(COULOMB_COUNTER) + cc_offset_;
}

int QmBatteryPrivate::getAverageCurrent(int usageMode,
					QmBattery::RemainingTimeMode psMode,
					int defaultCurrent) const
{
    int result = makeUsetimeQuery(USETIME_METHOD_GET_CURRENT,
				  usageMode, psMode);
    if (result < 0)
	result = defaultCurrent;
    
    return result;
}

int QmBatteryPrivate::getRemainingTime(int usageMode,
				       QmBattery::RemainingTimeMode psMode,
				       int defaultCurrent) const
{
    int current = getAverageCurrent(usageMode, psMode, defaultCurrent);

    union emsg_usetime_info msg;
    memset(&msg, 0, sizeof(msg));
    msg.request.type = EM_BATTERY_USETIME_REQ;
    msg.request.current = current;
    if (!ipc_->query(&msg, sizeof(msg.request),
		     &msg, sizeof(msg.reply))) {
	qWarning() << "Can't query use time:" << strerror(errno);
	return -1;
    }

    qDebug() << __FUNCTION__ << usageMode << psMode
	     << "current (mA):" << current
	     << "remaining time (s):" << msg.reply.time;
    
    return msg.reply.time;
}

int QmBatteryPrivate::makeUsetimeQuery(const QString& method, int usageMode,
				       QmBattery::RemainingTimeMode psMode)
    const
{
    int result = -1;
    
    QDBusMessage msg = QDBusMessage::createMethodCall(
	USETIME_SERVICE, USETIME_PATH, USETIME_IF, method);
    
    QList<QVariant> args;
    args << usageMode;
    if (psMode == QmBattery::PowersaveMode)
	args << 1;
    else 
	args << 0;
    msg.setArguments(args);
    
    QDBusReply<int> tReply = QDBusConnection::systemBus().call(msg);
    if(tReply.isValid()) {
	result = tReply.value();
    } else {
	/**
	 * @note The following error means that the usetime package
	 *       is not installed or that the usetime daemon is not
	 *       running.
	 *
	 * 4 QDBusError::ServiceUnknown
	 * "org.freedesktop.DBus.Error.ServiceUnknown"
	 * "The name com.nokia.usetime was not provided by any
	 * .service files"
	 */
	qDebug() << "No use time estimate available"
		 << method << usageMode << psMode;
	if (tReply.error().isValid()) {
	    qDebug() << tReply.error().type()
		     << tReply.error().name()
		     << tReply.error().message();
	}
    }
    
    return result;
}


bool QmBatteryPrivate::startCurrentMeasurement(QmBattery::Period rate)
{
    unsigned int period = 0;

    switch (rate) {
    case QmBattery::RATE_250ms:
        period = EM_MEASUREMENT_PERIOD_250MS;
        break;
    case QmBattery::RATE_1000ms:
        period = EM_MEASUREMENT_PERIOD_1S;
        break;
    case QmBattery::RATE_5000ms:
        period = EM_MEASUREMENT_PERIOD_5S;
        break;
    }

    if (!measurements_.isNull()) {
        qDebug() << "Current Measurement is ongoing."
                 << " Stop it, to restart new current measurement.";
        return false;
    }

    measurements_.reset(new EmCurrentMeasurement(period));
    if (!measurements_->open())
        return false;

    connect(measurements_->notifier(), SIGNAL(activated(int))
            , this, SLOT(onMeasurement(int)));

    return true;
}

bool QmBatteryPrivate::stopCurrentMeasurement()
{
    measurements_->close();
    if (measurements_->is_opened()) {
        qDebug() << "QmBattery::stopCurrentMeasurement failed";
        return false;
    }
    measurements_.reset(0);
    return true;
}

void QmBatteryPrivate::onMeasurement(int /*socket*/)
{
    bool rc;
    int current;

    if (measurements_.isNull()) {
        qWarning() << "onMeasurement: null";
        return;
    }

    rc = measurements_->measure(current);
    emit parent_->batteryCurrent(current);
}

void QmBatteryPrivate::emitEventBatmon_()
{
    queryStat_();

    bool is_level_changed 
        = (saved_stat_[BATTERY_LEVEL_PCT] != stat_[BATTERY_LEVEL_PCT]
           || saved_stat_[BATTERY_LEVEL_NOW] != stat_[BATTERY_LEVEL_NOW]);

    bool is_state_changed = (saved_stat_[BATTERY_STATE] != stat_[BATTERY_STATE]);

    if (is_level_changed || is_state_changed)
        saveStat_();

    if (is_level_changed) {
        emit parent_->batteryRemainingCapacityChanged
            (stat_[BATTERY_LEVEL_PCT], stat_[BATTERY_LEVEL_NOW]);
        /* ToDo: Depreciated, remove when possible */
        emit parent_->batteryEnergyLevelChanged(stat_[BATTERY_LEVEL_PCT]);
    }

    if (is_state_changed)
        emit parent_->batteryStateChanged((QmBattery::BatteryState)stat_[BATTERY_STATE]);
}

void QmBatteryPrivate::waitForUSB500mA()
{
    // USB 500 event didn't come. Emit the delayed signals.
    timer->stop();
    emit parent_->chargerEvent(parent_->getChargerType());
    emit parent_->chargingStateChanged(parent_->getChargingState());
}

void QmBatteryPrivate::onEmEvent(int /*socket*/)
{
    is_data_actual_ = false;
    int events = events_->read();
    if (BMEVENT_CHARGER & events) {
        qDebug() << "BMEVENT_CHARGER";
        switch (getStat(CHARGER_TYPE)) {
        case CHARGER_TYPE_USB100MA:
            timer->start(4000);
            break;
        default:
            if (timer->isActive()) timer->stop();
            emit parent_->chargerEvent(parent_->getChargerType());
            if (usb100ma_emit_delayed == 1) {
                emit parent_->chargingStateChanged(parent_->getChargingState());
                usb100ma_emit_delayed = 0;
            }
        }
    }
    if (BMEVENT_CHARGE & events) {
        qDebug() << "BMEVENT_CHARGING";
        //when usb100ma_emit_delayed is 1, means we missed one charging state signal. when timer is active, means waiting for usb500
        if (timer->isActive()) { usb100ma_emit_delayed = 1; return;}
        emit parent_->chargingStateChanged(parent_->getChargingState());
    }
    if (BMEVENT_BATMON & events) {
        qDebug() << "BMEVENT_BATMON";
        emitEventBatmon_();
    }
}

/*------------ class QmBattery Implementation ------------*/

QmBattery::QmBattery(QObject *parent)
    : QObject(parent),
      pimpl_(new QmBatteryPrivate())
{
    if (!pimpl_->init(this))
        qWarning() << "QmBattery is not initialized";

    //Register the datatypes for  usage on DBus
    qRegisterMetaType < BatteryState >
        ("MeeGo::QmBattery::BatteryState");
    qRegisterMetaType < ChargingState >
        ("MeeGo::QmBattery::ChargingState");
    qRegisterMetaType < ChargerType >
        ("MeeGo::QmBattery::ChargerType");
    qRegisterMetaType < RemainingTimeMode >
        ("MeeGo::QmBattery::RemainingTimeMode");
    qRegisterMetaType < Period >
        ("MeeGo::QmBattery::Period");

    /* Depreceated, use BatteryState */
    qRegisterMetaType < Level >
        ("MeeGo::QmBattery::Level");
}

QmBattery::~QmBattery() { }

int QmBattery::getNominalCapacity() const
{
    return pimpl_->getStat(BATTERY_CAPA_MAX);
}

QmBattery::BatteryState QmBattery::getBatteryState() const
{
    switch (pimpl_->getStat(BATTERY_STATE)) {
    case BATTERY_STATE_EMPTY:
        return StateEmpty;
    case BATTERY_STATE_LOW:
        return StateLow;
    case BATTERY_STATE_OK:
        return StateOK;
    case BATTERY_STATE_FULL:
        return StateFull;
    case BATTERY_STATE_ERROR:
    default:
        return StateError;
    }
}

int QmBattery::getRemainingCapacitymAh() const
{
    return pimpl_->getStat(BATTERY_CAPA_NOW);
}

int QmBattery::getRemainingCapacityPct() const
{
    return pimpl_->getStat(BATTERY_LEVEL_PCT);
}

int QmBattery::getRemainingCapacityBars() const
{
    return pimpl_->getStat(BATTERY_LEVEL_NOW);
}

int QmBattery::getMaxBars() const
{
    return pimpl_->getStat(BATTERY_LEVEL_MAX);
}

int QmBattery::getVoltage() const
{
    return pimpl_->getStat(BATTERY_VOLT_NOW);
}

int QmBattery::getBatteryCurrent() const
{
    return pimpl_->getStat(BATTERY_CURRENT);
}

int QmBattery::getCumulativeBatteryCurrent() const
{
    return pimpl_->getCumulativeBatteryCurrent();
}

QmBattery::ChargerType QmBattery::getChargerType() const
{
    switch (pimpl_->getStat(CHARGER_TYPE)) {
    case CHARGER_TYPE_USB100MA:
        return USB_100mA;
    case CHARGER_TYPE_USB500MA:
        return USB_500mA;
    case CHARGER_TYPE_USBWALL:
    case CHARGER_TYPE_DYNAMO:
        return Wall;
    case CHARGER_TYPE_NONE:
        return None;
    case CHARGER_TYPE_ERROR:
    default:
        return Unknown;
    }
}

QmBattery::ChargingState QmBattery::getChargingState() const
{
    switch (pimpl_->getStat(CHARGING_STATE)) {
    case CHARGING_STATE_STOPPED:
        return StateNotCharging;
    case CHARGING_STATE_STARTED:
        return StateCharging;
    case CHARGING_STATE_ERROR:
    default:
        return StateChargingFailed;
    }
}

int QmBattery::getRemainingChargingTime() const
{
    if (pimpl_->getStat(CHARGING_STATE) == CHARGING_STATE_STARTED) {
	return pimpl_->getStat(CHARGING_TIME) * 60;
    } else {
	return -1;
    }
}

bool QmBattery::startCurrentMeasurement(Period rate)
{
    return pimpl_->startCurrentMeasurement(rate);
}

bool QmBattery::stopCurrentMeasurement()
{
    return pimpl_->stopCurrentMeasurement();
}

int QmBattery::getAverageTalkCurrent(RemainingTimeMode mode) const
{
    return pimpl_->getAverageCurrent(USETIME_MODE_TALK, mode,
				     DEFAULT_TALK_CURRENT);
}

int QmBattery::getRemainingTalkTime(QmBattery::RemainingTimeMode mode) const
{
    return pimpl_->getRemainingTime(USETIME_MODE_TALK, mode,
				    DEFAULT_TALK_CURRENT);
}

int QmBattery::getAverageActiveCurrent(RemainingTimeMode mode) const
{
    return pimpl_->getAverageCurrent(USETIME_MODE_ACTIVE, mode,
				     DEFAULT_ACTIVE_CURRENT);
}

int QmBattery::getRemainingActiveTime(RemainingTimeMode mode) const
{
    return pimpl_->getRemainingTime(USETIME_MODE_ACTIVE, mode,
				    DEFAULT_ACTIVE_CURRENT);
}

int QmBattery::getAverageIdleCurrent(RemainingTimeMode mode) const
{
	return pimpl_->getAverageCurrent(USETIME_MODE_IDLE, mode,
					 DEFAULT_IDLE_CURRENT);
}

int QmBattery::getRemainingIdleTime(QmBattery::RemainingTimeMode mode) const
{
	return pimpl_->getRemainingTime(USETIME_MODE_IDLE, mode,
					DEFAULT_IDLE_CURRENT);
}

QmBattery::BatteryCondition QmBattery::getBatteryCondition() const
{
    switch (pimpl_->getStat(BATTERY_CONDITION)) {
    case BATTERY_CONDITION_GOOD:
	return ConditionGood;
    case BATTERY_CONDITION_POOR:
	return ConditionPoor;
    default:
	return ConditionUnknown;
    }
}

int QmBattery::getBatteryEnergyLevel() const
{
    /* Depreceated */
    return getRemainingCapacityPct();
}

QmBattery::Level QmBattery::getLevel() const
{
    /* Depreceated */
    switch (getBatteryState()) {
    case StateFull:
        return LevelFull;
    case StateLow:
    case StateOK:
        return LevelLow;
    case StateError:
    case StateEmpty:
    default:
        return LevelCritical;
    }
}

QmBattery::State QmBattery::getState() const 
{
    return getChargingState();
}

} /* namespace MeeGo */
