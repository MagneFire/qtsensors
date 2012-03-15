/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtSensors module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qpickupsensorgesturerecognizer.h"

#include <QtCore/qmath.h>

#define RADIANS_TO_DEGREES 57.2957795

QT_BEGIN_NAMESPACE

QPickupSensorGestureRecognizer::QPickupSensorGestureRecognizer(QObject *parent)
    : QSensorGestureRecognizer(parent)
    , active(0)
    , atRest(1)
    , okToSignal(1)
    , lastpitch(0)
    , detecting(0)
{
}

QPickupSensorGestureRecognizer::~QPickupSensorGestureRecognizer()
{
}

void QPickupSensorGestureRecognizer::create()
{
    timer = new QTimer(this);

    connect(timer,SIGNAL(timeout()),this,SLOT(timeout()));
    timer->setSingleShot(true);
    timer->setInterval(750);

}

QString QPickupSensorGestureRecognizer::id() const
{
    return QString("QtSensors.pickup");
}

bool QPickupSensorGestureRecognizer::start()
{
    if (QtSensorGestureSensorHandler::instance()->startSensor(QtSensorGestureSensorHandler::Accel)) {
        active = true;
        connect(QtSensorGestureSensorHandler::instance(),SIGNAL(accelReadingChanged(QAccelerometerReading *)),
                this,SLOT(accelChanged(QAccelerometerReading *)));
    } else {
        active = false;
    }
    return active;

}

bool QPickupSensorGestureRecognizer::stop()
{
    QtSensorGestureSensorHandler::instance()->stopSensor(QtSensorGestureSensorHandler::Accel);
    disconnect(QtSensorGestureSensorHandler::instance(),SIGNAL(accelReadingChanged(QAccelerometerReading*)),
            this,SLOT(accelChanged(QAccelerometerReading *)));
    active = false;
    return active;
}

bool QPickupSensorGestureRecognizer::isActive()
{
    return active;
}

#define PICKUP_BOTTOM_THRESHOLD 20
#define PICKUP_TOP_THRESHOLD 87

void QPickupSensorGestureRecognizer::accelChanged(QAccelerometerReading *reading)
{
    accelReading = reading;
    qreal x = reading->x();
    qreal xdiff =  pXaxis - x;
    qreal y = reading->y();
    qreal ydiff = pYaxis - y;
    qreal z = reading->z();
    qreal zdiff =  pZaxis - z;

    pitch = qAtan(y / qSqrt(x*x + z*z)) * RADIANS_TO_DEGREES;

    if (zList.count() > 5)
        zList.removeLast();

    if (qAbs(xdiff) < 0.7 && qAbs(ydiff) < .7 && qAbs(zdiff) < .7) {
        atRest = true;
    } else {
        atRest = false;
    }

    if (detectingNegativeList.count() > 5)
        detectingNegativeList.removeLast();

    if (!detecting) {
        zList.insert(0,z);
    }
    if (detecting && z < 0) {
        okToSignal = false;
        detecting = false;
        detectingNegativeList.insert(0,true);
        atRest = true;
    }

    if (!atRest && !detecting && (lastpitch - pitch < -PICKUP_BOTTOM_THRESHOLD)) {
        detecting = true;
        if (!timer->isActive()) {
            timer->start();
        }
        detectedPitchDifference = lastpitch - pitch;
        lastpitch = pitch;
        okToSignal = true;
    }

    pXaxis = x;
    pYaxis = y;
    pZaxis = z;
    if (atRest && !detecting)
        lastpitch = pitch;
}

void QPickupSensorGestureRecognizer::timeout()
{
    qreal x = accelReading->x();
    qreal y = accelReading->y();
    qreal z = accelReading->z();

    qreal roll = qAtan(x / qSqrt(y*y + z*z)) * RADIANS_TO_DEGREES;

    bool ok = true;
    for (int i = 0; i < zList.count() - 1; i++) {
        if (zList.at(i) < 0) {
            ok = false;
        }
    }

    if (ok) {
        for (int i = 0; i < detectingNegativeList.count() - 1; i++) {
            if (detectingNegativeList.at(i) == true) {
                ok = false;
            }
        }
    }

    if (ok && detecting
            && okToSignal
            && qAbs(roll) < 10
            && (pitch < PICKUP_TOP_THRESHOLD
                && pitch > PICKUP_BOTTOM_THRESHOLD)
            && (y > 4.0 && y < 10)
            && (z > 4.0 && z < 10)) {
        Q_EMIT pickup();
        Q_EMIT detected("pickup");
    }
    clear();
}


void QPickupSensorGestureRecognizer::clear()
{
    okToSignal = false;
    detecting = false;
    detectingNegativeList.clear();
}
QT_END_NAMESPACE

