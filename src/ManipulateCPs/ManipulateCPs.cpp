#include "ManipulateCPs/ManipulateCPs.hpp"

#include <QMessageBox>
#include <cmath>
#include <QKeyEvent>
#include <QList>
#include <QtMath>
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
  OutLog.open("Log.txt");
}

void ManipulateCPs::ShowingCPs(){
  scene->clear();
  for(auto it = CPlistMap.begin(); it != CPlistMap.end(); ++it)
  {
    unsigned int NodeId = it.key();
    vector<vector<Vector3<float>>> CPlist = it.value();
    if(CPlistMap.size() == 1) {
      CPlistForOneNode = CPlist;
      nodeIdForOneNode = NodeId;
    }

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
                node1->setComponentId(NodeId);
                node1->setPrevNode(prevNode);

                if(j > 1){
                  prevNode->setNextNode(node1);
                  Edge *edge = new Edge(prevNode, node1, degree, i);
                  edge->setComponentId(NodeId);
                  scene->addItem(edge);
                  WholeEdgeList << edge;
                } 
                
                prevNode = node1;
                if(j == ReadingCPforEachBranch.size() - 1) node1->setNextNode(nullptr);
                
              }
          }
        }
      }
    OutLog<< "Node-"<<NodeId<< " contains "<<CPlist.size()<<" branches."<<endl<<endl;
  }
  
}
void ManipulateCPs::AddOneCp(){
  
  AddCPbuttonPressed = true;

}

void ManipulateCPs::DeleteTheBranch()
{
  int CurrNodeIndex_m = CurrPressedNode->getIndexM();
  //1. update CPlist - erase the branch
  vector<Vector3<float>> *changedBranch;
  changedBranch = &(CPlistForOneNode[CurrNodeIndex_m]);
  changedBranch->clear(); //This size will not change and CPlist[CurrNodeIndex_m] will be empty.

  //2. change sliders
  emit PressNode(0, 0); 

  //3.remove all points and edges in the branch.
  scene->removeItem(CurrPressedNode);
  removeTwoEdgeOfNode(CurrPressedNode);

  Node_ *forewardNode = CurrPressedNode->getPrevNode();
  while(forewardNode != nullptr){
    scene->removeItem(forewardNode);
    removeTwoEdgeOfNode(forewardNode);
    forewardNode = forewardNode->getPrevNode();
  }
  Node_ *backwardNode = CurrPressedNode->getNextNode();
  while(backwardNode != nullptr){
    scene->removeItem(backwardNode);
    removeTwoEdgeOfNode(backwardNode);
    backwardNode = backwardNode->getNextNode();
  }
  OutLog<<"Delete the whole branch."<<endl<<endl;
  
}

void ManipulateCPs::DeleteTheBranch_multiNode()
{
  vector<vector<Vector3<float>>> CPlistForOneNode_ = CPlistMap[CurrPressedNode->getComponentId()];
  
  int CurrNodeIndex_m = CurrPressedNode->getIndexM();
  
  //1. update CPlist - erase the branch
  vector<Vector3<float>> *changedBranch;
  changedBranch = &(CPlistForOneNode_[CurrNodeIndex_m]);
  changedBranch->clear(); //This size will not change and CPlist[CurrNodeIndex_m] will be empty.
  //cout<<"empty? "<<CPlistForOneNode_[CurrNodeIndex_m].empty()<<endl;
  //2. change sliders
  emit PressNode(0, 0); 

  //3.remove all points and edges in the branch.
  scene->removeItem(CurrPressedNode);
  removeTwoEdgeOfNode(CurrPressedNode);

  Node_ *forewardNode = CurrPressedNode->getPrevNode();
  while(forewardNode != nullptr){
    scene->removeItem(forewardNode);
    removeTwoEdgeOfNode(forewardNode);
    forewardNode = forewardNode->getPrevNode();
  }
  Node_ *backwardNode = CurrPressedNode->getNextNode();
  while(backwardNode != nullptr){
    scene->removeItem(backwardNode);
    removeTwoEdgeOfNode(backwardNode);
    backwardNode = backwardNode->getNextNode();
  }
  
  CPlistMap.insert(CurrPressedNode->getComponentId(), CPlistForOneNode_);
    
}

