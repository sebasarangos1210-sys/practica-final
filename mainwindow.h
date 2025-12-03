#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QTimer>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include "gameengine.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateGame();
    void launchProjectile();
    void updateAngleLabel(int value);
    void updateSpeedLabel(int value);

private:
    QGraphicsScene *scene;
    QGraphicsView *view;
    QTimer *timer;

    QSlider *angleSlider;
    QSlider *speedSlider;
    QPushButton *launchButton;
    QLabel *angleLabel;
    QLabel *speedLabel;
    QLabel *playerLabel;
    QLabel *statusLabel;
    QLabel *bouncesLabel;  // NUEVO: Etiqueta para mostrar rebotes restantes

    GameEngine *engine;

    QGraphicsEllipseItem *projectileItem;
    QVector<QGraphicsRectItem*> player1InfraItems;
    QVector<QGraphicsRectItem*> player2InfraItems;
    QVector<QGraphicsTextItem*> resistanceLabels1;
    QVector<QGraphicsTextItem*> resistanceLabels2;

    void setupUI();
    void setupGame();
    void renderScene();
    void updateResistanceLabels();
};

#endif // MAINWINDOW_H
