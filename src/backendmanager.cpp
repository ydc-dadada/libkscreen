/*
 * Copyright (C) 2014  Daniel Vratil <dvratil@redhat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "backendmanager_p.h"
#include "backendlauncher/backendloader.h"
#include "debug_p.h"

#include <QDBusConnection>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDBusConnectionInterface>

#include "config-libkscreen.h"

#include <QProcess>

using namespace KScreen;

Q_DECLARE_METATYPE(org::kde::kscreen::Backend*)

BackendManager *BackendManager::sInstance = 0;

BackendManager *BackendManager::instance()
{
    if (!sInstance) {
        sInstance = new BackendManager();
    }

    return sInstance;
}

BackendManager::BackendManager()
    : QObject()
    , mInterface(0)
    , mCrashCount(0)
{
    qRegisterMetaType<org::kde::kscreen::Backend*>("OrgKdeKscreenBackendInterface");
}

BackendManager::~BackendManager()
{
}

void BackendManager::requestBackend()
{
    if (mInterface) {
        QMetaObject::invokeMethod(this, "backendReady", Qt::QueuedConnection,
                                  Q_ARG(org::kde::kscreen::Backend*, mInterface));
        return;
    }

    // If an explicit backend is requested, then start it
    if (!qgetenv("KSCREEN_BACKEND").isEmpty()) {
        startBackend(QString::fromLatin1(qgetenv("KSCREEN_BACKEND")));
        return;
    }

    startBackend();
}

void BackendManager::startBackend(const QString &backend)
{
    if (mLauncher && mLauncher->state() == QProcess::Running) {
        mLauncher->terminate();
    }

    mLauncher = new QProcess(this);
    connect(mLauncher, static_cast<void(QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished),
            this, &BackendManager::launcherFinished);
    connect(mLauncher, &QProcess::readyReadStandardOutput,
            this, &BackendManager::launcherDataAvailable);

    QString launcher = QString::fromLatin1(CMAKE_INSTALL_FULL_LIBEXECDIR_KF5 "/kscreen_backend_launcher");
    if (!QFile::exists(launcher)) {
        launcher = QStandardPaths::findExecutable("kscreen_backend_launcher");
        if (launcher.isEmpty()) {
            qCWarning(KSCREEN) << "Failed to locate kscreen_backend_launcher, KScreen will be useless";
            delete mLauncher;
            mLauncher = 0;
            QMetaObject::invokeMethod(this, "backendReady", Qt::QueuedConnection,
                                      Q_ARG(org::kde::kscreen::Backend*, 0));
            return;
        }
    }

    mLauncher->setProgram(launcher);
    if (!backend.isEmpty()) {
        mLauncher->setArguments(QStringList() << "--backend" << backend);
    }

    mLauncher->start();
}

void BackendManager::launcherFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qCDebug(KSCREEN) << "Launcher finished with exit code" << exitCode << ", status" << exitStatus;

    if (exitStatus == QProcess::CrashExit) {
        // Backend has crashed: restart it
        delete mInterface;
        mInterface = 0;
        if (++mCrashCount < 4) {
            requestBackend();
        } else {
            qCWarning(KSCREEN) << "Launcher has crashed too many times: not restarting";
            mLauncher->deleteLater();
        }
        return;
    }

    switch (exitCode) {
    case BackendLoader::BackendLoaded:
        // This means that the launcher has terminated successfully after doing
        // what it was asked to do, so delete the interface, but don't emit signals
        delete mInterface;
        mInterface = 0;
        break;

    case BackendLoader::BackendFailedToLoad:
        // Launcher terminated immediatelly because there was no backend, this
        // means that we didn't try before and someone is probably waiting for
        // the signal
        qCWarning(KSCREEN) << "Launcher failed to load any backend: KScreen will be useless";
        delete mInterface;
        mInterface = 0;
        Q_EMIT backendReady(0);
        break;

    case BackendLoader::BackendAlreadyExists:
        // The launcher wrote service name to stdout, so backendReady() was emitted
        // from launcherDataAvailable(), nothing else to do here
        qCDebug(KSCREEN) << "Service for requested backend already running";
        break;
    }

    mLauncher->deleteLater();
};

void BackendManager::launcherDataAvailable()
{
    mLauncher->setReadChannel(QProcess::StandardOutput);
    QByteArray service = mLauncher->readLine();
    qCDebug(KSCREEN) << "launcherDataAvailable:" << service;
    mBackendService = QString::fromLatin1(service);

    mInterface = new org::kde::kscreen::Backend(mBackendService,
                                                QLatin1String("/"),
                                                QDBusConnection::sessionBus(),
                                                this);
    if (!mInterface->isValid()) {
        qCWarning(KSCREEN) << "Failed start KScreen backend service:" << QDBusConnection::sessionBus().lastError().message();
        delete mInterface;
        mInterface = 0;
    }

    Q_EMIT backendReady(mInterface);
}
