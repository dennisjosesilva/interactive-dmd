#pragma once 

#include <QGraphicsItem>

class Node_;

class Edge : public QGraphicsItem
{
public:
  Edge(Node_ *sourceNode, Node_ *destNode);

  Node_* sourceNode() const;
  Node_ *destNode() const;

  void adjust();

  enum { Type = UserType + 2 };
  int type() const override { return Type; }

protected:
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
  Node_ *source, *dest;

  QPointF sourcePoint;
  QPointF destPoint;
  qreal arrowSize = 10; 
};