void ManipulateCPs::deleteCurrCp(){

  int CurrNodeIndex_m = CurrPressedNode->getIndexM();
  int CurrNodeIndex_n = CurrPressedNode->getIndexN();
  int degree = CPlistForOneNode[CurrNodeIndex_m][0][1];
  bool DegreeChanged = false;

  //update CPlist
  int CurrCPNum = CPlistForOneNode[CurrNodeIndex_m][0][0] - 1;
 
  if(CurrCPNum == 0) DeleteTheBranch();//only has one CP
  else
  {
    CPlistForOneNode[CurrNodeIndex_m][0][0] = CurrCPNum;
    while(CPlistForOneNode[CurrNodeIndex_m][0][1] > CurrCPNum-1) 
      CPlistForOneNode[CurrNodeIndex_m][0][1] -= 1;//change degree.
    
    if(degree != CPlistForOneNode[CurrNodeIndex_m][0][1])
    {
      DegreeChanged = true;
      degree = CPlistForOneNode[CurrNodeIndex_m][0][1];
    }
    
    //update cpnum and degree for all nodes.
    Node_ *forewardNode = CurrPressedNode->getPrevNode();
    while(forewardNode != nullptr){
      forewardNode->setDegree(CPlistForOneNode[CurrNodeIndex_m][0][0]-1, degree);
      if(DegreeChanged) SetDegreeOfTwoEdgeOfNode(forewardNode, degree);
      forewardNode = forewardNode->getPrevNode();
    }
    Node_ *backwardNode = CurrPressedNode->getNextNode();
    while(backwardNode != nullptr){
      backwardNode->setDegree(CPlistForOneNode[CurrNodeIndex_m][0][0]-1, degree);
      if(DegreeChanged) SetDegreeOfTwoEdgeOfNode(backwardNode, degree);
      int index_n = backwardNode->getIndexN();
      backwardNode->setIndex(CurrNodeIndex_m, index_n-1);//update location of nodes after the CurrPressedNode.
      backwardNode = backwardNode->getNextNode();
    }
   
    //update CPlist
    vector<Vector3<float>> *changedBranch;
    changedBranch = &(CPlistForOneNode[CurrNodeIndex_m]);
    changedBranch->erase(changedBranch->begin() + CurrNodeIndex_n);
    
    //change value display
    emit PressNode(0, CPlistForOneNode[CurrNodeIndex_m][0][1]); 

    scene->removeItem(CurrPressedNode);
    removeTwoEdgeOfNode(CurrPressedNode);
    //add new edge and update prev/next node.
    Node_ *CurrPrevN = CurrPressedNode->getPrevNode();
    Node_ *CurrNextN = CurrPressedNode->getNextNode();
    if(CurrPrevN != nullptr && CurrNextN != nullptr){
      Edge *edge = new Edge(CurrPrevN, CurrNextN, degree, CurrNodeIndex_m);
     
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
    OutLog<<"Delete one control point."<<endl<<endl;
  }

}

void ManipulateCPs::deleteCurrCp_multiNode(){
  
  vector<vector<Vector3<float>>> CPlistForOneNode = CPlistMap[CurrPressedNode->getComponentId()];

  int CurrNodeIndex_m = CurrPressedNode->getIndexM();
  int CurrNodeIndex_n = CurrPressedNode->getIndexN();

  int degree = CPlistForOneNode[CurrNodeIndex_m][0][1];
  bool DegreeChanged = false;

  //update CPlist
  int CurrCPNum = CPlistForOneNode[CurrNodeIndex_m][0][0] - 1;
 
  if(CurrCPNum == 0) DeleteTheBranch_multiNode();//only has one CP
  else
  {
    CPlistForOneNode[CurrNodeIndex_m][0][0] = CurrCPNum;
    while(CPlistForOneNode[CurrNodeIndex_m][0][1] > CurrCPNum-1) 
      CPlistForOneNode[CurrNodeIndex_m][0][1] -= 1;//change degree.
    
    if(degree != CPlistForOneNode[CurrNodeIndex_m][0][1])
    {
      DegreeChanged = true;
      degree = CPlistForOneNode[CurrNodeIndex_m][0][1];
    }
    //update cpnum and degree for all nodes.
    Node_ *forewardNode = CurrPressedNode->getPrevNode();
    while(forewardNode != nullptr){
      forewardNode->setDegree(CPlistForOneNode[CurrNodeIndex_m][0][0]-1, degree);
      if(DegreeChanged) SetDegreeOfTwoEdgeOfNode(forewardNode, degree);
      forewardNode = forewardNode->getPrevNode();
    }
    Node_ *backwardNode = CurrPressedNode->getNextNode();
    while(backwardNode != nullptr){
      backwardNode->setDegree(CPlistForOneNode[CurrNodeIndex_m][0][0]-1, degree);
      if(DegreeChanged) SetDegreeOfTwoEdgeOfNode(backwardNode, degree);
      int index_n = backwardNode->getIndexN();
      backwardNode->setIndex(CurrNodeIndex_m, index_n-1);//update location of nodes after the CurrPressedNode.
      backwardNode = backwardNode->getNextNode();
    }
   
    //update CPlist
    vector<Vector3<float>> *changedBranch;
    changedBranch = &(CPlistForOneNode[CurrNodeIndex_m]);
    changedBranch->erase(changedBranch->begin() + CurrNodeIndex_n);
    
    //change value display
    emit PressNode(0, CPlistForOneNode[CurrNodeIndex_m][0][1]); 

    scene->removeItem(CurrPressedNode);
    removeTwoEdgeOfNode(CurrPressedNode);
    //add new edge and update prev/next node.
    Node_ *CurrPrevN = CurrPressedNode->getPrevNode();
    Node_ *CurrNextN = CurrPressedNode->getNextNode();
    if(CurrPrevN != nullptr && CurrNextN != nullptr){
      Edge *edge = new Edge(CurrPrevN, CurrNextN, degree, CurrNodeIndex_m);
      edge->setComponentId(CurrPressedNode->getComponentId());
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
    OutLog<<"Delete one control point."<<endl<<endl;

    CPlistMap.insert(CurrPressedNode->getComponentId(), CPlistForOneNode);
    //If there is already an item with the key key, that item's value is replaced with value.
  }
   
}

void ManipulateCPs::rotateCPsBtnPressed()
{
  rotateCPs = true;
}
void ManipulateCPs::ZoomInOutBtn_pressed()
{
  ZoomInOut = true;
}
void ManipulateCPs::deleteMultiCp()
{
  OutLog<<"Delete multiple CPs."<<endl<<endl;
  QList<QGraphicsItem*> selectedList =	scene->selectedItems();
  if(!selectedList.empty()){
    Node_ * selectedNode;
    
    for (auto& selectedItem : selectedList) {
      if (selectedNode = qgraphicsitem_cast<Node_ *>(selectedItem)) {
          CurrPressedNode = selectedNode;
          if(CPlistMap.size() == 1) deleteCurrCp();
          else deleteCurrCp_multiNode();
      }
    }
  }
}

void ManipulateCPs::removeTwoEdgeOfNode (Node_ *CurrNode)
{
  QVector<Edge*> EdgeList = CurrNode->getEdgeList();
  for (Edge *edge : qAsConst(EdgeList)){
    scene->removeItem(edge);
    //does WholeEdgeList need remove edge? Seems doesn't.
  }
}
void ManipulateCPs::SetDegreeOfTwoEdgeOfNode (Node_ *CurrNode, int degree)
{
  QVector<Edge*> EdgeList = CurrNode->getEdgeList();
  for (Edge *edge : qAsConst(EdgeList)){
    edge -> setDegree(degree);
  }
}

void ManipulateCPs::MoveMultiPoint(Node_ *node, QPointF newPos){
  QList<QGraphicsItem*> selectedList =	scene->selectedItems();
  if(selectedList.size() == 1){
     OutLog<<"For Node-"<<node->getComponentId()<<", branch-"<<node->getIndexM()<<": moving node-"<<
     node->getIndexN()<<" to ("<<newPos.rx() + w/2<<", "<<newPos.ry() + h/2 <<")."<<endl<<endl;
  }
  //else OutLog<<"Moving multi-nodes."<<endl;
  

  if(CPlistMap.size() == 1){
    CPlistForOneNode[node->getIndexM()][node->getIndexN()][0] = newPos.rx() + w/2;
    CPlistForOneNode[node->getIndexM()][node->getIndexN()][1] = newPos.ry() + h/2;
    CPlistForOneNode[node->getIndexM()][node->getIndexN()][2] *= ZoomFactor;
  }
  else{
    vector<vector<Vector3<float>>> CPlist = CPlistMap[node->getComponentId()];
    CPlist[node->getIndexM()][node->getIndexN()][0] = newPos.rx() + w/2;
    CPlist[node->getIndexM()][node->getIndexN()][1] = newPos.ry() + h/2;
    CPlist[node->getIndexM()][node->getIndexN()][2] *= ZoomFactor;
    //If there is already an item with the key key, that item's value is replaced with value.
    CPlistMap.insert(node->getComponentId(), CPlist);

  }

}

void ManipulateCPs::Press_node(Node_ *node, int radius, int maxDegree, int degree){
  emit PressNode(radius, degree); 
  
  CurrPressedNode = node;
  
  CurrNodeIndex_m = CurrPressedNode->getIndexM();
  CurrNodeIndex_n = CurrPressedNode->getIndexN();
  
  //make edge thicker.
  for (Edge *edge : qAsConst(WholeEdgeList)){
    edge->setThickerSignal(false);
    if(CPlistMap.size() == 1){
      if(edge->getIndex() == CurrNodeIndex_m) 
        edge->setThickerSignal(true);
    }
    else{
      if(edge->getIndex() == CurrNodeIndex_m && edge->getComponentId() == CurrPressedNode->getComponentId()) 
        edge->setThickerSignal(true);
    }
    
    edge->adjust();
  }
  //make one branch seleted.
  isBranchSelected = true;
}

void ManipulateCPs::Update(){
  scene->clear();
  UpdateBackground();
}
void ManipulateCPs::UpdateBackground()
{
  scaleView(pow(2.0, -0.1 / 240.0));
}

void ManipulateCPs::changeCurrNodeRInCplist(int r)
{
  CPlistForOneNode[CurrNodeIndex_m][CurrNodeIndex_n][2] = r;
  OutLog<<"Changing the radius of the CP to "<<r<<endl<<endl;
}
void ManipulateCPs::changeCurrbranchDegree(int d)
{
  CPlistForOneNode[CurrNodeIndex_m][0][1] = d;
  OutLog<<"Changing the degree of the CP to "<<d<<endl<<endl;
} 

void ManipulateCPs::ReconFromMovedCPs(dmdReconstruct *recon)
{
  emit setUnSync();
  if(CPlistMap.size() == 1) CPlistMap.insert(nodeIdForOneNode, CPlistForOneNode);
 
  recon->reconFromMovedCPs(CPlistMap);

  UpdateBackground();

  //if(!MultiCPsDelete.empty()) MultiCPsDelete.clear();//To avoid being deleted next.
  AllItemsUnselected = true;
  AllItemsSelected = false;
  ZoomFactor = 1.0;
  
  if(HoriLine != nullptr)
  {
    scene->removeItem(HoriLine);
    scene->removeItem(VerLine);
    HoriLine = nullptr;
    VerLine = nullptr;
  }  
}
void ManipulateCPs::ReconImageFromMovedCPs(dmdReconstruct *recon)
{
  OutLog.close(); 
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
  case Qt::Key_R:
    Key_R_pressed = true;
    break;
  case Qt::Key_Z:
    Key_Z_pressed = true;
    break;
  case Qt::Key_A:
    AllItemsSelected = true;
    UpdateBackground();
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
    case Qt::Key_R:
      Key_R_pressed = false;
      break;
    case Qt::Key_Z:
      Key_Z_pressed = false;
      break;  
  }

}
void ManipulateCPs::wheelEvent(QWheelEvent *event)
{
  //change the radius of the current CP
  if(Key_Shift_pressed){
    int setR = CurrPressedNode->getRadius() + event->angleDelta().y() / 120.0;
    CurrPressedNode->setRadius(setR);
    CurrPressedNode->setPaintRadius(true);
    CurrPressedNode->update();
    //update CPlist
    changeCurrNodeRInCplist(setR);
    //change value display
    emit PressNode(setR, 0);
  }
  //change the degree of the current CP
  else if(Key_D_pressed){
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
      SetDegreeOfTwoEdgeOfNode(forewardNode, setD);
      UpdateBackground();
      forewardNode = forewardNode->getPrevNode();
    }
    Node_ *backwardNode = CurrPressedNode->getNextNode();
    while(backwardNode != nullptr){
      backwardNode->setDegree(CurrPressedNode->getMaxDegree(), setD);
      SetDegreeOfTwoEdgeOfNode(backwardNode, setD);
      UpdateBackground();
      backwardNode = backwardNode->getNextNode();
    }
  }
  //rotate current CP(s)
    else if(Key_R_pressed)
    {
      QList<QGraphicsItem*> selectedList =	scene->selectedItems();
      if(!selectedList.empty()){
        Node_ * selectedNode;
        QPointF StartPos, rotatedPos;
        float angle = M_PI/180 * (event->angleDelta().y() / 120.0);
        for (auto& selectedItem : selectedList) {
          if (selectedNode = qgraphicsitem_cast<Node_ *>(selectedItem)) {
              StartPos = selectedNode->getPos();
              rotatedPos.rx() = (StartPos.rx() - crossPoint.rx())*qCos(angle) - (StartPos.ry() - crossPoint.ry())*qSin(angle) + crossPoint.rx();
              rotatedPos.ry() = (StartPos.rx() - crossPoint.rx())*qSin(angle) + (StartPos.ry() - crossPoint.ry())*qCos(angle) + crossPoint.ry();
              
              selectedNode->setPos(rotatedPos);
          }
        }

      }
      OutLog<<"Rotating multiple CPs."<<endl<<endl;
    }
    //zoom in/out current CP(s)
    else if(Key_Z_pressed)
      {
        QList<QGraphicsItem*> selectedList =	scene->selectedItems();
        if(!selectedList.empty()){
          Node_ * selectedNode;
          QPointF StartPos, changedPos;
          ZoomFactor = (event->angleDelta().y() / 120.0) * 0.1 + 1.0;
          //cout<<"ZoomFactor: "<<ZoomFactor<<endl;
          for (auto& selectedItem : selectedList) {
            if (selectedNode = qgraphicsitem_cast<Node_ *>(selectedItem)) {
                StartPos = selectedNode->getPos();
                changedPos.rx() = crossPoint.rx() + (StartPos.rx() - crossPoint.rx()) * ZoomFactor;
                changedPos.ry() = crossPoint.ry() + (StartPos.ry() - crossPoint.ry()) * ZoomFactor;
                
                selectedNode->setPos(changedPos);
            }
          }

        }
        OutLog<<"Zoom in/out multiple CPs."<<endl<<endl;
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
  FirstTriple[1] = 1;//degree = 1;
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

  //cout<<"CPlist.size(): "<<CPlist.size()<<endl;
  CPlistForOneNode.push_back(addedBranch);
  //cout<<"CPlist.size(): "<<CPlist.size()<<endl;

  //2. change sliders
  emit PressNode(set_radius, 1); 

  //3.add two points in the scene.
  Node_ *node1 = new Node_(this);
  scene->addItem(node1);
  node1->setPos(point.x(), point.y());
  node1->setIndex(CPlistForOneNode.size()-1, 1);
  node1->setRadius(set_radius);
  node1->setDegree(1, 1);
  node1->setComponentId(nodeIdForOneNode);

  Node_ *node2 = new Node_(this);
  scene->addItem(node2);
  node2->setPos(point.x()+1, point.y()+1);
  node2->setIndex(CPlistForOneNode.size()-1, 2);
  node2->setRadius(set_radius);
  node2->setDegree(1, 1);
  node2->setComponentId(nodeIdForOneNode);

  node1->setPrevNode(nullptr);
  node1->setNextNode(node2);
  node2->setPrevNode(node1);
  node2->setNextNode(nullptr);

  //4.Add edges between them
  Edge *edge = new Edge(node1, node2, 1, CPlistForOneNode.size()-1);
  edge->setComponentId(nodeIdForOneNode);
  edge->setThickerSignal(true);
  scene->addItem(edge);
  WholeEdgeList << edge;

  //5. Add all features of pressing the node1.
  CurrNodeIndex_n = 1;
  CurrPressedNode = node1;

  OutLog<<"Add a new branch (branch-"<<(CPlistForOneNode.size()-1)<<") at ("
  <<point.x()+w/2<<", "<<point.y()+h/2<<")."<<endl<<endl;

}

void ManipulateCPs::updateAddingCP(int index_n, QPointF point){
  int set_radius = 1;
  //update CPlist
  CPlistForOneNode[CurrNodeIndex_m][0][0] += 1;
  Vector3<float> newPoint;
  newPoint[0] = point.x() + w/2;
  newPoint[1] = point.y() + h/2;
  newPoint[2] = set_radius;
  
  vector<Vector3<float>> *changedBranch;
  changedBranch = &(CPlistForOneNode[CurrNodeIndex_m]);
  changedBranch->insert(changedBranch->begin() + index_n, newPoint);
  
  //add node in the scene
  Node_ *node1 = new Node_(this);
  scene->addItem(node1);
  node1->setPos(point.x(), point.y());
  node1->setIndex(CurrNodeIndex_m, index_n);
  node1->setRadius(set_radius);
  node1->setDegree(CPlistForOneNode[CurrNodeIndex_m][0][0]-1, CPlistForOneNode[CurrNodeIndex_m][0][1]);
  node1->setComponentId(nodeIdForOneNode);
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
    Edge *edge = new Edge(CurrPrevNode, node1, CPlistForOneNode[CurrNodeIndex_m][0][1], CurrNodeIndex_m);
    edge->setThickerSignal(true);
    scene->addItem(edge);
    WholeEdgeList << edge;

    CurrPrevNode->setNextNode(node1);
  }
  
  if(CurrNextNode != nullptr){
    Edge *edge = new Edge(node1, CurrNextNode, CPlistForOneNode[CurrNodeIndex_m][0][1], CurrNodeIndex_m);
    edge->setThickerSignal(true);
    scene->addItem(edge);
    WholeEdgeList << edge;

    CurrNextNode->setPrevNode(node1);
  }
  
  // Add all features of pressing the node1.
  CurrNodeIndex_n = index_n;
  //change sliders
  emit PressNode(set_radius, CPlistForOneNode[CurrNodeIndex_m][0][1]); 
  CurrPressedNode = node1;

  
  //update cpnum and degree for all nodes.
  Node_ *forewardNode = CurrPressedNode->getPrevNode();
  while(forewardNode != nullptr){
    forewardNode->setDegree(CPlistForOneNode[CurrNodeIndex_m][0][0]-1, CPlistForOneNode[CurrNodeIndex_m][0][1]);
    forewardNode = forewardNode->getPrevNode();
  }
 
  Node_ *backwardNode = CurrPressedNode->getNextNode();
  while(backwardNode != nullptr){
    backwardNode->setDegree(CPlistForOneNode[CurrNodeIndex_m][0][0]-1, CPlistForOneNode[CurrNodeIndex_m][0][1]);
    int indexN = backwardNode->getIndexN();
    backwardNode->setIndex(CurrNodeIndex_m, indexN+1);//update location of nodes after the CurrPressedNode.
    backwardNode = backwardNode->getNextNode();
  }
  OutLog<<"Add a new CP at ("
  <<point.x()+w/2<<", "<<point.y()+h/2<<")."<<endl<<endl;

}

