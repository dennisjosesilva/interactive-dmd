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
  setDragMode(QGraphicsView::RubberBandDrag);
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
    if(!CPlist[i].empty()){
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
            node1->setIndex(i, j);//i means the (i+1)_th branch; j means the j_th CP. When j=0, represents(CPnum,degree,sampleNum)
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

}
void ManipulateCPs::AddOneCp(){
  // if(!isBranchSelected){//
  //   QMessageBox::information(0, "For your information",
  //       "Please select a branch/point first!");
  // }
  AddCPbuttonPressed = true;

}

void ManipulateCPs::DeleteLastTwoCPs(Node_ *CurrNode)
{
  int CurrNodeIndex_m = CurrNode->getIndexM();
  //1. update CPlist - erase the branch
  //CPlist.erase(CPlist.begin() + CurrNodeIndex_m);//The size will decrease
  vector<Vector3<float>> *changedBranch;
  changedBranch = &(CPlist[CurrNodeIndex_m]);
  changedBranch->clear(); //This size will not change and CPlist[CurrNodeIndex_m] will be empty.

  //2. change sliders
  emit PressNode(0, 0); 

  //3.remove two points in the scene.
  scene->removeItem(CurrNode);
  Node_ *forewardNode = CurrNode->getPrevNode();
  if (forewardNode != nullptr) scene->removeItem(forewardNode);
  Node_ *backwardNode = CurrNode->getNextNode();
  if (backwardNode != nullptr) scene->removeItem(backwardNode);

  //4.remove edges of CurrNode
  QVector<Edge*> EdgeList = CurrNode->getEdgeList();
  for (Edge *edge : qAsConst(EdgeList)){
    scene->removeItem(edge);
    //does WholeEdgeList need remove edge? Seems doesn't.
  }

}

