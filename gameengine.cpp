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
    // Si ya hay un proyectil activo, eliminarlo primero
    if (activeProjectile != nullptr) {
        delete activeProjectile;
        activeProjectile = nullptr;
    }

    // Lanzar desde la posición del cañón
    double startX = (player == 1) ? 35 : boxWidth - 35;
    double startY = 175;  // Altura de los cañones

    try {
        // Pasar el jugador al constructor para que ajuste la dirección
        activeProjectile = new Projectile(startX, startY, angle, speed, projectileMass, player);
    } catch (const std::exception& e) {
        activeProjectile = nullptr;
    } catch (...) {
        activeProjectile = nullptr;
    }

}

bool GameEngine::update(double dt)
{
    if (activeProjectile == nullptr) {
        return false;
    }

    if (!activeProjectile->isActive()) {
        return false;
    }

    // Actualizar proyectil
    activeProjectile->update(dt);

    // Obtener posición DESPUÉS de actualizar
    QPointF pos = activeProjectile->getPosition();

    // Verificar límites básicos PRIMERO para evitar valores inválidos
    if (pos.x() < -100 || pos.x() > boxWidth + 100 ||
        pos.y() < -100 || pos.y() > boxHeight + 100) {
        activeProjectile->setActive(false);
        return false;
    }

    // Verificar si el proyectil toca al rival ANTES de manejar colisiones
    // Zona del rival del jugador 1 (centro izquierdo)
    QRectF rivalZone1(240, 440, 60, 110);

    // Zona del rival del jugador 2 (centro derecho)
    QRectF rivalZone2(690, 440, 60, 110);

    // Si el jugador actual es 1, verificar si golpea al rival 2
    if (currentPlayer == 1 && rivalZone2.contains(pos)) {
        gameOver = true;
        winner = 1;
        activeProjectile->setActive(false);
        return false;
    }

    // Si el jugador actual es 2, verificar si golpea al rival 1
    if (currentPlayer == 2 && rivalZone1.contains(pos)) {
        gameOver = true;
        winner = 2;
        activeProjectile->setActive(false);
        return false;
    }

    // Manejar colisiones DESPUÉS de verificar victoria
    handleWallCollisions();

    // Verificar si sigue activo después de colisión con pared
    if (!activeProjectile->isActive()) {
        return false;
    }

    handleInfrastructureCollisions();

    // Verificar si fue desactivado por rebotes
    if (!activeProjectile->isActive()) {
        return false;
    }

    // Verificar si el proyectil salió del área de juego normal
    if (pos.y() > boxHeight + 50) {
        activeProjectile->setActive(false);
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

    // Colisión con pared izquierda
    if (pos.x() - radius <= 0) {
        vel.setX(-vel.x());
        pos.setX(radius);
        collided = true;
    }
    // Colisión con pared derecha
    else if (pos.x() + radius >= boxWidth) {
        vel.setX(-vel.x());
        pos.setX(boxWidth - radius);
        collided = true;
    }

    // Colisión con techo
    if (pos.y() - radius <= 0) {
        vel.setY(-vel.y());
        pos.setY(radius);
        collided = true;
    }

    // Colisión con piso (elástica)
    if (pos.y() + radius >= 550) {
        vel.setY(-vel.y() * 0.8);
        pos.setY(550 - radius);
        collided = true;
    }

    if (collided) {
        activeProjectile->setVelocity(vel);
        activeProjectile->setPosition(pos);
        activeProjectile->incrementBounce();  // incremetnar  contador de rebotes

        // Si ya rebotó 3 veces, desactivar el proyectil
        if (activeProjectile->getBounceCount() >= 3) {
            activeProjectile->setActive(false);
        }
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


            if (side == 0 || side == 2) {
                vel.setY(-vel.y() * restitutionCoefficient);
            } else {
                vel.setX(-vel.x() * restitutionCoefficient);
            }

            activeProjectile->setVelocity(vel);
            activeProjectile->incrementBounce();  // contar rebote con infraestructura también


            // Si ya rebotó 3 veces, desactivar
            if (activeProjectile->getBounceCount() >= 3) {
                activeProjectile->setActive(false);
            }

            checkVictoryConditions();
            break;
        }
    }
}

void GameEngine::checkVictoryConditions()
{
    // Si ya hay un ganador (por golpear al rival)
    if (gameOver) return;

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

    // Solo declarar victoria por destrucción de infraestructura
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
    // Limpiar el proyectil activo si existe
    if (activeProjectile != nullptr) {
        delete activeProjectile;
        activeProjectile = nullptr;
    }

    currentPlayer = (currentPlayer == 1) ? 2 : 1;
    checkVictoryConditions();
}
