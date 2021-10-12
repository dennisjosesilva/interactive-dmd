#include "ManipulateCPs/ManipulateCPs.hpp"

#include <QMessageBox>
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
    
    Node_ *prevNode = nullptr;
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
          node1->setPrevNode(prevNode);

          if(j > 1){
            prevNode->setNextNode(node1);
            Edge *edge = new Edge(prevNode, node1, i);
            scene->addItem(edge);
            WholeEdgeList << edge;
          } 
           
          prevNode = node1;
          if(j == ReadingCPforEachBranch.size() - 1) node1->setNextNode(nullptr);
        }
    }
   
  }

}
void ManipulateCPs::AddOneCp(){
  if(!isBranchSelected){//
    QMessageBox::information(0, "For your information",
        "Please select a branch/point first!");
  }
  else AddCPbuttonPressed = true;

}

void ManipulateCPs::deleteCurrCp(){
  //update CPlist
  int CurrCPNum = CPlist[CurrNodeIndex_m][0][0] - 1;
  if(CurrCPNum < 2){
    QMessageBox::information(0, "For your information",
        "Please leave at least two nodes!");
  }
  else {
    CPlist[CurrNodeIndex_m][0][0] = CurrCPNum;
    if(CPlist[CurrNodeIndex_m][0][1] == CurrCPNum) 
      CPlist[CurrNodeIndex_m][0][1] -= 1;
    
    //update cpnum and degree for all nodes.
    Node_ *forewardNode = CurrPressedNode->getPrevNode();
    while(forewardNode != nullptr){
      forewardNode->setDegree(CPlist[CurrNodeIndex_m][0][0]-1, CPlist[CurrNodeIndex_m][0][1]);
      forewardNode = forewardNode->getPrevNode();
    }
    Node_ *backwardNode = CurrPressedNode->getNextNode();
    while(backwardNode != nullptr){
      backwardNode->setDegree(CPlist[CurrNodeIndex_m][0][0]-1, CPlist[CurrNodeIndex_m][0][1]);
      int index_n = backwardNode->getIndexN();
      backwardNode->setIndex(CurrNodeIndex_m, index_n-1);//update location of nodes after the CurrPressedNode.
      backwardNode = backwardNode->getNextNode();
    }
   
    vector<Vector3<float>> *changedBranch;
    changedBranch = &(CPlist[CurrNodeIndex_m]);
    changedBranch->erase(changedBranch->begin() + CurrNodeIndex_n);
    
    //change sliders
    emit PressNode(0, CPlist[CurrNodeIndex_m][0][0]-1, CPlist[CurrNodeIndex_m][0][1]); 

    scene->removeItem(CurrPressedNode);
    //remove two edges of CurrPressedNode
    QVector<Edge*> EdgeList = CurrPressedNode->getEdgeList();
    for (Edge *edge : qAsConst(EdgeList)){
      scene->removeItem(edge);
      //does WholeEdgeList need remove edge? Seems doesn't.
    }
    //add new edge and update prev/next node.
    Node_ *CurrPrevN = CurrPressedNode->getPrevNode();
    Node_ *CurrNextN = CurrPressedNode->getNextNode();
    if(CurrPrevN != nullptr && CurrNextN != nullptr){
      Edge *edge = new Edge(CurrPrevN, CurrNextN, CurrNodeIndex_m);
      edge->setThickerSignal(true);
      scene->addItem(edge);
      WholeEdgeList << edge;

      CurrPrevN->setNextNode(CurrNextN);
      CurrNextN->setPrevNode(CurrPrevN);
    }
    else{
      if(CurrPrevN == nullptr) CurrNextN->setPrevNode(nullptr);
      else CurrPrevN->setNextNode(nullptr);
    }

  } 
  
}

