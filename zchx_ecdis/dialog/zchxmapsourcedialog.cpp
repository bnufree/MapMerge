#include "zchxmapsourcedialog.h"
#include "ui_zchxmapsourcedialog.h"
#include <QFileDialog>
#include "zchxMapDataUtils.h"

using namespace qt;

zchxMapSourceDialog::zchxMapSourceDialog(QWidget *parent) :
    QDialog(parent),
    mUrl(""),
    mPos(0),
    ui(new Ui::zchxMapSourceDialog)
{
    ui->setupUi(this);
    ui->pos->addItem(tr("左上"), TILE_ORIGIN_TOPLEFT);
    ui->pos->addItem(tr("左下"), TILE_ORIGIN_BOTTEMLEFT);
    this->setAttribute(Qt::WA_DeleteOnClose);
}

zchxMapSourceDialog::~zchxMapSourceDialog()
{
    delete ui;
}

void zchxMapSourceDialog::on_browse_clicked()
{
    QString url = QFileDialog::getExistingDirectory();
    if(url.isEmpty()) return;
    ui->url->setText(url);
    mUrl = url;
}

void zchxMapSourceDialog::on_pos_currentIndexChanged(int index)
{
    mPos = index;
}


void zchxMapSourceDialog::on_ok_clicked()
{
    accept();
}

void zchxMapSourceDialog::on_cancel_clicked()
{
    reject();
}
