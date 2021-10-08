#include "ManipulateCPs/edge.hpp"
#include "ManipulateCPs/node.hpp"
#include <iostream>

#include <QPainter>
#include <QtMath>

Edge::Edge(Node_ *sourceNode, Node_ *destNode, int color_i)
  :source(sourceNode), dest(destNode), colorI(color_i)
{
  setAcceptedMouseButtons(Qt::NoButton);
  source->addEdge(this);
  dest->addEdge(this);
  adjust();
}

Node_ *Edge::sourceNode() const 
{
  return source;
}

Node_ *Edge::destNode() const 
{
  return dest;
}

void Edge::adjust()
{
  //std::cout<<"Edge adjust"<<std::endl;
  if (!source || !dest)
    return;
  int nodeR = source->getNodeRadius();
  QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
  qreal length = line.length();

  prepareGeometryChange();

  if (length > qreal(nodeR * 2)) {//two nodes does not overlap
    QPointF edgeOffset((line.dx() * nodeR) / length, (line.dy() * nodeR) / length);
    sourcePoint = line.p1() + edgeOffset;
    destPoint = line.p2() - edgeOffset;
  } 
  
}

QRectF Edge::boundingRect() const 
{
  if (!source || !dest)
    return QRectF();

  qreal penWidth = 1;
  qreal extra = (penWidth + arrowSize) / 2.0;

  return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(), 
    destPoint.y() - sourcePoint.y()))
    .normalized()
    .adjusted(-extra, -extra, extra, extra);
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  //std::cout<<"Edge paint"<<std::endl;
  if (!source || !dest)
    return;

  QLineF line(sourcePoint, destPoint);
  if (qFuzzyCompare(line.length(), qreal(0.0)))
    return;

  // Draw the line itself
  //painter->setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  QColor qc = QColor::fromRgb(80*(colorI%3),50*(colorI/10),200*(colorI%2));//note: colorI should smaller than 60.
  if(thickerEdge)  painter->setPen(QPen(qc, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  else painter->setPen(QPen(qc, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter->drawLine(line);

}


