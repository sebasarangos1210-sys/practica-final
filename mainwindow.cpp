#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QBrush>
#include <QPen>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    projectileItem(nullptr),
    engine(nullptr)
{

    // Inicializar timer ANTES de setupUI
    timer = new QTimer(this);
    timer->setInterval(16);  // ~60 FPS
    connect(timer, &QTimer::timeout, this, &MainWindow::updateGame);

    setupUI();
    setupGame();

    // Verificar que todos los widgets se crearon
    qDebug() << "Verificando widgets:";
    qDebug() << "  bouncesLabel:" << (bouncesLabel != nullptr ? "OK" : "NULL");
    qDebug() << "  launchButton:" << (launchButton != nullptr ? "OK" : "NULL");
    qDebug() << "  statusLabel:" << (statusLabel != nullptr ? "OK" : "NULL");

    qDebug() << "MainWindow inicializado correctamente";
}

MainWindow::~MainWindow()
{
    delete engine;
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 800, 600);
    scene->setBackgroundBrush(QBrush(QColor(135, 206, 235)));

    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(view);

    QGroupBox *controlBox = new QGroupBox("Controles de Lanzamiento");
    QHBoxLayout *controlLayout = new QHBoxLayout(controlBox);

    QVBoxLayout *angleLayout = new QVBoxLayout();
    angleLabel = new QLabel("Ángulo: 45°");
    angleSlider = new QSlider(Qt::Horizontal);
    angleSlider->setRange(0, 90);
    angleSlider->setValue(45);
    angleLayout->addWidget(angleLabel);
    angleLayout->addWidget(angleSlider);
    controlLayout->addLayout(angleLayout);

    QVBoxLayout *speedLayout = new QVBoxLayout();
    speedLabel = new QLabel("Velocidad: 150");
    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(50, 300);
    speedSlider->setValue(150);
    speedLayout->addWidget(speedLabel);
    speedLayout->addWidget(speedSlider);
    controlLayout->addLayout(speedLayout);

    launchButton = new QPushButton("LANZAR");
    launchButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 10px; }");
    controlLayout->addWidget(launchButton);

    mainLayout->addWidget(controlBox);

    QHBoxLayout *statusLayout = new QHBoxLayout();
    playerLabel = new QLabel("Turno: Jugador 1");
    playerLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    statusLabel = new QLabel("Ajusta el ángulo y velocidad, luego presiona LANZAR");
    statusLayout->addWidget(playerLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(statusLabel);
    mainLayout->addLayout(statusLayout);

    connect(angleSlider, &QSlider::valueChanged, this, &MainWindow::updateAngleLabel);
    connect(speedSlider, &QSlider::valueChanged, this, &MainWindow::updateSpeedLabel);
    connect(launchButton, &QPushButton::clicked, this, &MainWindow::launchProjectile);

    setWindowTitle("esto es un 5 profe");
    resize(900, 750);
}

void MainWindow::setupGame()
{

    engine = new GameEngine(800, 600);

    // Infraestructura Jugador 1 (izquierda) - como en la imagen
    engine->addInfrastructure(1, Infrastructure(170, 280, 55, 270, 200));  // Izquierda
    engine->addInfrastructure(1, Infrastructure(170, 230, 200, 50, 100));  // Arriba (centro)
    engine->addInfrastructure(1, Infrastructure(315, 280, 55, 270, 200));  // Derecha

    // Infraestructura Jugador 2 (derecha) - como en la imagen
    engine->addInfrastructure(2, Infrastructure(620, 280, 55, 270, 200));  // Izquierda
    engine->addInfrastructure(2, Infrastructure(620, 230, 200, 50, 100));  // Arriba (centro)
    engine->addInfrastructure(2, Infrastructure(765, 280, 55, 270, 200));  // Derecha

    renderScene();

}

