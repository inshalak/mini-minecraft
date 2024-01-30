#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), cHelp()
{
    ui->setupUi(this);
    ui->mygl->setFocus();
    this->playerInfoWindow.show();
    playerInfoWindow.move(QGuiApplication::primaryScreen()->availableGeometry().center() - this->rect().center() + QPoint(this->width() * 0.75, 0));

    connect(ui->mygl, SIGNAL(sig_sendPlayerPos(QString)), &playerInfoWindow, SLOT(slot_setPosText(QString)));
    connect(ui->mygl, SIGNAL(sig_sendPlayerVel(QString)), &playerInfoWindow, SLOT(slot_setVelText(QString)));
    connect(ui->mygl, SIGNAL(sig_sendPlayerAcc(QString)), &playerInfoWindow, SLOT(slot_setAccText(QString)));
    connect(ui->mygl, SIGNAL(sig_sendPlayerLook(QString)), &playerInfoWindow, SLOT(slot_setLookText(QString)));
    connect(ui->mygl, SIGNAL(sig_sendPlayerChunk(QString)), &playerInfoWindow, SLOT(slot_setChunkText(QString)));
    connect(ui->mygl, SIGNAL(sig_sendPlayerTerrainZone(QString)), &playerInfoWindow, SLOT(slot_setZoneText(QString)));

    //inventory
    connect(ui->mygl, SIGNAL(sig_openCloseInventory(bool)), this, SLOT(slot_openCloseInventory(bool)));
        connect(ui->mygl, SIGNAL(sig_sendInvGrass(int)), &inventoryWindow, SLOT(slot_setGrassNum(int)));
        connect(ui->mygl, SIGNAL(sig_sendInvDirt(int)), &inventoryWindow, SLOT(slot_setDirtNum(int)));
        connect(ui->mygl, SIGNAL(sig_sendInvStone(int)), &inventoryWindow, SLOT(slot_setStoneNum(int)));
        connect(ui->mygl, SIGNAL(sig_sendInvWater(int)), &inventoryWindow, SLOT(slot_setWaterNum(int)));
        connect(ui->mygl, SIGNAL(sig_sendInvSnow(int)), &inventoryWindow, SLOT(slot_setSnowNum(int)));
        connect(ui->mygl, SIGNAL(sig_sendInvLava(int)), &inventoryWindow, SLOT(slot_setLavaNum(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    cHelp.show();
}

void MainWindow::slot_openCloseInventory(bool inventoryStatus){
    if (inventoryStatus) {
        this->inventoryWindow.ui_main = ui;
        this->inventoryWindow.show();
        // open in center
        inventoryWindow.move(QPoint(this->width() * 0.5, this->height() * 0.5) +
                             QPoint(inventoryWindow.width() * 0.5, inventoryWindow.height() * 0.5));
    } else {
        this->inventoryWindow.close();
    }
}
