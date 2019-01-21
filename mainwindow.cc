#include <QMessageBox>
#include <QTime>
#include <QMutexLocker>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, _timer(this)
{
	ui->setupUi(this);
	update_device_info();
	connect(&_timer, SIGNAL(timeout()), this, SLOT(update_device_info()));
	_timer.start(60000);
}

MainWindow::~MainWindow()
{
	miio_cleanup ();
	delete ui;
}

void MainWindow::on_pushButtonStart_clicked()
{
	check_ok (locked_miio_cmd ("app_start", ""));
}

void MainWindow::on_pushButtonPause_clicked()
{
	check_ok (locked_miio_cmd ("app_pause", ""));
}

void MainWindow::on_pushButtonHome_clicked()
{
	json_value_free (locked_miio_cmd ("app_stop", ""));
	check_ok (locked_miio_cmd ("app_charge", ""));
}

void MainWindow::on_pushButtonFind_clicked()
{
	check_ok (locked_miio_cmd ("find_me", ""));
}

void MainWindow::on_pushButtonRstSide_clicked()
{
	reset_consumable("Side Brush Work Time", "\"params\":[\"side_brush_work_time\"]");
}

void MainWindow::on_pushButtonRstMain_clicked()
{
	reset_consumable("Main Brush Work Time", "\"params\":[\"main_brush_work_time\"]");
}

void MainWindow::on_pushButtonRstFilter_clicked()
{
	reset_consumable("Filter Work Time", "\"params\":[\"filter_work_time\"]");
}

void MainWindow::on_pushButtonRstSensor_clicked()
{
	reset_consumable("Sensor Dirty Time", "\"params\":[\"sensor_dirty_time\"]");
}

void MainWindow::on_tme_now_clicked()
{
	update_device_info ();
}

void MainWindow::on_sliderFanSpeed_valueChanged(int value)
{
	char param[32];
	snprintf (param, sizeof(param), "\"params\":[%d]", value);
	check_ok (locked_miio_cmd ("set_custom_mode", param));
}

static QString format_time (int sec)
{
	if (sec < 0) {
		return "--";
	}
	int dd = sec / 86400;
	int hh = (sec % 86400) / 3600;
	int mm = (sec % 3600) / 60;
	int ss = sec % 60;
	if (dd > 1) {
		return QString("").sprintf("%d days, %02d:%02d:%02d", dd, hh, mm, ss);
	}
	else if (dd > 0) {
		return QString("").sprintf("%d day, %02d:%02d:%02d", dd, hh, mm, ss);
	}
	else {
		return QString("").sprintf("%02d:%02d:%02d", hh, mm, ss);
	}
}

void MainWindow::update_device_info()
{
	QTime now = QTime::currentTime();
	ui->tme_now->setText (now.toString("hh:mm"));

	json_value* val =  locked_miio_cmd ("get_status", "");
	if (val &&  (*val)["result"][0].type == json_object) {
		const json_value& v = (*val)["result"][0];
		json_int_t bat = v["battery"];
		json_int_t fan = v["fan_power"];
		json_int_t area = v["clean_area"];
		json_int_t secs = v["clean_time"];
		json_int_t state = v["state"];
		json_int_t error = v["error_code"];
		QString status (vac_status(state));
		if (error > 0) {
			status += ": ";
			status += vac_error (error);
		}
		if (fan > 0 && fan < 100) {
			ui->sliderFanSpeed->blockSignals(true);
			ui->sliderFanSpeed->setValue (fan);
			ui->sliderFanSpeed->blockSignals(false);
		}

		ui->nfo_state->setText(status);
		ui->nfo_battery->setText(QString ("%1 %").arg(bat));
		ui->nfo_fanspeed->setText(QString("%1 %").arg(fan));
		ui->nfo_area->setText(QString ().sprintf("%.2f m²", area / 1000000.0));
		ui->nfo_clean->setText(format_time (secs));
		ui->groupBoxAction->setDisabled(false);
	} else {
		ui->nfo_battery->setText("");
		ui->nfo_fanspeed->setText("");
		ui->nfo_area->setText("");
		ui->nfo_clean->setText("");
		ui->nfo_state->setText("Unkown");
		ui->groupBoxAction->setDisabled(true);
	}
	json_value_free (val);

	val =  locked_miio_cmd ("get_consumable", "");
	if (val &&  (*val)["result"][0].type == json_object) {
		const json_value& v = (*val)["result"][0];
		json_int_t sec_main = v["main_brush_work_time"];
		json_int_t sec_side = v["side_brush_work_time"];
		json_int_t sec_filt = v["filter_work_time"];
		json_int_t sec_sens = v["sensor_dirty_time"];
		ui->con_mainbrush->setValue(std::min(100.0, 100.0 * sec_main / (300 * 3600)));
		ui->con_sidebrush->setValue(std::min(100.0, 100.0 * sec_side / (200 * 3600)));
		ui->con_filter->setValue(std::min(100.0, 100.0 * sec_filt / (150 * 3600)));
		ui->con_sensor->setValue(std::min(100.0, 100.0 * sec_sens / (30 * 3600)));

		ui->tme_sensor->setText(QString("%1 (left: %2)")
				.arg(format_time(sec_sens))
				.arg(format_time(30 * 3600 - sec_sens)));
		ui->tme_filter->setText(QString("%1 (left: %2)")
				.arg(format_time(sec_filt))
				.arg(format_time(150 * 3600 - sec_filt)));
		ui->tme_mainbrush->setText(QString("%1 (left: %2)")
				.arg(format_time(sec_main))
				.arg(format_time(300 * 3600 - sec_main)));
		ui->tme_sidebrush->setText(QString("%1 (left: %2)")
				.arg(format_time(sec_side))
				.arg(format_time(200 * 3600 - sec_side)));
	} else {
		ui->con_mainbrush->setValue(0);
		ui->con_sidebrush->setValue(0);
		ui->con_filter->setValue(0);
		ui->con_sensor->setValue(0);
		ui->tme_mainbrush->setText("");
		ui->tme_sidebrush->setText("");
		ui->tme_filter->setText("");
		ui->tme_sensor->setText("");
	}
	json_value_free (val);
}

void MainWindow::check_ok (json_value* val)
{
	bool ok = false;
	if (val && (*val)["result"][0].type == json_string) {
		ok = ! strcmp ((*val)["result"][0], "ok");
	}
	if (!ok) {
		QMessageBox msg;
		msg.critical(this, "Error", "Command failed.");
	}
	json_value_free (val);
	QTimer::singleShot(1000, this, SLOT(update_device_info()));
}

void MainWindow::reset_consumable (const char* what, const char* param)
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "Confirm",
			QString ("Really reset consumable timer of \"%1\"?"). arg(what),
			QMessageBox::Yes|QMessageBox::No);
	if (reply == QMessageBox::Yes) {
		json_value_free (locked_miio_cmd ("reset_consumable", param));
	}
	QTimer::singleShot(1000, this, SLOT(update_device_info()));
}

json_value* MainWindow::locked_miio_cmd (const char* cmd, const char* opt)
{
	QMutexLocker locker(&_lock);
	return miio_cmd (cmd, opt);
}
