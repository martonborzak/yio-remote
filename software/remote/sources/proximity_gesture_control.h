#ifndef PROXIMITY_GESTURE_CONTROL_H
#define PROXIMITY_GESTURE_CONTROL_H

#include <QObject>
#include <QThread>
#include <QtDebug>

#ifdef __arm__
#include "apds9960.h"
#endif

class ProximityGestureControl : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(int ambientLight READ getambientLight)
    Q_PROPERTY(int proximity NOTIFY proximityEvent)
    Q_PROPERTY(QString gesture READ getgesture NOTIFY gestureEvent)
    Q_PROPERTY(QString apds9960Error READ getapds9960Error NOTIFY apds9960Notify)
    Q_PROPERTY(int proximitySetting READ getproximitySetting WRITE setproximitySetting)

    int getambientLight() {
        return int(m_ambientLight);
    }

    QString getgesture()
    {
        return m_gesture;
    }

    QString getapds9960Error()
    {
        return m_apds9960Error;
    }

    int getproximitySetting() {
        return m_proximitySetting;
    }

    void setproximitySetting(int proximity) {
        m_proximitySetting = proximity;
    }

    Q_INVOKABLE void proximityDetection(bool state, bool qml=true)
    {
        m_proximityDetection = state;
#ifdef __arm__
        if (state) {

            if (qml) {
                qDebug() << "Proximity turn on from QML, resetting retry count";
                retry_count = 0;
            }

            // turn on
            if ( !apds.enableProximitySensor(false) ){
                //: Error message that shows up as notification when the proximity sensor cannot be initialized
                m_apds9960Error = tr("Cannot initialize the proximity sensor.");
                emit apds9960Notify();
                return;
            }
            delay(200);
            apds.setProximityGain(2);
            apds.setProximityIntLowThreshold(0);
            apds.setProximityIntHighThreshold(uint8_t(m_proximitySetting));
            apds.clearProximityInt();
            apds.setLEDBoost(0);
            apds.setProximityIntEnable(1);

        } else {
            // turn off
            apds.disableProximitySensor();
        }
#endif
    }

    Q_INVOKABLE void gestureDetection(bool state)
    {
        m_gestureDetection = state;
#ifdef __arm__
        if (state) {
            // turn on
            apds.setGestureGain(0);
            apds.setLEDBoost(0);
            apds.enableGestureSensor(true);

        } else {
            // turn off
            apds.disableGestureSensor();
        }
#endif
    }

    Q_INVOKABLE int readAmbientLight()
    {
#ifdef __arm__
        // enable the light sensor
        if ( !apds.enableLightSensor(false) ) {
            qDebug() << "Cannot initialize the light sensor. Cannot read the light value.";
            return int(m_ambientLight);
        }

        delay(200);

        // read the ambient light
        apds.readAmbientLight(m_ambientLight);

        //disable light sensor
        apds.disableLightSensor();
#endif

        return int(m_ambientLight);
    }

    Q_INVOKABLE void readInterrupt() {
#ifdef __arm__
        if (m_proximityDetection) {
            // read the value
            apds.readProximity(m_proximity);
            delay(100);

            // clear the interrupt
            apds.clearProximityInt();
            delay(100);

            // turn off proximity detection
            proximityDetection(false);

            if (m_proximity > m_proximitySetting) {

                delay(200);
                // let qml know
                emit proximityEvent();
                qDebug() << "Proximity detected:" << m_proximity;

            } else {
                // if the reading is smaller than the threshold, restart the sensor
                // this needed as a workaround for some weird issue of reporting continous interrupts
                if (retry_count < 4) {
                    delay(200);
                    restart();
                    retry_count++;
                    qDebug() << "Restarting the proximity sensor" << retry_count;
                } else {
                    qDebug() << "Tried restarting too many times. Will wait until a new standby cycle.";
                }
            }

        } else if (m_gestureDetection) {

            // read the gesture
            if ( false == apds.isGestureAvailable() )
            {
                return;
            }

            switch ( apds.readGesture() )
            {
            case DIR_UP:
                m_gesture = "right";
                emit gestureEvent();
                break;
            case DIR_DOWN:
                emit gestureEvent();
                m_gesture = "left";
                break;
            case DIR_LEFT:
                emit gestureEvent();
                m_gesture = "up";
                break;
            case DIR_RIGHT:
                emit gestureEvent();
                m_gesture = "down";
                break;
            }
        }
#endif
    }

public:
    ProximityGestureControl()
    {
#ifdef __arm__
        if ( !apds.initi2c() )
        {
            //: Error message that shows up as notification when the i2c interface cannot be initialized
            m_apds9960Error = tr("Cannot initialize the i2c interface. Please restart the remote.");
            emit apds9960Notify();
            return;
        }

        if ( !apds.init() )
        {
            //: Error message that shows up as notification when the proximity sensor cannot be initialized
            m_apds9960Error = tr("Cannot initialize the proximity sensor. Please restart the remote.");
            emit apds9960Notify();
            return;
        }

        delay(100);

        if ( !apds.enableLightSensor(false) ) {
            //: Error message that shows up as notification when the light sensor cannot be initialized
            m_apds9960Error = tr("Cannot initialize the light sensor.");
            emit apds9960Notify();
            return;
        }

        delay(100);

        // read the ambient light when it's on first
        if ( !apds.readAmbientLight(m_ambientLight)) {
            //: Error message that shows up as notification when light value cannot be read
            m_apds9960Error = tr("Error reading light values.");
            emit apds9960Notify();
            return;
        }

        delay(100);
        // disable the light sensor
        apds.disableLightSensor();
#endif
    }

    ~ProximityGestureControl()
    {

    }

signals:
    void proximityEvent();
    void gestureEvent();
    void apds9960Notify();

private:
    void restart()
    {
#ifdef __arm__
        // power cycle the sensor
        apds.disablePower();
        delay(200);
//        apds.enablePower();

        // initalise the sensor again
        if ( !apds.init() )
        {
            qDebug() << "Cannot initialize the proximity sensor.";
            return;
        }
        delay(200);

        // turn on proximity detection
        proximityDetection(true, false);
#endif
    }

#ifdef __arm__
    APDS9960 apds = APDS9960();
#endif
    uint16_t m_ambientLight;
    uint8_t m_proximity;
    QString m_gesture;
    QString m_apds9960Error;
    bool m_proximityDetection = false;
    bool m_gestureDetection = false;
    int m_proximitySetting = 100; // default value
    int retry_count = 0;
};

#endif // PROXIMITY_GESTURE_CONTROL_H
