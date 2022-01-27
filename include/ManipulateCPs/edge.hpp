#pragma once 

#include <QGraphicsItem>

class Node_;

class Edge : public QGraphicsItem
{
public:
  Edge(Node_ *sourceNode, Node_ *destNode, int degree, int color_i);
  inline void setThickerSignal(bool thicker) {thickerEdge = thicker;}
  inline void setDegree(int degree) {Degree = degree;}
  inline void setComponentId(unsigned int id) {component_id = id;}

  inline int getIndex() {return colorI;}
  inline unsigned int getComponentId() {return component_id;}

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
  int colorI, Degree;
  bool thickerEdge = false;
  unsigned int component_id;
};