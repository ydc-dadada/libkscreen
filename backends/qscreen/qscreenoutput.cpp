/*************************************************************************************
 *  Copyright 2014 Sebastian K�gler <sebas@kde.org>                                  *
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

#include "qscreenoutput.h"
#include <mode.h>

#include <QtCore/QRect>

#include <QGuiApplication>
#include <QScreen>



using namespace KScreen;

static int s_kscreenqscreenbackendOutputId = -1;

int getOutputId()
{
    s_kscreenqscreenbackendOutputId++;
    return s_kscreenqscreenbackendOutputId;
}


QScreenOutput::QScreenOutput(const QScreen *qscreen, QObject* parent)
    : Output(parent)
    , m_qscreen(qscreen)

{
    updateFromQScreen(qscreen);
}

QScreenOutput::~QScreenOutput()
{
}

void QScreenOutput::updateFromQScreen(const QScreen *qscreen)
{

    // Initialize primary output
    setId(getOutputId());
    setName(qscreen->name());
    setSizeMm(qscreen->size());
    setEnabled(true);
    setConnected(true);
    setPrimary(QGuiApplication::primaryScreen() == qscreen);

    // Rotation

    Mode* mode = new Mode(this);
    const QString modeid = QStringLiteral("14");
    mode->setId(modeid);
    mode->setRefreshRate(qscreen->refreshRate());
    mode->setSize(qscreen->size());

    setCurrentModeId(modeid);

    const QString modename = QString::number(qscreen->size().width()) + QStringLiteral("x") + QString::number(qscreen->size().height()) + QStringLiteral("@") + QString::number(qscreen->refreshRate());
    mode->setName(modename);

    ModeList modes;
    modes[modeid] = mode;
    setModes(modes);


    qCDebug(KSCREEN_QSCREEN) << "   Output.setCurrentSize: " << sizeMm() << modename;

}


#include "qscreenoutput.moc"













