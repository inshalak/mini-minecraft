/********************************************************************************
** Form generated from reading UI file 'inventory.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INVENTORY_H
#define UI_INVENTORY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Inventory
{
public:
    QFrame *line;
    QLabel *label;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLCDNumber *waterNumber;
    QPushButton *pushButton_2;
    QLabel *lava;
    QLabel *stone;
    QLCDNumber *grassNumber;
    QLCDNumber *lavaNumber;
    QLabel *dirt;
    QPushButton *pushButton;
    QLabel *water;
    QLabel *snow;
    QLabel *grass;
    QLCDNumber *dirtNumber;
    QLCDNumber *snowNumber;
    QLCDNumber *stoneNumber;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QWidget *gridLayoutWidget_2;
    QGridLayout *gridLayout_2;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QPushButton *pushButton_10;
    QPushButton *pushButton_11;
    QPushButton *craftButton;
    QWidget *gridLayoutWidget_3;
    QGridLayout *gridLayout_3;
    QLabel *label_2;

    void setupUi(QWidget *Inventory)
    {
        if (Inventory->objectName().isEmpty())
            Inventory->setObjectName(QString::fromUtf8("Inventory"));
        Inventory->resize(653, 352);
        line = new QFrame(Inventory);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(10, 20, 621, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        label = new QLabel(Inventory);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 0, 621, 31));
        QFont font;
        font.setPointSize(12);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);
        gridLayoutWidget = new QWidget(Inventory);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(20, 190, 622, 151));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        waterNumber = new QLCDNumber(gridLayoutWidget);
        waterNumber->setObjectName(QString::fromUtf8("waterNumber"));

        gridLayout->addWidget(waterNumber, 1, 3, 1, 1);

        pushButton_2 = new QPushButton(gridLayoutWidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        gridLayout->addWidget(pushButton_2, 3, 1, 1, 1);

        lava = new QLabel(gridLayoutWidget);
        lava->setObjectName(QString::fromUtf8("lava"));
        lava->setPixmap(QPixmap(QString::fromUtf8(":/textures/lava.png")));

        gridLayout->addWidget(lava, 0, 5, 1, 1);

        stone = new QLabel(gridLayoutWidget);
        stone->setObjectName(QString::fromUtf8("stone"));
        stone->setPixmap(QPixmap(QString::fromUtf8(":/textures/stone.png")));

        gridLayout->addWidget(stone, 0, 2, 1, 1);

        grassNumber = new QLCDNumber(gridLayoutWidget);
        grassNumber->setObjectName(QString::fromUtf8("grassNumber"));

        gridLayout->addWidget(grassNumber, 1, 0, 1, 1);

        lavaNumber = new QLCDNumber(gridLayoutWidget);
        lavaNumber->setObjectName(QString::fromUtf8("lavaNumber"));

        gridLayout->addWidget(lavaNumber, 1, 5, 1, 1);

        dirt = new QLabel(gridLayoutWidget);
        dirt->setObjectName(QString::fromUtf8("dirt"));
        dirt->setPixmap(QPixmap(QString::fromUtf8(":/textures/dirt.png")));

        gridLayout->addWidget(dirt, 0, 1, 1, 1);

        pushButton = new QPushButton(gridLayoutWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        gridLayout->addWidget(pushButton, 3, 0, 1, 1);

        water = new QLabel(gridLayoutWidget);
        water->setObjectName(QString::fromUtf8("water"));
        water->setPixmap(QPixmap(QString::fromUtf8(":/textures/water.png")));

        gridLayout->addWidget(water, 0, 3, 1, 1);

        snow = new QLabel(gridLayoutWidget);
        snow->setObjectName(QString::fromUtf8("snow"));
        snow->setPixmap(QPixmap(QString::fromUtf8(":/textures/snow.png")));

        gridLayout->addWidget(snow, 0, 4, 1, 1);

        grass = new QLabel(gridLayoutWidget);
        grass->setObjectName(QString::fromUtf8("grass"));
        grass->setPixmap(QPixmap(QString::fromUtf8(":/textures/grass.png")));

        gridLayout->addWidget(grass, 0, 0, 1, 1);

        dirtNumber = new QLCDNumber(gridLayoutWidget);
        dirtNumber->setObjectName(QString::fromUtf8("dirtNumber"));

        gridLayout->addWidget(dirtNumber, 1, 1, 1, 1);

        snowNumber = new QLCDNumber(gridLayoutWidget);
        snowNumber->setObjectName(QString::fromUtf8("snowNumber"));

        gridLayout->addWidget(snowNumber, 1, 4, 1, 1);

        stoneNumber = new QLCDNumber(gridLayoutWidget);
        stoneNumber->setObjectName(QString::fromUtf8("stoneNumber"));

        gridLayout->addWidget(stoneNumber, 1, 2, 1, 1);

        pushButton_3 = new QPushButton(gridLayoutWidget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

        gridLayout->addWidget(pushButton_3, 3, 2, 1, 1);

        pushButton_4 = new QPushButton(gridLayoutWidget);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));

        gridLayout->addWidget(pushButton_4, 3, 3, 1, 1);

        pushButton_5 = new QPushButton(gridLayoutWidget);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));

        gridLayout->addWidget(pushButton_5, 3, 4, 1, 1);

        pushButton_6 = new QPushButton(gridLayoutWidget);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));

        gridLayout->addWidget(pushButton_6, 3, 5, 1, 1);

        gridLayoutWidget_2 = new QWidget(Inventory);
        gridLayoutWidget_2->setObjectName(QString::fromUtf8("gridLayoutWidget_2"));
        gridLayoutWidget_2->setGeometry(QRect(20, 40, 181, 121));
        gridLayout_2 = new QGridLayout(gridLayoutWidget_2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        pushButton_8 = new QPushButton(gridLayoutWidget_2);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));

        gridLayout_2->addWidget(pushButton_8, 0, 0, 1, 1);

        pushButton_9 = new QPushButton(gridLayoutWidget_2);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));

        gridLayout_2->addWidget(pushButton_9, 2, 0, 1, 1);

        pushButton_10 = new QPushButton(gridLayoutWidget_2);
        pushButton_10->setObjectName(QString::fromUtf8("pushButton_10"));

        gridLayout_2->addWidget(pushButton_10, 0, 1, 1, 1);

        pushButton_11 = new QPushButton(gridLayoutWidget_2);
        pushButton_11->setObjectName(QString::fromUtf8("pushButton_11"));

        gridLayout_2->addWidget(pushButton_11, 2, 1, 1, 1);

        craftButton = new QPushButton(Inventory);
        craftButton->setObjectName(QString::fromUtf8("craftButton"));
        craftButton->setGeometry(QRect(210, 90, 80, 18));
        gridLayoutWidget_3 = new QWidget(Inventory);
        gridLayoutWidget_3->setObjectName(QString::fromUtf8("gridLayoutWidget_3"));
        gridLayoutWidget_3->setGeometry(QRect(310, 60, 91, 81));
        gridLayout_3 = new QGridLayout(gridLayoutWidget_3);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(gridLayoutWidget_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_3->addWidget(label_2, 0, 0, 1, 1);


        retranslateUi(Inventory);

        QMetaObject::connectSlotsByName(Inventory);
    } // setupUi

    void retranslateUi(QWidget *Inventory)
    {
        Inventory->setWindowTitle(QCoreApplication::translate("Inventory", "Form", nullptr));
        label->setText(QCoreApplication::translate("Inventory", "Inventory", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Inventory", "USE DIRT", nullptr));
        lava->setText(QString());
        stone->setText(QString());
        dirt->setText(QString());
        pushButton->setText(QCoreApplication::translate("Inventory", "USE GRASS", nullptr));
        water->setText(QString());
        snow->setText(QString());
        grass->setText(QString());
        pushButton_3->setText(QCoreApplication::translate("Inventory", "USE STONE", nullptr));
        pushButton_4->setText(QCoreApplication::translate("Inventory", "USE WATER", nullptr));
        pushButton_5->setText(QCoreApplication::translate("Inventory", "USE SNOW", nullptr));
        pushButton_6->setText(QCoreApplication::translate("Inventory", "USE LAVA", nullptr));
        pushButton_8->setText(QCoreApplication::translate("Inventory", "select block", nullptr));
        pushButton_9->setText(QCoreApplication::translate("Inventory", "select block", nullptr));
        pushButton_10->setText(QCoreApplication::translate("Inventory", "select block", nullptr));
        pushButton_11->setText(QCoreApplication::translate("Inventory", "select block", nullptr));
        craftButton->setText(QCoreApplication::translate("Inventory", "CRAFT IT -->", nullptr));
        label_2->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Inventory: public Ui_Inventory {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INVENTORY_H
