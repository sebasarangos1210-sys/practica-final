#include "projectile.h"
#include <cmath>
#include <QDebug>

Projectile::Projectile(double x, double y, double angle, double speed, double m, int player)
    : position(x, y), mass(m), radius(8), active(true), bounceCount(0)  // Inicializar contador
{
    double angleRad = angle * M_PI / 180.0;

    // Si es jugador 2, invertir la dirección horizontal
    if (player == 2) {
        velocity.setX(-speed * std::cos(angleRad));  // Negativo para ir a la izquierda
        velocity.setY(-speed * std::sin(angleRad));
    } else {
        velocity.setX(speed * std::cos(angleRad));   // Positivo para ir a la derecha
        velocity.setY(-speed * std::sin(angleRad));
    }
}

void Projectile::update(double dt)
{
    if (!active) return;

    // Actualizar posición
    position.setX(position.x() + velocity.x() * dt);
    position.setY(position.y() + velocity.y() * dt);

    // Aplicar gravedad
    velocity.setY(velocity.y() + gravity * dt);

    // Debug: mostrar posición ocasionalmente
    static int counter = 0;
    if (counter++ % 30 == 0) {  // Cada medio segundo aproximadamente
        qDebug() << "Proyectil pos:" << position.x() << "," << position.y()
                 << "vel:" << velocity.x() << "," << velocity.y();
    }
}

