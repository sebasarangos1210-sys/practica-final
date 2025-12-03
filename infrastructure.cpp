#include "infrastructure.h"
#include <cmath>
#include <algorithm>

Infrastructure::Infrastructure(double x, double y, double w, double h, double r)
    : rect(x, y, w, h), resistance(r)
{
}

void Infrastructure::takeDamage(double damage)
{
    resistance -= damage;
    if (resistance < 0) resistance = 0;
}

bool Infrastructure::checkCollision(const QPointF& center, double radius) const
{
    if (isDestroyed()) return false;

    double closestX = std::max(rect.left(), std::min(center.x(), rect.right()));
    double closestY = std::max(rect.top(), std::min(center.y(), rect.bottom()));

    double dx = center.x() - closestX;
    double dy = center.y() - closestY;
    double distance = std::sqrt(dx * dx + dy * dy);

    return distance < radius;
}

int Infrastructure::getCollisionSide(const QPointF& center, const QPointF& prevCenter) const
{
    double distTop = std::abs(center.y() - rect.top());
    double distBottom = std::abs(center.y() - rect.bottom());
    double distLeft = std::abs(center.x() - rect.left());
    double distRight = std::abs(center.x() - rect.right());

    double minDist = std::min({distTop, distBottom, distLeft, distRight});

    if (minDist == distTop) return 0;
    if (minDist == distRight) return 1;
    if (minDist == distBottom) return 2;
    return 3;
}
