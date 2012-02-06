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


#include "qcoversensorgesturerecognizer.h"
#include <math.h>

QT_BEGIN_NAMESPACE

QCoverSensorGestureRecognizer::QCoverSensorGestureRecognizer(QObject *parent) :
    QSensorGestureRecognizer(parent),
    detecting(0), lastProx(0)
{
}

QCoverSensorGestureRecognizer::~QCoverSensorGestureRecognizer()
{
}

void QCoverSensorGestureRecognizer::create()
{
    proximity = new QIRProximitySensor(this);
    proximity->connectToBackend();

    orientation = new QOrientationSensor(this);
    orientation->connectToBackend();

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeout()));
    timer->setSingleShot(true);
    timer->setInterval(1000);
}

QString QCoverSensorGestureRecognizer::id() const
{
    return QString("QtSensors.cover");
}

bool QCoverSensorGestureRecognizer::start()
{
    connect(proximity,SIGNAL(readingChanged()),this,SLOT(proximityChanged()));
    proximity->start();
    orientation->start();
    lastProx = proximity->reading()->reflectance();
    return proximity->isActive();
}

bool QCoverSensorGestureRecognizer::stop()
{
    proximity->stop();
    orientation->stop();
    disconnect(proximity,SIGNAL(readingChanged()),this,SLOT(proximityChanged()));
    return proximity->isActive();
}

bool QCoverSensorGestureRecognizer::isActive()
{
    return proximity->isActive();
}

void QCoverSensorGestureRecognizer::proximityChanged()
{// look at case of face up->face down->face up.

    qreal refl = proximity->reading()->reflectance();
    qreal difference =  lastProx - refl;

    if (fabs(difference) < .15) {
        return;
    }

    if (orientation->reading()->orientation() ==  QOrientationReading::FaceUp
            && refl > .55) {
        if (!timer->isActive()) {
            timer->start();
            detecting = true;
        }
    }
    if (refl < .55) {
        if (timer->isActive()) {
            timer->stop();
            detecting = false;
        }
    }
    lastProx = refl;
}

void QCoverSensorGestureRecognizer::timeout()
{
    if (detecting && orientation->reading()->orientation() == QOrientationReading::FaceUp
            && proximity->reading()->reflectance() > 0.55) {
        Q_EMIT cover();
        Q_EMIT detected("cover");
        detecting = false;
    }
}

QT_END_NAMESPACE