void ManipulateCPs::mousePressEvent(QMouseEvent *event) {
  //const QPoint& point = event->pos(); 
  //cout<<"view: mousePressEvent"<<endl;
  QPointF scenePoint = mapToScene(event->pos());
  if(AddCPbuttonPressed){
    //std::cout<< scenePoint.x() <<"---/"<<scenePoint.y()<<endl;
    if(!isBranchSelected){//Didn't select a branch
      AddNewBranch(scenePoint);
    }
    else{
      
        int index_n = DetermineLocation(scenePoint);
        cout<<"index_n---"<<index_n<<endl;
        if(index_n != 100)
          updateAddingCP(index_n, scenePoint);
        else AddNewBranch(scenePoint);
    }
    
    AddCPbuttonPressed = false;
  }
  else if (rotateCPs || ZoomInOut)
    {
      HoriLine = new QGraphicsLineItem(scenePoint.x()-5, scenePoint.y(), scenePoint.x()+5, scenePoint.y());
      HoriLine->setPen(QPen(Qt::red, 2));
      scene->addItem(HoriLine);

      VerLine = new QGraphicsLineItem(scenePoint.x(), scenePoint.y()-5, scenePoint.x(), scenePoint.y()+5);
      VerLine->setPen(QPen(Qt::red, 2));
      scene->addItem(VerLine);
      crossPoint = scenePoint;
      //rotateCPs = false;
    }
  AllItemsSelected = false;
  AllItemsUnselected = false;
  QGraphicsView::mousePressEvent(event);
}

void ManipulateCPs::mouseReleaseEvent(QMouseEvent *event) {
  //QPointF releasePoint = mapToScene(event->pos());
   //std::cout<< releasePoint.x() <<"---/"<<releasePoint.y()<<endl;
  rotateCPs = false;
  ZoomInOut = false;

  QGraphicsView::mouseReleaseEvent(event);
}

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