/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <signal.h>

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDBusConnection>
#include<sys/utsname.h>
#include <DLog>

#include "lftmanager.h"
#include "anything_adaptor.h"

void handleSIGTERM(int sig)
{
    qDebug() << sig;

    if (qApp) {
        qApp->quit();
    }
}

DCORE_USE_NAMESPACE

static QString logFormat = "[%{time}{yyyy-MM-dd, HH:mm:ss.zzz}] [%{type:-7}] [%{file}=>%{function}: %{line}] %{message}\n";

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    app.setOrganizationName("deepin");

    //read the kernel version
    struct utsname uts;
    if (uname(&uts) >= 0) {
        qWarning() << "the kernel version:" << uts.release;
        QString s_release(uts.release);
        if (s_release.startsWith("5.10")) {
            QFile file_mountinfo("/proc/self/mountinfo");
            QByteArray t_info;
            if (!file_mountinfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qWarning() << "open file" << file_mountinfo.Text << "failed";
            } else {
                t_info = file_mountinfo.readAll();
                file_mountinfo.close();
            }

            QFile file_drv("/dev/driver_set_info");
            char buf[4096 * 2]; //缓冲区

            if (!file_drv.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qWarning() << "open driver_set_info file" << file_drv.Text << "failed";
            } else {
                //写信息
                memcpy(buf, t_info, t_info.size());
                qWarning() << "write file info:" << buf;
                file_drv.write(buf);
                file_drv.close();
            }
        }
    }
    // init log
    ConsoleAppender *consoleAppender = new ConsoleAppender;
    consoleAppender->setFormat(logFormat);

    RollingFileAppender *rollingFileAppender = new RollingFileAppender(LFTManager::cacheDir() + "/app.log");
    rollingFileAppender->setFormat(logFormat);
    rollingFileAppender->setLogFilesLimit(5);
    rollingFileAppender->setDatePattern(RollingFileAppender::DailyRollover);

    logger->registerAppender(consoleAppender);
    logger->registerAppender(rollingFileAppender);

    for (const QString &c : LFTManager::logCategoryList()) {
        logger->registerCategoryAppender(c, consoleAppender);
        logger->registerCategoryAppender(c, rollingFileAppender);
    }

    QCommandLineOption option_dbus("dbus", "Start on DBus mode.");
    QCommandLineParser parser;

    parser.addOption(option_dbus);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    if (parser.isSet(option_dbus)) {
        if (!QDBusConnection::systemBus().isConnected()) {
            qWarning("Cannot connect to the D-Bus session bus.\n"
                     "Please check your system settings and try again.\n");
            return 1;
        }

        // add our D-Bus interface and connect to D-Bus
        if (!QDBusConnection::systemBus().registerService("com.deepin.anything")) {
            qWarning("Cannot register the \"com.deepin.anything\" service.\n");
            return 2;
        }

        Q_UNUSED(new AnythingAdaptor(LFTManager::instance()));

        if (!QDBusConnection::systemBus().registerObject("/com/deepin/anything", LFTManager::instance())) {
            qWarning("Cannot register to the D-Bus object: \"/com/deepin/anything\"\n");
            return 3;
        }
    } else {
        return 0;
    }

    signal(SIGTERM, handleSIGTERM);

    return app.exec();
}
