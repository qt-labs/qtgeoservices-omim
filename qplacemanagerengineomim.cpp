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

#include "qplacemanagerengineomim.h"
#include <QDebug>
#include <geometry/rect2d.hpp>
#include <QString>
#include <QPlaceSearchReply>
#include <QGeoRectangle>
#include <QPlaceResult>
#include <QPlace>
#include <QPointer>
#include <QGeoLocation>
#include <QtPositioning/private/qwebmercator_p.h>
#include <QtPositioning/private/qdoublevector2d_p.h>
#include "qplacesearchreplyomim.h"




QPlaceManagerEngineOmim::QPlaceManagerEngineOmim(const QVariantMap &parameters,
                                                 QGeoServiceProvider::Error *error,
                                                 QString *errorString)
    : QPlaceManagerEngine(parameters)
{
    *error = QGeoServiceProvider::NoError;
    errorString->clear();
}

QPlaceManagerEngineOmim::~QPlaceManagerEngineOmim()
{

}

// Here's the weirdness of this backend: it requires the coordinates to be in some sort of expanded mercator space:
//    function coordinateToText(coord) {
//        var merc = Mercator.toMercator(coord)

//        merc.y = 1.0 - merc.y
//        merc.x *= 2.0
//        merc.y *= 2.0
//        merc.x -= 1.0
//        merc.y -= 1.0
//        merc.x *= 180.0
//        merc.y *= 180.0

//        return merc.y.toFixed(5) +  ", " + merc.x.toFixed(5)
//    }
static QDoubleVector2D toMWMCoord(const QGeoCoordinate &coord)
{
    QDoubleVector2D merc = QWebMercator::coordToMercator(coord);
    merc.setY(1.0 - merc.y());

    merc *= 2.0;
    merc -= QDoubleVector2D(1.0, 1.0);
    merc *= 180.0;

    return merc;
}

QPlaceSearchReply *QPlaceManagerEngineOmim::search(const QPlaceSearchRequest &request)
{
    m_results.clear();
    QGeoRectangle rect = request.searchArea().boundingGeoRectangle();


    QDoubleVector2D topLeft = toMWMCoord(rect.topLeft());
    QDoubleVector2D bottomRight = toMWMCoord(rect.bottomRight());


    m2::RectD viewport(/*15.9262, 51.6196, 15.9553, 51.6775*/
                       topLeft.x(), bottomRight.y(), // minx miny
                       bottomRight.x(), topLeft.y()); // maxx maxy

    m_framework.GetSearchAPI().OnViewportChanged(viewport);

    const QString searchString = request.searchTerm();
    const QString normalized = searchString.normalized(QString::NormalizationForm_KC);

    if (normalized.isEmpty())
        return new QPlaceSearchReply();


    QPointer<QPlaceSearchReplyOmim> reply = new QPlaceSearchReplyOmim(request, this);
    m_params.m_query = normalized.toUtf8().constData();
    auto const timestamp = ++m_timestamp;
    m_params.m_onResults = [reply, timestamp, cnt = 0](search::Results const & results,
            vector<bool> const &  /*isLocalAdsCustomer*/) mutable {
        qDebug() << "Results came back " << cnt;
        if (cnt) {     // This ensure only first update will be propagated
            qDebug() << "Second Round <-- SKIPPED!!!!";
            return;
        }
        cnt = cnt + 1;
        if (reply)
            reply->onReplyFinished(results, timestamp);
    };

    connect(reply.data(), &QPlaceReply::finished, this, &QPlaceManagerEngineOmim::onReplyFinished);
    connect(reply.data(), QOverload<QPlaceReply::Error, const QString &>::of(&QPlaceReply::error),
            this, &QPlaceManagerEngineOmim::onReplyError);

    if (m_framework.SearchEverywhere(m_params))
    {
        qDebug() << "Search started";
    }
    else
    {
        reply->setError(QPlaceReply::UnknownError, QStringLiteral("Error!"));
    }

    return reply.data();
}

void QPlaceManagerEngineOmim::onReplyFinished()
{
    QPlaceReply *reply = qobject_cast<QPlaceReply *>(sender());
    if (reply)
        emit finished(reply);
}

void QPlaceManagerEngineOmim::onReplyError(QPlaceReply::Error errorCode, const QString &errorString)
{
    QPlaceReply *reply = qobject_cast<QPlaceReply *>(sender());
    if (reply)
        emit error(reply, errorCode, errorString);
}

