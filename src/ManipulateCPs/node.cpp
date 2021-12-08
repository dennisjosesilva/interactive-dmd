#include "ManipulateCPs/node.hpp"
#include "ManipulateCPs/ManipulateCPs.hpp"

#include "ManipulateCPs/edge.hpp"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

Node_::Node_(ManipulateCPs *graphWidget)
  :graph(graphWidget)
{
  setFlag(ItemIsMovable);
  setFlag(ItemSendsGeometryChanges);
  setFlag(ItemIsSelectable);
  setCacheMode(DeviceCoordinateCache);
  setZValue(-1);
}

void Node_::addEdge(Edge *edge)
{
  edgeList << edge;
  edge->adjust();
}

QVector<Edge *> Node_::edges() const 
{
  return edgeList;
}

QRectF Node_::boundingRect() const 
{
  qreal adjust = 2; //2
  return QRectF( -NodeRadius - adjust, -NodeRadius - adjust, 2*(NodeRadius + adjust), 2*(NodeRadius + adjust));
}

QPainterPath Node_::shape() const 
{
  QPainterPath path;
  path.addEllipse(-NodeRadius, -NodeRadius, 2*NodeRadius, 2*NodeRadius);
  return path;
}

void Node_::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)//setting style for each node
{
  QRadialGradient gradient(-3, -3, 10);
  if(isSelected()){
    gradient.setColorAt(0, Qt::blue);
    gradient.setColorAt(1, Qt::darkBlue);
    graph->TranspCurrPoint(this);
  }
  else{
    gradient.setColorAt(0, Qt::yellow);
    gradient.setColorAt(1, Qt::darkYellow);
  }
  
  painter->setBrush(gradient);

  painter->setPen(QPen(Qt::red, 0));
  painter->drawEllipse(-NodeRadius, -NodeRadius, 2*NodeRadius, 2*NodeRadius);

}

QVariant Node_::itemChange(GraphicsItemChange change, const QVariant &value)
{
  switch (change) 
  {
  case ItemPositionHasChanged:
    for (Edge *edge : qAsConst(edgeList))
      edge->adjust();
    //graph->itemMoved();
    break;
  
  default:
    break;
  }

  return QGraphicsItem::itemChange(change, value);
}

void Node_::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  //graph->setCurrentNodeIndex(index_m, index_n);
  graph->Press_node(this, radius_, maxDegree, degree);
 
  update();
  QGraphicsItem::mousePressEvent(event);
 
}

void Node_::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  //std::cout<<"mouseReleaseEvent"<<std::endl;
  newPos = pos();
  //cout<<"index_m "<<newPos.rx()<<" index_m "<<newPos.ry()<<endl;
  graph->itemMoved(newPos); 
   
  update();
  QGraphicsItem::mouseReleaseEvent(event);
}