void MainWindow::renderScene()
{
    scene->clear();
    player1InfraItems.clear();
    player2InfraItems.clear();
    resistanceLabels1.clear();
    resistanceLabels2.clear();

    // Dibujar suelo
    QGraphicsRectItem *ground = scene->addRect(0, 550, 800, 50, QPen(Qt::NoPen), QBrush(QColor(160, 82, 45)));

    // Dibujar cañones/lanzadores
    // Cañón Jugador 1 (esquina superior izquierda)
    QGraphicsEllipseItem *cannon1 = scene->addEllipse(20, 160, 30, 30, QPen(Qt::black, 2), QBrush(QColor(70, 130, 180)));
    QGraphicsRectItem *base1 = scene->addRect(15, 190, 40, 10, QPen(Qt::black, 2), QBrush(QColor(50, 50, 50)));

    // Cañón Jugador 2 (esquina superior derecha)
    QGraphicsEllipseItem *cannon2 = scene->addEllipse(750, 160, 30, 30, QPen(Qt::black, 2), QBrush(QColor(220, 20, 60)));
    QGraphicsRectItem *base2 = scene->addRect(745, 190, 40, 10, QPen(Qt::black, 2), QBrush(QColor(50, 50, 50)));

    // Dibujar infraestructura Jugador 1 con colores como en la imagen
    const QVector<Infrastructure>& infra1 = engine->getPlayer1Infrastructure();
    QColor player1Colors[3] = {QColor(255, 200, 150), QColor(255, 255, 255), QColor(255, 200, 150)};

    for (int i = 0; i < infra1.size(); ++i) {
        if (!infra1[i].isDestroyed()) {
            QRectF rect = infra1[i].getRect();
            QGraphicsRectItem *item = scene->addRect(rect, QPen(Qt::black, 2), QBrush(player1Colors[i]));
            player1InfraItems.append(item);

            QGraphicsTextItem *label = scene->addText(QString::number((int)infra1[i].getResistance()));
            QFont font = label->font();
            font.setPointSize(14);
            font.setBold(true);
            label->setFont(font);
            label->setPos(rect.center().x() - 15, rect.center().y() - 15);
            label->setDefaultTextColor(Qt::black);
            resistanceLabels1.append(label);
        }
    }

    // Dibujar figura "Rival" para Jugador 1
    if (infra1.size() >= 3 || !infra1[0].isDestroyed() || !infra1[1].isDestroyed() || !infra1[2].isDestroyed()) {
        // Cabeza
        scene->addEllipse(255, 450, 30, 30, QPen(Qt::black, 2), QBrush(Qt::white));
        // Cuerpo
        scene->addLine(270, 480, 270, 510, QPen(Qt::black, 2));
        // Brazos
        scene->addLine(270, 490, 250, 500, QPen(Qt::black, 2));
        scene->addLine(270, 490, 290, 500, QPen(Qt::black, 2));
        // Piernas
        scene->addLine(270, 510, 255, 540, QPen(Qt::black, 2));
        scene->addLine(270, 510, 285, 540, QPen(Qt::black, 2));

        // Texto "Rival"
        QGraphicsTextItem *rivalText1 = scene->addText("Rival");
        QFont rivalFont = rivalText1->font();
        rivalFont.setPointSize(10);
        rivalFont.setBold(true);
        rivalText1->setFont(rivalFont);
        rivalText1->setPos(250, 540);
    }

    // Dibujar infraestructura Jugador 2 con colores como en la imagen
    const QVector<Infrastructure>& infra2 = engine->getPlayer2Infrastructure();
    QColor player2Colors[3] = {QColor(255, 200, 150), QColor(255, 255, 255), QColor(255, 200, 150)};

    for (int i = 0; i < infra2.size(); ++i) {
        if (!infra2[i].isDestroyed()) {
            QRectF rect = infra2[i].getRect();
            QGraphicsRectItem *item = scene->addRect(rect, QPen(Qt::black, 2), QBrush(player2Colors[i]));
            player2InfraItems.append(item);

            QGraphicsTextItem *label = scene->addText(QString::number((int)infra2[i].getResistance()));
            QFont font = label->font();
            font.setPointSize(14);
            font.setBold(true);
            label->setFont(font);
            label->setPos(rect.center().x() - 15, rect.center().y() - 15);
            label->setDefaultTextColor(Qt::black);
            resistanceLabels2.append(label);
        }
    }

    // Dibujar figura "Rival" para Jugador 2
    if (infra2.size() >= 3 || !infra2[0].isDestroyed() || !infra2[1].isDestroyed() || !infra2[2].isDestroyed()) {
        // Cabeza
        scene->addEllipse(705, 450, 30, 30, QPen(Qt::black, 2), QBrush(Qt::white));
        // Cuerpo
        scene->addLine(720, 480, 720, 510, QPen(Qt::black, 2));
        // Brazos
        scene->addLine(720, 490, 700, 500, QPen(Qt::black, 2));
        scene->addLine(720, 490, 740, 500, QPen(Qt::black, 2));
        // Piernas
        scene->addLine(720, 510, 705, 540, QPen(Qt::black, 2));
        scene->addLine(720, 510, 735, 540, QPen(Qt::black, 2));

        // Texto "Rival"
        QGraphicsTextItem *rivalText2 = scene->addText("Rival");
        QFont rivalFont = rivalText2->font();
        rivalFont.setPointSize(10);
        rivalFont.setBold(true);
        rivalText2->setFont(rivalFont);
        rivalText2->setPos(700, 540);
    }

    // Etiquetas de jugadores
    QGraphicsTextItem *p1Label = scene->addText("JUGADOR 1");
    p1Label->setPos(30, 210);
    p1Label->setDefaultTextColor(QColor(70, 130, 180));
    QFont font = p1Label->font();
    font.setBold(true);
    font.setPointSize(10);
    p1Label->setFont(font);

    QGraphicsTextItem *p2Label = scene->addText("JUGADOR 2");
    p2Label->setPos(690, 210);
    p2Label->setDefaultTextColor(QColor(220, 20, 60));
    p2Label->setFont(font);
}

