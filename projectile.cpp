#include "projectile.h"
#include <cmath>
#include <QDebug>

Projectile::Projectile(double x, double y, double angle, double speed, double m, int player)
    : position(x, y), mass(m), radius(8), active(true), bounceCount(0)
{
    double angleRad = angle * M_PI / 180.0;

    // Si es jugador 2, invertir la dirección horizontal
    if (player == 2) {
        velocity.setX(-speed * std::cos(angleRad));
        velocity.setY(-speed * std::sin(angleRad));
    } else {
        velocity.setX(speed * std::cos(angleRad));
        velocity.setY(-speed * std::sin(angleRad));
    }
}

void Projectile::update(double dt)
{
    if (!active) return;

    position.setX(position.x() + velocity.x() * dt);
    position.setY(position.y() + velocity.y() * dt);

    velocity.setY(velocity.y() + gravity * dt);

    static int counter = 0;
    if (counter++ % 30 == 0) {
        qDebug() << "Proyectil pos:" << position.x() << "," << position.y()
        << "vel:" << velocity.x() << "," << velocity.y();
    }
}