void ManipulateCPs::Press_node(Node_ *node, int radius, int maxDegree, int degree){
  emit PressNode(radius, maxDegree, degree); 
  CurrPressedNode = node;
  //make edge thicker.
  for (Edge *edge : qAsConst(WholeEdgeList)){
    edge->setThickerSignal(false);
    if(edge->getIndex() == CurrNodeIndex_m) edge->setThickerSignal(true);
    edge->adjust();
  }
  //make one branch seleted.
  isBranchSelected = true;
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


QRectF getBoundingRect(QPointF sourcePoint, QPointF destPoint) 
{
  qreal extra = 3;

  return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(), 
    destPoint.y() - sourcePoint.y()))
    .normalized()
    .adjusted(-extra, -extra, extra, extra);
}

int ManipulateCPs::DetermineLocation(QPointF pressedPoint){
  Node_ *head, *tail, *CurrNode;
  QRectF rect;
  
  //std::cout<< CurrPressedNode->getIndexN() <<"---"<<endl;
  //search forward. 
  Node_ *forewardNode = CurrPressedNode->getPrevNode();
  if(forewardNode != nullptr){
    while(forewardNode != nullptr){
      CurrNode = forewardNode->getNextNode();//use CurrNode to prevent changing CurrPressedNode.
      QPointF source = CurrNode->pos();// in scene coord.
      QPointF dest = forewardNode->pos();
      rect = getBoundingRect(source, dest);

      if(rect.contains(pressedPoint)) {
        CurrPrevNode = forewardNode;
        CurrNextNode = CurrNode;
        return CurrNode->getIndexN();
      }
        
      forewardNode = forewardNode->getPrevNode();
    }
    head = CurrNode->getPrevNode();
  }
  else head = CurrPressedNode;
  //std::cout<< CurrPressedNode->pos().x() <<"---"<<CurrPressedNode->pos().y()<<endl;
  //search backward. 
  Node_ *backwardNode = CurrPressedNode->getNextNode();
  if(backwardNode != nullptr){
    while(backwardNode != nullptr){
      CurrNode = backwardNode->getPrevNode();//use CurrNode to prevent changing CurrPressedNode.
      QPointF source = CurrNode->pos();// in scene coord.
      QPointF dest = backwardNode->pos();
      rect = getBoundingRect(source, dest);

      if(rect.contains(pressedPoint)) {
        CurrPrevNode = CurrNode;
        CurrNextNode = backwardNode;
        return backwardNode->getIndexN();
      }
        
      backwardNode = backwardNode->getNextNode();
    }
    tail = CurrNode->getNextNode();
  }
  else tail = CurrPressedNode;
  
  //search in the two ends.
  QPointF headP = head->pos();
  QPointF tailP = tail->pos();
  int delta_x = tailP.x() - headP.x();
  int delta_y = tailP.y() - headP.y();

  if(delta_x < 0)
  {
    if(delta_y < 0) rect = getBoundingRect(headP, QPointF(w/2, h/2));//bottom right point
    else rect = getBoundingRect(headP, QPointF(w/2, -h/2));//upper right point
  }
  else{
    if(delta_y < 0) rect = getBoundingRect(headP, QPointF(-w/2, h/2));//bottom left point
    else rect = getBoundingRect(headP, QPointF(-w/2, -h/2));//upper left point
  }
  if(rect.contains(pressedPoint)){
    CurrPrevNode = nullptr;
    CurrNextNode = head;
    return 1;//in the first place
  } 
      
  if(delta_x < 0)
  {
    if(delta_y < 0) rect = getBoundingRect(QPointF(-w/2, -h/2), tailP);//upper left point
    else rect = getBoundingRect(QPointF(-w/2, h/2), tailP);//bottom left point 
  }
  else{
    if(delta_y < 0) rect = getBoundingRect(QPointF(w/2, -h/2), tailP);//upper right point
    else rect = getBoundingRect(QPointF(w/2, h/2), tailP);//bottom right point
  }
  if(rect.contains(pressedPoint)){
    CurrPrevNode = tail;
    CurrNextNode = nullptr;
    return (tail->getIndexN()+1);//in the last place
  } 
  
  return 100;//Didn't click in the right place.
}

