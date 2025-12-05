#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "projectile.h"
#include "infrastructure.h"
#include <QVector>

class GameEngine
{
public:
    GameEngine(double width, double height);
    ~GameEngine();

    void addInfrastructure(int player, const Infrastructure& infra);
    void launchProjectile(int player, double angle, double speed);

    bool update(double dt);

    int getCurrentPlayer() const { return currentPlayer; }
    bool isGameOver() const { return gameOver; }
    int getWinner() const { return winner; }

    const QVector<Infrastructure>& getPlayer1Infrastructure() const { return player1Infrastructure; }
    const QVector<Infrastructure>& getPlayer2Infrastructure() const { return player2Infrastructure; }
    const Projectile* getActiveProjectile() const { return activeProjectile; }
    void switchTurn();

private:
    double boxWidth, boxHeight;
    int currentPlayer;
    bool gameOver;
    int winner;

    QVector<Infrastructure> player1Infrastructure;
    QVector<Infrastructure> player2Infrastructure;
    Projectile* activeProjectile;

    const double restitutionCoefficient = 0.6;
    const double damageFactor = 0.5;
    const double projectileMass = 1.0;

    void handleWallCollisions();
    void handleInfrastructureCollisions();
    void checkVictoryConditions();
};

#endif // GAMEENGINE_H
