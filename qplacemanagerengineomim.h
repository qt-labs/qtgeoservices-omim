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

#ifndef QPLACEMANAGERENGINEOMIM_H
#define QPLACEMANAGERENGINEOMIM_H

#include <QtLocation/QPlaceManagerEngine>
#include <QtLocation/QGeoServiceProvider>

#include <map/framework.hpp>
#include <map/everywhere_search_params.hpp>
#include <search/result.hpp>

QT_BEGIN_NAMESPACE

class QPlaceManagerEngineOmim : public QPlaceManagerEngine
{
    Q_OBJECT

public:
    QPlaceManagerEngineOmim(const QVariantMap &parameters, QGeoServiceProvider::Error *error,
                           QString *errorString);
    ~QPlaceManagerEngineOmim();

    QPlaceSearchReply *search(const QPlaceSearchRequest &request) Q_DECL_OVERRIDE;

private slots:
    void onReplyFinished();
    void onReplyError(QPlaceReply::Error errorCode, const QString &errorString);

private:
    Framework m_framework;
    search::EverywhereSearchParams m_params;
    uint64_t m_timestamp = 0;
    vector<search::Result> m_results;
};

QT_END_NAMESPACE

#endif // QPLACEMANAGERENGINEOMIM_H