void MainWindow::updateGame()
{

    if (!engine) {
        timer->stop();
        return;
    }

    if (!engine->getActiveProjectile()) {
        timer->stop();
        launchButton->setEnabled(true);
        return;
    }

    bool projectileActive = engine->update(0.016);

    if (projectileActive) {
        const Projectile *proj = engine->getActiveProjectile();
        if (proj && proj->isActive()) {
            QPointF pos = proj->getPosition();

            // Validar posición antes de usar
            if (pos.x() < -100 || pos.x() > 900 || pos.y() < -100 || pos.y() > 700) {
                timer->stop();
                launchButton->setEnabled(true);
                return;
            }

            if (projectileItem == nullptr) {
                projectileItem = scene->addEllipse(0, 0, 16, 16,
                                                   QPen(Qt::black), QBrush(Qt::black));
            }

            projectileItem->setPos(pos.x() - 8, pos.y() - 8);
            updateResistanceLabels();

            // Actualizar contador de rebotes restantes
            int bouncesLeft = 3 - proj->getBounceCount();
            bouncesLabel->setText(QString("Rebotes restantes: %1").arg(bouncesLeft));

            // Cambiar color según rebotes restantes
            if (bouncesLeft == 1) {
                bouncesLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #FF0000;");
            } else if (bouncesLeft == 2) {
                bouncesLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #FF6347;");
            } else {
                bouncesLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #32CD32;");
            }
        }
    } else {

        // Limpiar el proyectil visual
        if (projectileItem) {
            scene->removeItem(projectileItem);
            delete projectileItem;
            projectileItem = nullptr;
        }

        timer->stop();
        launchButton->setEnabled(true);

        // Verificar si el juego terminó
        if (engine->isGameOver()) {
            QString message;
            if (engine->getWinner() == 1) {
                message = "¡JUGADOR 1 GANA!\n\n¡Has alcanzado al rival enemigo!";
            } else {
                message = "¡JUGADOR 2 GANA!\n\n¡Has alcanzado al rival enemigo!";
            }

            QMessageBox::information(this, "¡Juego Terminado!", message);
            statusLabel->setText("Juego terminado");
            bouncesLabel->setText("Rebotes restantes: -");
        } else {
            engine->switchTurn();
            // Cambiar de turno
            playerLabel->setText(QString("Turno: Jugador %1").arg(engine->getCurrentPlayer()));
            statusLabel->setText("Ajusta el ángulo y velocidad, luego presiona LANZAR");
            bouncesLabel->setText("Rebotes restantes: 3");
            bouncesLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #32CD32;");

            // Re-renderizar la escena para actualizar resistencias
            renderScene();
        }
    }
}

void MainWindow::launchProjectile()
{
    double angle = angleSlider->value();
    double speed = speedSlider->value();

    engine->launchProjectile(engine->getCurrentPlayer(), angle, speed);

    const Projectile* proj = engine->getActiveProjectile();

    launchButton->setEnabled(false);

    statusLabel->setText("Proyectil en vuelo...");

    bouncesLabel = new QLabel("Rebotes restantes: 3", this);

    bouncesLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #32CD32;");

    timer->start();
}

void MainWindow::updateAngleLabel(int value)
{
    angleLabel->setText(QString("Ángulo: %1°").arg(value));
}

void MainWindow::updateSpeedLabel(int value)
{
    speedLabel->setText(QString("Velocidad: %1").arg(value));
}

void MainWindow::updateResistanceLabels()
{
    const QVector<Infrastructure>& infra1 = engine->getPlayer1Infrastructure();
    short int cont1 = 0;
    for (int i = 0; i < resistanceLabels1.size() && i < infra1.size(); ++i) {
        if (infra1[i].getResistance() != 0){
            resistanceLabels1[i]->setPlainText(QString::number((int)infra1[cont1].getResistance()));
        }
        cont1++;
    }

    const QVector<Infrastructure>& infra2 = engine->getPlayer2Infrastructure();
    short int cont2 = 0;
    for (int i = 0; i < resistanceLabels2.size() && i < infra2.size(); i++) {
        if (infra2[i].getResistance() != 0){
            resistanceLabels2[i]->setPlainText(QString::number((int)infra2[cont2].getResistance()));
        }
        cont2++;
    }
}
