#ifndef INFRASTRUCTURE_H
#define INFRASTRUCTURE_H

#include <QRectF>
#include <QPointF>

class Infrastructure
{
public:
    Infrastructure(double x, double y, double w, double h, double resistance);

    QRectF getRect() const { return rect; }
    double getResistance() const { return resistance; }
    bool isDestroyed() const { return resistance <= 0; }

    void takeDamage(double damage);

    bool checkCollision(const QPointF& center, double radius) const;
    int getCollisionSide(const QPointF& center, const QPointF& prevCenter) const;

private:
    QRectF rect;
    double resistance;
};

#endif // INFRASTRUCTURE_H
