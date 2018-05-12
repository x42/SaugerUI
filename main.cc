#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>

#include "sauger/miio.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	if (miio_init () != 0) {
		QMessageBox messageBox;
		messageBox.critical(0, "Error", "Cannot initialize Sauger!");
		return -1;
	}

	MainWindow w;
	w.show();

	return a.exec();
}
