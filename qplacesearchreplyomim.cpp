/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qplacesearchreplyomim.h"
#include "qplacemanagerengineomim.h"
#include <QPlaceResult>
#include <QtPositioning/private/qwebmercator_p.h>
#include <QtPositioning/private/qdoublevector2d_p.h>
#include <QDebug>

QPlaceSearchReplyOmim::QPlaceSearchReplyOmim(const QPlaceSearchRequest &request, QPlaceManagerEngineOmim *parent)
    :   QPlaceSearchReply(parent)
{
    setRequest(request);
}

QPlaceSearchReplyOmim::~QPlaceSearchReplyOmim()
{

}

void QPlaceSearchReplyOmim::setError(QPlaceReply::Error errorCode, const QString &errorString)
{
    QPlaceReply::setError(errorCode, errorString);
    emit error(errorCode, errorString);

    setFinished(true);
    emit finished();
}

static QGeoCoordinate fromMWMCoord(QDoubleVector2D pos)
{
    pos /= 180.0;
    pos += QDoubleVector2D(1.0, 1.0);
    pos /= 2.0;
    pos.setY(1.0 - pos.y());

    QGeoCoordinate coord = QWebMercator::mercatorToCoord(pos);

    return coord;
}

void QPlaceSearchReplyOmim::onReplyFinished(const search::Results &results, uint64_t timestamp)
{
    if (m_finished)
        return;

    m_finished++;
    QList<QPlaceSearchResult> qresults;


    for (size_t i = 0; i < results.GetCount(); ++i)
    {
        QPlaceResult placeResult;
        auto const & res = results[i];

        QString name = QString::fromStdString(res.GetString());
        QGeoAddress address;
        address.setText(QString::fromStdString(res.GetAddress()));
        QGeoLocation location;
        location.setAddress(address);
        location.setCoordinate(fromMWMCoord(QDoubleVector2D(res.GetFeatureCenter().x, res.GetFeatureCenter().y)));
        QPlace place;
        place.setLocation(location);
        place.setName(name);
        placeResult.setPlace(place);
        placeResult.setTitle(name);
        qDebug() << "Res "<<i<< " : " << name;
        qresults.append(placeResult);
    }

    setResults(qresults);

    setFinished(true);
    emit finished();
}
