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
  inline void setDegree(int max_d, int d) {maxDegree = max_d; degree = d;}
  inline void setPrevNode(Node_ *pn) {prev_node = pn;}
  inline void setNextNode(Node_ *nn) {next_node = nn;}
  inline void setPaintRadius(bool state) {paintRadius = state;}

  inline QVector<Edge*> getEdgeList() {return edgeList;}
  inline Node_ * getPrevNode() {return prev_node;}
  inline Node_ * getNextNode() {return next_node;}
  inline int getIndexM() {return index_m;}
  inline int getIndexN() {return index_n;}
  inline int getRadius() {return radius_;}
  inline int getDegree() {return degree;}
  inline int getMaxDegree() {return maxDegree;}
  inline QPointF getPos() {return pos();}

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
  int maxDegree, degree;
  Node_ *prev_node, *next_node;
  bool paintRadius = false;
};
