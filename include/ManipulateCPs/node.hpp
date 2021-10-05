#pragma once 
#include <iostream>
#include <QVector>
#include <QGraphicsItem>

class Edge;
class ManipulateCPs;

class Node_ : public QGraphicsItem
{
public:
  Node_(ManipulateCPs *graphWidget);

  void addEdge(Edge *edge);
  QVector<Edge*> edges() const;
  inline int getNodeRadius() {return NodeRadius;}
/*
  enum { Type = UserType + 1 };
  int type() const override { return Type; }

  void calculateForces();
  bool advancePosition();
*/
  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
  inline void setIndex(int i, int j){ index_m = i; index_n = j;}
  inline void setRadius(int radius) {radius_ = radius;}


protected:
  QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
  QVector<Edge*> edgeList;
  QPointF newPos;
  ManipulateCPs *graph;
  int NodeRadius = 3;
  int index_m = 1000, index_n = 1000;
  int radius_;
};
