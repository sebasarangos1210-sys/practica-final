#include "projectile.h"
#include <cmath>

Projectile::Projectile(double x, double y, double angle, double speed, double m)
    : position(x, y), mass(m), radius(8), active(true)
{
    double angleRad = angle * M_PI / 180.0;
    velocity.setX(speed * std::cos(angleRad));
    velocity.setY(-speed * std::sin(angleRad));
}

void Projectile::update(double dt)
{
    if (!active) return;

    position.setX(position.x() + velocity.x() * dt);
    position.setY(position.y() + velocity.y() * dt);

    // Aplicar gravedad
    velocity.setY(velocity.y() + gravity * dt);
}