void ManipulateCPs::updateAddingCP(int index_n, QPointF point){
  int set_radius = 1;
  //update CPlist
  CPlist[CurrNodeIndex_m][0][0] += 1;
  Vector3<float> newPoint;
  newPoint[0] = point.x() + w/2;
  newPoint[1] = point.y() + h/2;
  newPoint[2] = set_radius;
  
  vector<Vector3<float>> *changedBranch;
  changedBranch = &(CPlist[CurrNodeIndex_m]);
  changedBranch->insert(changedBranch->begin() + index_n, newPoint);
  
  
  //add node in the scene
  Node_ *node1 = new Node_(this);
  scene->addItem(node1);
  node1->setPos(point.x(), point.y());
  node1->setIndex(CurrNodeIndex_m, index_n);
  node1->setRadius(set_radius);
  node1->setDegree(CPlist[CurrNodeIndex_m][0][0]-1, CPlist[CurrNodeIndex_m][0][1]);
  node1->setPrevNode(CurrPrevNode);
  node1->setNextNode(CurrNextNode);
  
  
  //remove the original edge.
  if(CurrPrevNode != nullptr && CurrNextNode!= nullptr){
    QVector<Edge*> EdgeListP = CurrPrevNode->getEdgeList();
    QVector<Edge*> EdgeListN = CurrNextNode->getEdgeList();
    for (Edge *edge : qAsConst(EdgeListP)){
      if(EdgeListN.contains(edge)) scene->removeItem(edge);
      //does WholeEdgeList need remove edge? Seems doesn't.
    }
  }
  //add two new edges
  if(CurrPrevNode != nullptr){
    Edge *edge = new Edge(CurrPrevNode, node1, CurrNodeIndex_m);
    edge->setThickerSignal(true);
    scene->addItem(edge);
    WholeEdgeList << edge;

    CurrPrevNode->setNextNode(node1);
  }
  
  if(CurrNextNode != nullptr){
    Edge *edge = new Edge(node1, CurrNextNode, CurrNodeIndex_m);
    edge->setThickerSignal(true);
    scene->addItem(edge);
    WholeEdgeList << edge;

    CurrNextNode->setPrevNode(node1);
  }
  
  // Add all features of pressing the node1.
  CurrNodeIndex_n = index_n;
  //change sliders
  emit PressNode(set_radius, CPlist[CurrNodeIndex_m][0][0]-1, CPlist[CurrNodeIndex_m][0][1]); 
  CurrPressedNode = node1;

  
  //update cpnum and degree for all nodes.
  Node_ *forewardNode = CurrPressedNode->getPrevNode();
  while(forewardNode != nullptr){
    forewardNode->setDegree(CPlist[CurrNodeIndex_m][0][0]-1, CPlist[CurrNodeIndex_m][0][1]);
    forewardNode = forewardNode->getPrevNode();
  }
  Node_ *backwardNode = CurrPressedNode->getNextNode();
  while(backwardNode != nullptr){
    backwardNode->setDegree(CPlist[CurrNodeIndex_m][0][0]-1, CPlist[CurrNodeIndex_m][0][1]);
    int indexN = backwardNode->getIndexN();
    backwardNode->setIndex(CurrNodeIndex_m, indexN+1);//update location of nodes after the CurrPressedNode.
    backwardNode = backwardNode->getNextNode();
  }

}
void ManipulateCPs::mousePressEvent(QMouseEvent *event) {
  //const QPoint& point = event->pos(); 
  if(AddCPbuttonPressed == true){
    QPointF scenePoint = mapToScene(event->pos()); 
    //std::cout<< scenePoint.x() <<"---/"<<scenePoint.y()<<endl;
    int index_n = DetermineLocation(scenePoint);
    //std::cout<<"index_n---"<<index_n<<endl;
    if(index_n != 100)
      updateAddingCP(index_n, scenePoint);
    
    AddCPbuttonPressed = false;
  }
  
  QGraphicsView::mousePressEvent(event);
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