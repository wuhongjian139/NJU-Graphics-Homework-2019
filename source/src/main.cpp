//  命令行接口
#include "cli.h"
//  用户交互接口
#include "scribblearea.h"
#include <QApplication>
#include <iostream>
#include <QTranslator>
using namespace std;
//#include <QtCore/QtPlugin>
//Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
int main(int argc, char* argv[]) {
	if (argc >= 2) {
		cout << "in" << endl;
		Cli cli;
		if (!cli.handleScript(argv[argc - 1])) {
			cout << "error open file" << endl;
		}
		return 0;
	}
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	QApplication::setOrganizationName(QObject::tr("南京大学计算机科学与技术系"));
	QApplication::setApplicationVersion(QObject::tr("版本-2019年12月"));
	QApplication::setApplicationName(QObject::tr("图形学课程作业_171860633_徐国栋"));
	QApplication app(argc, argv);
	QTranslator translator;
	if (translator.load(":/Resources/qt_zh_CN.qm"))
		app.installTranslator(&translator);
	else {
		return throwErrorBox("Error Message", "Fail to load(\":/Resources/qt_zh_CN.qm\")!");
	}
	ScribbleArea w;
	w.resize(getDesktopSize() / 1.5);
	w.show();
	return app.exec();
}

/*
version.lib
Netapi32.lib
userenv.lib
Dwmapi.lib
wtsapi32.lib
Wsock32.lib
Imm32.lib
Winmm.lib
ws2_32.lib
Qt5AccessibilitySupport.lib
Qt5Network.lib
Qt5Widgets.lib
Qt5Core.lib
Qt5Gui.lib
Qt5PlatformCompositorSupport.lib
Qt5EventDispatcherSupport.lib
Qt5FontDatabaseSupport.lib
Qt5ThemeSupport.lib
Qt5WindowsUIAutomationSupport.lib
Qt5OpenGL.lib
Qt5OpenGLExtensions.lib
opengl32.lib
glu32.lib
qtmain.lib
qwindows.lib
qgif.lib
qico.lib
qjpeg.lib
qtfreetype.lib
qtharfbuzz.lib
qtlibpng.lib
qsvgicon.lib
qicns.lib
qsvg.lib
qtga.lib
qtiff.lib
qwbmp.lib
qwebp.lib
qtpcre2.lib
*/
