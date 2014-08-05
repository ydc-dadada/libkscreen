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

#ifndef QSCREEN_CONFIG_H
#define QSCREEN_CONFIG_H

#include "../abstractbackend.h"
#include "config.h"

#include <QScreen>
#include <QtCore/QSize>
#include <QLoggingCategory>

namespace KScreen
{
class Output;
class QScreenOutput;
class QScreenScreen;

class QScreenConfig : public Config
{
    Q_OBJECT

public:
    explicit QScreenConfig(QObject *parent = 0);
    virtual ~QScreenConfig();

    QMap<int, QScreenOutput *> outputMap() const;

private Q_SLOTS:
    void updateConfig();

private:
    QScreenScreen *m_screen;
};

} // namespace

Q_DECLARE_LOGGING_CATEGORY(KSCREEN_QSCREEN)

#endif // QSCREEN_CONFIG_H