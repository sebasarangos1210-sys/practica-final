#include "gameengine.h"
#include <cmath>
#include <QDebug>

GameEngine::GameEngine(double w, double h)
    : boxWidth(w), boxHeight(h), currentPlayer(1),
    gameOver(false), winner(0), activeProjectile(nullptr)
{
}

GameEngine::~GameEngine()
{
    if (activeProjectile) {
        delete activeProjectile;
    }
}

void GameEngine::addInfrastructure(int player, const Infrastructure& infra)
{
    if (player == 1) {
        player1Infrastructure.append(infra);
    } else {
        player2Infrastructure.append(infra);
    }
}

void GameEngine::launchProjectile(int player, double angle, double speed)
{
    if (activeProjectile != nullptr) return;

    double startX = (player == 1) ? 50 : boxWidth - 50;
    double startY = boxHeight - 50;

    activeProjectile = new Projectile(startX, startY, angle, speed, projectileMass);
}

bool GameEngine::update(double dt)
{
    if (activeProjectile == nullptr || !activeProjectile->isActive()) {
        return false;
    }

    activeProjectile->update(dt);

    handleWallCollisions();
    handleInfrastructureCollisions();

    QPointF pos = activeProjectile->getPosition();
    if (pos.y() > boxHeight || !activeProjectile->isActive()) {
        delete activeProjectile;
        activeProjectile = nullptr;
        switchTurn();
        return false;
    }

    return true;
}

void GameEngine::handleWallCollisions()
{
    if (!activeProjectile || !activeProjectile->isActive()) return;

    QPointF pos = activeProjectile->getPosition();
    QPointF vel = activeProjectile->getVelocity();
    double radius = activeProjectile->getRadius();

    bool collided = false;

    if (pos.x() - radius <= 0) {
        vel.setX(-vel.x());
        pos.setX(radius);
        collided = true;
    } else if (pos.x() + radius >= boxWidth) {
        vel.setX(-vel.x());
        pos.setX(boxWidth - radius);
        collided = true;
    }

    if (pos.y() - radius <= 0) {
        vel.setY(-vel.y());
        pos.setY(radius);
        collided = true;
    }

    if (collided) {
        activeProjectile->setVelocity(vel);
        activeProjectile->setPosition(pos);
    }
}

void GameEngine::handleInfrastructureCollisions()
{
    if (!activeProjectile || !activeProjectile->isActive()) return;

    QPointF pos = activeProjectile->getPosition();
    QPointF vel = activeProjectile->getVelocity();
    double radius = activeProjectile->getRadius();

    QVector<Infrastructure>* targetInfra =
        (currentPlayer == 1) ? &player2Infrastructure : &player1Infrastructure;

    for (int i = 0; i < targetInfra->size(); ++i) {
        if ((*targetInfra)[i].checkCollision(pos, radius)) {
            QPointF prevPos = pos - vel * 0.01;
            int side = (*targetInfra)[i].getCollisionSide(pos, prevPos);

            double speed = std::sqrt(vel.x() * vel.x() + vel.y() * vel.y());
            double damage = damageFactor * projectileMass * speed;

            (*targetInfra)[i].takeDamage(damage);

            qDebug() << "Colisión! Daño:" << damage
                     << "Resistencia restante:" << (*targetInfra)[i].getResistance();

            if (side == 0 || side == 2) {
                vel.setY(-vel.y() * restitutionCoefficient);
            } else {
                vel.setX(-vel.x() * restitutionCoefficient);
            }

            activeProjectile->setVelocity(vel);

            checkVictoryConditions();
            break;
        }
    }
}

void GameEngine::checkVictoryConditions()
{
    bool player1Defeated = true;
    bool player2Defeated = true;

    for (const Infrastructure& infra : player1Infrastructure) {
        if (!infra.isDestroyed()) {
            player1Defeated = false;
            break;
        }
    }

    for (const Infrastructure& infra : player2Infrastructure) {
        if (!infra.isDestroyed()) {
            player2Defeated = false;
            break;
        }
    }

    if (player1Defeated) {
        gameOver = true;
        winner = 2;
    } else if (player2Defeated) {
        gameOver = true;
        winner = 1;
    }
}

void GameEngine::switchTurn()
{
    currentPlayer = (currentPlayer == 1) ? 2 : 1;
    checkVictoryConditions();
}
