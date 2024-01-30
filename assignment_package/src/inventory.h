#ifndef INVENTORY_H
#define INVENTORY_H

#include "qpushbutton.h"
#include <QWidget>

#include <ui_mainwindow.h>

namespace Ui {
class Inventory;
}

class Inventory : public QWidget
{
    Q_OBJECT
public:
    explicit Inventory(QWidget *parent = nullptr);
    virtual ~Inventory();

    void keyPressEvent(QKeyEvent *e);
    Ui::MainWindow *ui_main;
    void createMenusForCrafting();
public slots:
    void slot_setGrassNum(int);
    void slot_setDirtNum(int);
    void slot_setStoneNum(int);
    void slot_setWaterNum(int);
    void slot_setSnowNum(int);
    void slot_setLavaNum(int);


    void slot_selectedBlockGrass();
    void slot_selectedBlockDirt();
    void slot_selectedBlockStone();
    void slot_selectedBlockWater();
    void slot_selectedBlockSnow();
    void slot_selectedBlockLava();

    void slot_showSelected8(QAction*);
    void slot_showSelected9(QAction*);
    void slot_showSelected10(QAction*);
    void slot_showSelected11(QAction*);

    void slot_craft();


private:
    Ui::Inventory *ui;

};

#endif // INVENTORY_H
