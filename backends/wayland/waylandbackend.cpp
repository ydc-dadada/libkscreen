/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
 *  Copyright (C) 2012, 2013 by Daniel Vrátil <dvratil@redhat.com>                   *
 *  Copyright 2014 Sebastian K gler <sebas@kde.org>                                  *
 *                                                                                   *
 *  This library is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU Lesser General Public                       *
 *  License as published by the Free Software Foundation; either                     *
 *  version 2.1 of the License, or (at your option) any later version.               *
 *                                                                                   *
 *  This library is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                *
 *  Lesser General Public License for more details.                                  *
 *                                                                                   *
 *  You should have received a copy of the GNU Lesser General Public                 *
 *  License along with this library; if not, write to the Free Software              *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA       *
 *************************************************************************************/

#include "waylandbackend.h"
#include "waylandconfig.h"
#include "waylandoutput.h"

#include <configmonitor.h>
#include <mode.h>

#include <QtCore/QFile>
#include <QtCore/qplugin.h>
#include <QtCore/QRect>
#include <QAbstractEventDispatcher>

#include <QGuiApplication>
#include <QScreen>

using namespace KScreen;

Q_LOGGING_CATEGORY(KSCREEN_WAYLAND, "kscreen.wayland");

WaylandConfig* WaylandBackend::s_internalConfig = 0;

WaylandBackend::WaylandBackend(QObject *parent)
    : QObject(parent)
    , m_isValid(true)
{
    QLoggingCategory::setFilterRules(QLatin1Literal("kscreen.qscreen.debug = true"));

    if (s_internalConfig == 0) {
        s_internalConfig = new WaylandConfig();
    }
}

WaylandBackend::~WaylandBackend()
{
}

QString WaylandBackend::name() const
{
    return QString("Wayland");
}

Config *WaylandBackend::config() const
{
    return s_internalConfig->toKScreenConfig();
}

void WaylandBackend::setConfig(Config *config) const
{
    if (!config) {
        return;
    }

    qWarning() << "The Wayland backend for libkscreen is currently read-only,";
    qWarning() << "setting a configuration is not supported.";
    qWarning() << "You can force another backend using the KSCREEN_BACKEND env var.";
}

Edid *WaylandBackend::edid(int outputId) const
{
    WaylandOutput *output = s_internalConfig->outputMap().value(outputId);
    if (!output) {
        return 0;
    }
    return output->edid();

    return 0;
}

bool WaylandBackend::isValid() const
{
    return m_isValid;
}

void WaylandBackend::updateConfig(Config *config) const
{
    Q_ASSERT(config != 0);
    s_internalConfig->updateKScreenConfig(config);
}


