/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include <QApplication>
#include <QMessageBox>
#include <QSplashScreen>
#include <QTimer>
#include <QLibraryInfo>
#include <QTranslator>
#include <QLocale>
#include <QFileOpenEvent>
#include "ComicMainWindow.h"
#include "ComicBookSettings.h"
#include "ComicFrameList.h"
#include "ImageJobResult.h"
#include "Thumbnail.h"
#include "Page.h"
#include "config.h"

//! directory with QComicBook translations
static QString translationsDir()
{
#ifdef Q_OS_MACOS
	// inside the app bundle: QComicBook.app/Contents/Resources/i18n
	return QCoreApplication::applicationDirPath() + "/../Resources/i18n";
#else
	return DATADIR "/i18n";
#endif
}

//! opens files that macOS sends as events (Finder "Open With", drop on the Dock icon)
class FileOpenFilter: public QObject
{
	public:
		FileOpenFilter(QComicBook::ComicMainWindow *win): QObject(win), m_win(win) {}

	protected:
		bool eventFilter(QObject *obj, QEvent *e)
		{
			if (e->type() == QEvent::FileOpen)
			{
				m_win->open(static_cast<QFileOpenEvent*>(e)->file());
				return true;
			}
			return QObject::eventFilter(obj, e);
		}

	private:
		QComicBook::ComicMainWindow *m_win;
};

int main(int argc, char *argv[])
{
	using namespace QComicBook;

#ifdef Q_OS_MACOS
	//
	// apps started from Finder get a minimal PATH: add the Homebrew and MacPorts
	// directories, so external archivers (7z, rar, ...) installed there are found
	QByteArray path = qgetenv("PATH");
	const QList<QByteArray> pathDirs = path.split(':');
	for (const char *dir: {"/opt/homebrew/bin", "/usr/local/bin", "/opt/local/bin"})
	{
		if (!pathDirs.contains(dir))
			path += QByteArray(":") + dir;
	}
	qputenv("PATH", path);
#endif

	QApplication app(argc, argv);
        app.setApplicationName("QComicBook");
        app.setOrganizationName("PawelStolowski");
        app.setOrganizationDomain("linux-projects.net");

        //
        // load translation
        QTranslator qtTrans;
        if (qtTrans.load("qt_" + QLocale::system().name(), QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
            app.installTranslator(&qtTrans);

        QTranslator qcomicbookTrans;
        if (qcomicbookTrans.load("qcomicbook_" + QLocale::system().name(), translationsDir()))
            app.installTranslator(&qcomicbookTrans);

	const QString errcaption = ComicMainWindow::tr("QComicBook error");

        qRegisterMetaType<Page>("Page");
        qRegisterMetaType<Thumbnail>("Thumbnail");
        qRegisterMetaType<ComicFrameList>("ComicFrameList");
        qRegisterMetaType<ImageJobResult>("ImageJobResult");

	ComicBookSettings::instance().load();

	
	if (!ComicBookSettings::instance().checkDirs())
        {
		QMessageBox::critical(NULL, errcaption, ComicMainWindow::tr("Can't initialize QComicBook directories"),
				QMessageBox::Ok, QMessageBox::NoButton);
        }
	
	ComicMainWindow *win = new ComicMainWindow(NULL);
	//app.setMainWidget(win);
	win->show();
	app.installEventFilter(new FileOpenFilter(win));

	//
	// show splashscreen
	if (ComicBookSettings::instance().showSplash())
	{
		QPixmap splashpix(":/images/qcomicbook-splash.png");
		if (!splashpix.isNull())
		{
                    QSplashScreen *splash = new QSplashScreen(splashpix, Qt::WindowStaysOnTopHint);
                    splash->show();

                    //
                    // close splashscreen after a few seconds
                    QTimer *timer = new QTimer(win);
                    QObject::connect(timer, SIGNAL(timeout()), splash, SLOT(close()));
                    timer->setSingleShot(true);
                    timer->start(2*1000);
		}
	}

	//
	// command line argument
	if (app.arguments().size() > 1)
	{ 
		win->open(app.arguments().at(1));
	}

	return app.exec();
}

