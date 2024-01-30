#include <QKeyEvent>
#include <QPixmap>
#include "inventory.h"
#include "ui_inventory.h"

Inventory::Inventory(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Inventory)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(slot_selectedBlockGrass()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(slot_selectedBlockDirt()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(slot_selectedBlockStone()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(slot_selectedBlockWater()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(slot_selectedBlockSnow()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(slot_selectedBlockLava()));


    createMenusForCrafting();


    connect(ui -> pushButton_8 -> menu(), SIGNAL(triggered(QAction*)), this, SLOT(slot_showSelected8(QAction*)));
    connect(ui -> pushButton_9 -> menu(), SIGNAL(triggered(QAction*)), this, SLOT(slot_showSelected9(QAction*)));
    connect(ui -> pushButton_10 -> menu(), SIGNAL(triggered(QAction*)), this, SLOT(slot_showSelected10(QAction*)));
    connect(ui -> pushButton_11 -> menu(), SIGNAL(triggered(QAction*)), this, SLOT(slot_showSelected11(QAction*)));

    connect(ui -> craftButton, SIGNAL(clicked()), this, SLOT(slot_craft()));



}

void Inventory::slot_craft(){
    QPixmap stoneBrick(":/textures/stonebrick.png");
    ui -> label_2 -> setPixmap(stoneBrick.scaled(QSize(50, 50)));
}

void Inventory::createMenusForCrafting(){
    QMenu* menu = new QMenu(this);
    QMenu* menu1 = new QMenu(this);
    QMenu* menu2 = new QMenu(this);
    QMenu* menu3 = new QMenu(this);
    QAction* grass = new QAction("Grass", this);
    grass ->setIcon(QIcon(":/textures/grass.png"));

    QAction* dirt = new QAction("Dirt", this);
    dirt ->setIcon(QIcon(":/textures/dirt.png"));

    QAction* snow = new QAction("Snow", this);
    snow ->setIcon(QIcon(":/textures/snow.png"));

    QAction* water = new QAction("Water", this);
    water ->setIcon(QIcon(":/textures/water.png"));

    QAction* lava = new QAction("Lava", this);
    lava ->setIcon(QIcon(":/textures/lava.png"));

    QAction* stone = new QAction("Stone", this);
    stone ->setIcon(QIcon(":/textures/stone.png"));

    QAction* grass3 = new QAction("Grass", this);
    grass3 ->setIcon(QIcon(":/textures/grass.png"));

    QAction* dirt3 = new QAction("Dirt", this);
    dirt3 ->setIcon(QIcon(":/textures/dirt.png"));

    QAction* snow3 = new QAction("Snow", this);
    snow3 ->setIcon(QIcon(":/textures/snow.png"));

    QAction* water3 = new QAction("Water", this);
    water3 ->setIcon(QIcon(":/textures/water.png"));

    QAction* lava3 = new QAction("Lava", this);
    lava3 ->setIcon(QIcon(":/textures/lava.png"));

    QAction* stone3 = new QAction("Stone", this);
    stone3 ->setIcon(QIcon(":/textures/stone.png"));

    QAction* grass2 = new QAction("Grass", this);
    grass2 ->setIcon(QIcon(":/textures/grass.png"));

    QAction* dirt2 = new QAction("Dirt", this);
    dirt2 ->setIcon(QIcon(":/textures/dirt.png"));

    QAction* snow2 = new QAction("Snow", this);
    snow2 ->setIcon(QIcon(":/textures/snow.png"));

    QAction* water2 = new QAction("Water", this);
    water2 ->setIcon(QIcon(":/textures/water.png"));

    QAction* lava2 = new QAction("Lava", this);
    lava2 ->setIcon(QIcon(":/textures/lava.png"));

    QAction* stone2 = new QAction("Stone", this);
    stone2 ->setIcon(QIcon(":/textures/stone.png"));

    QAction* grass1 = new QAction("Grass", this);
    grass1 ->setIcon(QIcon(":/textures/grass.png"));

    QAction* dirt1 = new QAction("Dirt", this);
    dirt1 ->setIcon(QIcon(":/textures/dirt.png"));

    QAction* snow1 = new QAction("Snow", this);
    snow1 ->setIcon(QIcon(":/textures/snow.png"));

    QAction* water1 = new QAction("Water", this);
    water1 ->setIcon(QIcon(":/textures/water.png"));

    QAction* lava1 = new QAction("Lava", this);
    lava1 ->setIcon(QIcon(":/textures/lava.png"));

    QAction* stone1 = new QAction("Stone", this);
    stone1 ->setIcon(QIcon(":/textures/stone.png"));

    menu->addAction(grass);
    menu->addAction(dirt);
    menu->addAction(stone);
    menu->addAction(snow);
    menu->addAction(water);
    menu->addAction(lava);

    menu1->addAction(grass1);
    menu1->addAction(dirt1);
    menu1->addAction(stone1);
    menu1->addAction(snow1);
    menu1->addAction(water1);
    menu1->addAction(lava1);

    menu2->addAction(grass2);
    menu2->addAction(dirt2);
    menu2->addAction(stone2);
    menu2->addAction(snow2);
    menu2->addAction(water2);
    menu2->addAction(lava2);

    menu3->addAction(grass3);
    menu3->addAction(dirt3);
    menu3->addAction(stone3);
    menu3->addAction(snow3);
    menu3->addAction(water3);
    menu3->addAction(lava3);
    ui->pushButton_8->setMenu(menu);
    ui->pushButton_9->setMenu(menu1);
    ui->pushButton_10->setMenu(menu2);
    ui->pushButton_11->setMenu(menu3);


}

void Inventory::slot_showSelected8(QAction* active){
    ui -> pushButton_8 -> setIcon(active->icon());
    ui -> pushButton_8->setIconSize(QSize(60,60));
    ui -> pushButton_8 -> setText("");


}

void Inventory::slot_showSelected9(QAction* active){
    ui -> pushButton_9 -> setIcon(active->icon());
    ui -> pushButton_9->setIconSize(QSize(60,60));
    ui -> pushButton_9 -> setText("");
}

void Inventory::slot_showSelected10(QAction* active){
    ui -> pushButton_10 -> setIcon(active->icon());
    ui -> pushButton_10->setIconSize(QSize(60,60));
    ui -> pushButton_10 -> setText("");
}

void Inventory::slot_showSelected11(QAction* active){
    ui -> pushButton_11 -> setIcon(active->icon());
    ui -> pushButton_11 -> setIconSize(QSize(60,60));
    ui -> pushButton_11 -> setText("");
}



Inventory::~Inventory() {
    delete ui;
}

// close the inventory
void Inventory::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_I) {
        this->close();
    }
}


void Inventory::slot_setGrassNum(int n) {
    ui->grassNumber->display(n);
}

void Inventory::slot_setDirtNum(int n) {
    ui->dirtNumber->display(n);
}

void Inventory::slot_setStoneNum(int n) {
    ui->stoneNumber->display(n);
}


void Inventory::slot_setWaterNum(int n) {
    ui->waterNumber->display(n);
}


void Inventory::slot_setLavaNum(int n) {
    ui->lavaNumber->display(n);
}



void Inventory::slot_setSnowNum(int n) {
    ui->snowNumber->display(n);
}

// set current block
void Inventory::slot_selectedBlockGrass() {
    ui_main->mygl->m_inventorySelectedBlock = GRASS;
}

void Inventory::slot_selectedBlockDirt() {
    ui_main->mygl->m_inventorySelectedBlock = DIRT;
}

void Inventory::slot_selectedBlockStone() {
    ui_main->mygl->m_inventorySelectedBlock = STONE;
}

void Inventory::slot_selectedBlockWater() {
    ui_main->mygl->m_inventorySelectedBlock = WATER;
}

void Inventory::slot_selectedBlockLava() {
    ui_main->mygl->m_inventorySelectedBlock = LAVA;
}

void Inventory::slot_selectedBlockSnow() {
    ui_main->mygl->m_inventorySelectedBlock = SNOW;
}
