#include "ManipulateCPs/ManipulateCPs.hpp"
#include "ManipulateCPs/node.hpp"

#include <cmath>
#include <QKeyEvent>
//#include <QRandomGenerator>          
             
ManipulateCPs::ManipulateCPs(int W, int H, QWidget *parent)
  :QGraphicsView(parent), w(W), h(H) 
{
  scene = new QGraphicsScene(this);
  scene->setItemIndexMethod(QGraphicsScene::NoIndex);
  scene->setSceneRect(-w/2, -h/2, w, h);
  setScene(scene);
  setCacheMode(CacheBackground);
  setViewportUpdateMode(BoundingRectViewportUpdate);
  setRenderHint(QPainter::Antialiasing);
  setTransformationAnchor(AnchorUnderMouse);
  
}

void ManipulateCPs::ShowingCPs(){
  if(!WholeEdgeList.empty()) WholeEdgeList.clear();
  vector<Vector3<float>> ReadingCPforEachBranch;
  Vector3<float> ReadingEachCP;
  
  for(auto i = 0; i < CPlist.size(); ++i){
    ReadingCPforEachBranch = CPlist[i];
    
    Node_ *prevNode;
    int degree, maxDegree;
    for(auto j = 0; j < ReadingCPforEachBranch.size(); ++j){
        
        ReadingEachCP = ReadingCPforEachBranch[j];
        if(j == 0) 
        {
          maxDegree = ReadingEachCP[0] - 1;
          degree = ReadingEachCP[1];
        }
        else{ //read 
          Node_ *node1 = new Node_(this);
          scene->addItem(node1);
          node1->setPos(ReadingEachCP[0] - w/2, ReadingEachCP[1] - h/2);
          node1->setIndex(i, j);
          node1->setRadius(ReadingEachCP[2]);
          node1->setDegree(maxDegree, degree);

          
          if(j > 1){
            Edge *edge = new Edge(prevNode, node1, i);
            scene->addItem(edge);
            WholeEdgeList << edge;
          } 
           
          prevNode = node1;
        }
    }
   
  }

}


void ManipulateCPs::Press_node(int radius, int maxDegree, int degree){
  emit PressNode(radius, maxDegree, degree); 
  
//std::cout<<"index_m "<<index_m<<std::endl;
  //make edge thicker.
  for (Edge *edge : qAsConst(WholeEdgeList)){
    edge->setThickerSignal(false);
    //std::cout<<"edge->getIndex() "<<edge->getIndex()<<std::endl;
    if(edge->getIndex() == CurrNodeIndex_m) edge->setThickerSignal(true);
    edge->adjust();
  }
}
void ManipulateCPs::Update(){
  scene->clear();
  scaleView(pow(2.0, -0.1 / 240.0));//Just make background update
}

void ManipulateCPs::itemMoved(QPointF Pos)
{
    CPlist[CurrNodeIndex_m][CurrNodeIndex_n][0] = Pos.rx() + w/2;
    CPlist[CurrNodeIndex_m][CurrNodeIndex_n][1] = Pos.ry() + h/2;
}
void ManipulateCPs::changeCurrNodeR(int r)
{
  CPlist[CurrNodeIndex_m][CurrNodeIndex_n][2] = r;
}
void ManipulateCPs::changeCurrbranchDegree(int d)
{
  CPlist[CurrNodeIndex_m][0][1] = d;
}
void ManipulateCPs::ReconFromMovedCPs(dmdReconstruct *recon, int intensity)
{
  //dmdReconstruct recon;
  recon->reconFromMovedCPs(intensity, CPlist);
  scaleView(pow(2.0, -0.1 / 240.0));//Just make background update
}
void ManipulateCPs::ReconImageFromMovedCPs(dmdReconstruct *recon)
{
  recon->ReconstructIndexingImage(false, -1, 0);
  scaleView(pow(2.0, -0.1 / 240.0));//Just make background update
}

void ManipulateCPs::keyPressEvent(QKeyEvent *event)
{
  
  switch (event->key())
  {
  case Qt::Key_Plus:
    zoomIn();
    break;
  case Qt::Key_Minus:
    zoomOut();
    break;
  default:
    QGraphicsView::keyPressEvent(event);
  }
}

void ManipulateCPs::wheelEvent(QWheelEvent *event)
{
  scaleView(pow(2.0, -event->angleDelta().y() / 240.0));
}

void ManipulateCPs::drawBackground(QPainter *painter, const QRectF &rect)
{
  Q_UNUSED(rect);
//std::cout<<"drawBackground "<<std::endl;

  QRectF sceneRect = this->sceneRect();
  QPixmap pixmap("../build/output.pgm");//A bit danger. Make sure that you run 'make' in build file.
  painter->drawPixmap(sceneRect, pixmap, QRect());

}

void ManipulateCPs::scaleView(qreal scaleFactor)
{
  qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRect(0, 0, 1, 1)).width();
  if (factor < 0.07 || factor > 100)
    return;
  
  scale(scaleFactor, scaleFactor);
}

void ManipulateCPs::zoomIn()
{
  scaleView(qreal(1.2));
}

void ManipulateCPs::zoomOut()
{
  scaleView(1 / qreal(1.2));
}