void ManipulateCPs::deleteCurrCp(){
  //update CPlist
  int CurrCPNum = CPlist[CurrNodeIndex_m][0][0] - 1;
  /*if(CurrCPNum < 2){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "For your information", 
    "There are only two CPs for this branch, Do you want to delete both of them?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
      //delete both of the two CPs, i.e., delete this branch
      DeleteLastTwoCPs(CurrPressedNode);
    } 
    //else do nothing.
  }*/
  if(CurrCPNum == 0) DeleteLastTwoCPs(CurrPressedNode);
  else
  {
    CPlist[CurrNodeIndex_m][0][0] = CurrCPNum;
    while(CPlist[CurrNodeIndex_m][0][1] > CurrCPNum-1) 
      CPlist[CurrNodeIndex_m][0][1] -= 1;//change degree.
    
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
   
    //update CPlist
    vector<Vector3<float>> *changedBranch;
    changedBranch = &(CPlist[CurrNodeIndex_m]);
    changedBranch->erase(changedBranch->begin() + CurrNodeIndex_n);
    
    //change value display
    emit PressNode(0, CPlist[CurrNodeIndex_m][0][1]); 

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

void ManipulateCPs::deleteMultiCp()
{
  //cout<<"MultiCPsDelete: "<<MultiCPsDelete.size()<<endl;
  if(!MultiCPsDelete.empty())
  {
    for(auto it_ = MultiCPsDelete.begin();it_!=MultiCPsDelete.end();it_++){
        
      Node_ *CurrPoint = (*it_);
      int CurrNodeIndex_m = CurrPoint->getIndexM();
      if(!CPlist[CurrNodeIndex_m].empty()){//means the points has already been deleted.
        int CurrCPNum = CPlist[CurrNodeIndex_m][0][0];
      
        if(CurrCPNum < 3)
          DeleteLastTwoCPs(CurrPoint); //delete both of the two CPs, i.e., delete this branch
      
      }
        
    }

    MultiCPsDelete.clear();

  }
  
}

void ManipulateCPs::TranspCurrPoint(Node_ *node){
  
  MultiCPsDelete.push_back(node);
  //cout<<"MultiCPsDelete: "<<MultiCPsDelete.size()<<endl;
}

void ManipulateCPs::MoveMultiPoint(Node_ *node, QPointF newPos){
  //cout<<" CurrNodeIndex_m "<<node->getIndexM()<<" "<<node->getIndexN()<<" "<<newPos.rx()<<" "<<newPos.ry()<<endl;
  
  CPlist[node->getIndexM()][node->getIndexN()][0] = newPos.rx() + w/2;
  CPlist[node->getIndexM()][node->getIndexN()][1] = newPos.ry() + h/2;
}

void ManipulateCPs::Press_node(Node_ *node, int radius, int maxDegree, int degree){
  emit PressNode(radius, degree); 
  CurrPressedNode = node;
  
  CurrNodeIndex_m = CurrPressedNode->getIndexM();
  CurrNodeIndex_n = CurrPressedNode->getIndexN();
  
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
void ManipulateCPs::changeCurrNodeRInCplist(int r)
{
  CPlist[CurrNodeIndex_m][CurrNodeIndex_n][2] = r;
}
void ManipulateCPs::changeCurrbranchDegree(int d)
{
  CPlist[CurrNodeIndex_m][0][1] = d;
}
void ManipulateCPs::ReconFromMovedCPs(dmdReconstruct *recon, int intensity)
{
  recon->reconFromMovedCPs(intensity, CPlist);

  scaleView(pow(2.0, -0.1 / 240.0));//Just make background update
  MultiCPsDelete.clear();//To avoid being deleted next.
  AllItemsUnselected = true;
 
  //cout<<"MultiCPsDelete---: "<<MultiCPsDelete.size()<<endl;
}
void ManipulateCPs::ReconImageFromMovedCPs(dmdReconstruct *recon)
{
  vector<int> reconAll;
  reconAll.push_back(10000);//Just make sure reconstruct all nodes.
  recon->ReconstructMultiNode(false, reconAll, 0);

  showBackgroundImg = recon->getOutQImage();
  drawQImage = true;

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
  case Qt::Key_Shift:
    Key_Shift_pressed = true;
    break;
  case Qt::Key_D:
    Key_D_pressed = true;
    break;
  default:
    QGraphicsView::keyPressEvent(event);
  }
}

void ManipulateCPs::keyReleaseEvent(QKeyEvent *event)
{
  switch (event->key())
  {
    case Qt::Key_Shift:
      Key_Shift_pressed = false;
      break;
    case Qt::Key_D:
      Key_D_pressed = false;
      break;
  }

}
void ManipulateCPs::wheelEvent(QWheelEvent *event)
{
  if(Key_Shift_pressed){
    //std::cout<< event->angleDelta().y() / 120.0 <<"---"<<endl;
    int setR = CurrPressedNode->getRadius() + event->angleDelta().y() / 120.0;
    CurrPressedNode->setRadius(setR);
    CurrPressedNode->setPaintRadius(true);
    CurrPressedNode->update();
    //update CPlist
    changeCurrNodeRInCplist(setR);
    //change value display
    emit PressNode(setR, 0);
  }
  else if(Key_D_pressed){//change degree
    int setD = CurrPressedNode->getDegree() + event->angleDelta().y() / 120.0;
    setD = (setD < 1) ? 1 : setD;
    setD = (setD > CurrPressedNode->getMaxDegree()) ? CurrPressedNode->getMaxDegree() : setD;

    CurrPressedNode->setDegree(CurrPressedNode->getMaxDegree(), setD);
    //update CPlist
    changeCurrbranchDegree(setD);
    //change value display
    emit PressNode(0, setD);
     //update degree for all nodes in the branch.
    Node_ *forewardNode = CurrPressedNode->getPrevNode();
    while(forewardNode != nullptr){
      forewardNode->setDegree(CurrPressedNode->getMaxDegree(), setD);
      forewardNode = forewardNode->getPrevNode();
    }
    Node_ *backwardNode = CurrPressedNode->getNextNode();
    while(backwardNode != nullptr){
      backwardNode->setDegree(CurrPressedNode->getMaxDegree(), setD);
      backwardNode = backwardNode->getNextNode();
    }
  }
    else scaleView(pow(2.0, -event->angleDelta().y() / 240.0));
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

void ManipulateCPs::AddNewBranch(QPointF point)
{
  //1. update CPlist - erase the branch
  int set_radius = 1;
 
  vector<Vector3<float>> addedBranch;
  Vector3<float> FirstTriple;
  FirstTriple[0] = 2;//CPnum = 2;
  FirstTriple[1] = 1;//degree = 2;
  FirstTriple[2] = 10;//Samplenum = 10;

  Vector3<float> firstPoint;
  firstPoint[0] = point.x() + w/2;
  firstPoint[1] = point.y() + h/2;
  firstPoint[2] = set_radius;
  
  Vector3<float> secondPoint;
  secondPoint[0] = point.x() + w/2 + 1;
  secondPoint[1] = point.y() + h/2 + 1;
  secondPoint[2] = set_radius;

  addedBranch.push_back(FirstTriple);
  addedBranch.push_back(firstPoint);
  addedBranch.push_back(secondPoint);

  cout<<"CPlist.size(): "<<CPlist.size()<<endl;
  CPlist.push_back(addedBranch);
  cout<<"CPlist.size(): "<<CPlist.size()<<endl;

  //2. change sliders
  emit PressNode(set_radius, 1); 

  //3.add two points in the scene.
  Node_ *node1 = new Node_(this);
  scene->addItem(node1);
  node1->setPos(point.x(), point.y());
  node1->setIndex(CPlist.size()-1, 1);
  node1->setRadius(set_radius);
  node1->setDegree(1, 1);

  Node_ *node2 = new Node_(this);
  scene->addItem(node2);
  node2->setPos(point.x()+1, point.y()+1);
  node2->setIndex(CPlist.size()-1, 2);
  node2->setRadius(set_radius);
  node2->setDegree(1, 1);

  node1->setPrevNode(nullptr);
  node1->setNextNode(node2);
  node2->setPrevNode(node1);
  node2->setNextNode(nullptr);

  //4.Add edges between them
  Edge *edge = new Edge(node1, node2, CPlist.size()-1);
  edge->setThickerSignal(true);
  scene->addItem(edge);
  WholeEdgeList << edge;

  //5. Add all features of pressing the node1.
  CurrNodeIndex_n = 1;
  CurrPressedNode = node1;

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
  emit PressNode(set_radius, CPlist[CurrNodeIndex_m][0][1]); 
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
  //cout<<"view: mousePressEvent"<<endl;
  if(AddCPbuttonPressed == true){
    QPointF scenePoint = mapToScene(event->pos());
    //std::cout<< scenePoint.x() <<"---/"<<scenePoint.y()<<endl;
    if(!isBranchSelected){//Didn't select a branch
      AddNewBranch(scenePoint);
    }
    else{
        int index_n = DetermineLocation(scenePoint);
        //std::cout<<"index_n---"<<index_n<<endl;
        if(index_n != 100)
          updateAddingCP(index_n, scenePoint);
        else AddNewBranch(scenePoint);
    }
    
    AddCPbuttonPressed = false;
  }
  AllItemsUnselected = false;
  QGraphicsView::mousePressEvent(event);
}

// void ManipulateCPs::mouseReleaseEvent(QMouseEvent *event) {
//   QPointF releasePoint = mapToScene(event->pos());
//    std::cout<< releasePoint.x() <<"---/"<<releasePoint.y()<<endl;

//   QGraphicsView::mouseReleaseEvent(event);
// }

void ManipulateCPs::drawBackground(QPainter *painter, const QRectF &rect)
{
  Q_UNUSED(rect);
//std::cout<<"drawBackground "<<std::endl;

  QRectF sceneRect = this->sceneRect();
  
  if(drawQImage) 
  {
    painter->drawImage(sceneRect, showBackgroundImg);
    drawQImage = false;
  }
  else {
    QPixmap pixmap("../build/output.pgm");//A bit danger. Make sure that you run 'make' in build file.
    painter->drawPixmap(sceneRect, pixmap, QRect());
  }

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