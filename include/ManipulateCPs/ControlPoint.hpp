#pragma once

#include <QPointF>

struct ControlPoint
{
  int indexM;
  int indexN;
  unsigned int componentId;
  QPointF pos;
  int radius;
};