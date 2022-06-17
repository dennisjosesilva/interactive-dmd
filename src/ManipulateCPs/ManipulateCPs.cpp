#include "ManipulateCPs/ManipulateCPs.hpp"

#include <QMessageBox>
#include <cmath>
#include <QKeyEvent>
#include <QList>
#include <QtMath>
#include <QPair>
//#include <QRandomGenerator>          
//using namespace std;
int offsetX, offsetY;
int selectedCentralX, selectedCentralY;
bool CutPastedOnce = false;
bool CutOrCopy = false;
bool NoItemsInScene = true;

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
  HaveShownCP = true;
  scene->clear();
  
  if(!WholeEdgeList.empty()) WholeEdgeList.clear();

  NoItemsInScene = false;

  for(auto it = CPlistMap.begin(); it != CPlistMap.end(); ++it)
  {
    int count = 0;
    unsigned int NodeId = it.key();
    vector<vector<Vector3<float>>> CPlist = it.value();
    if(CPlistMap.size() == 1) {
      CPlistForOneNode = CPlist;
      nodeIdForOneNode = NodeId;
    }

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
              count++;
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
    OutLog<< "Node-"<<NodeId<< " contains "<<CPlist.size()<<" branches and "<<count<<" CPs."<<endl<<endl;
  }
  
}
void ManipulateCPs::AddOneCp(){
  AddCPbuttonPressed = true;
}

void ManipulateCPs::DeleteTheBranch()
{
  int CurrNodeIndex_m = CurrPressedNode->getIndexM();
  //cout<<"CurrNodeIndex_m "<<CurrNodeIndex_m<<endl;
  //1. update CPlist - erase the branch
  vector<Vector3<float>> *changedBranch;
  if(CPlistMap.size() == 1 ) {
    changedBranch = &(CPlistForOneNode[CurrNodeIndex_m]);
    changedBranch->clear(); //This size will not change and CPlist[CurrNodeIndex_m] will be empty.
  }
  else {
    vector<vector<Vector3<float>>> CPlistForOneNode_ = CPlistMap[CurrPressedNode->getComponentId()];
    changedBranch = &(CPlistForOneNode_[CurrNodeIndex_m]);
    changedBranch->clear();
    CPlistMap.insert(CurrPressedNode->getComponentId(), CPlistForOneNode_);
  }
  
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

void ManipulateCPs::deleteCurrCp(){
  
  int CurrNodeIndex_m = CurrPressedNode->getIndexM();
  int CurrNodeIndex_n = CurrPressedNode->getIndexN();

  int degree, CurrCPNum;
  bool DegreeChanged = false;
  vector<Vector3<float>> *changedBranch;

  if(CPlistMap.size() == 1 ) CurrCPNum = CPlistForOneNode[CurrNodeIndex_m][0][0] - 1;
  else {
    vector<vector<Vector3<float>>> CPlistForOneNode_ = CPlistMap[CurrPressedNode->getComponentId()];
    CurrCPNum = CPlistForOneNode_[CurrNodeIndex_m][0][0] - 1;
  }
 
  if(CurrCPNum == 0) DeleteTheBranch();//only has one CP
  else
  {
    if(CPlistMap.size() == 1 ){
      CPlistForOneNode[CurrNodeIndex_m][0][0] = CurrCPNum;
      while(CPlistForOneNode[CurrNodeIndex_m][0][1] > CurrCPNum-1) {
        CPlistForOneNode[CurrNodeIndex_m][0][1] -= 1;//change degree.
        DegreeChanged = true;
      }
      degree = CPlistForOneNode[CurrNodeIndex_m][0][1];
      //update CPlist
      changedBranch = &(CPlistForOneNode[CurrNodeIndex_m]);
      changedBranch->erase(changedBranch->begin() + CurrNodeIndex_n);
    }
    else{
      vector<vector<Vector3<float>>> CPlistForOneNode = CPlistMap[CurrPressedNode->getComponentId()];
      CPlistForOneNode[CurrNodeIndex_m][0][0] = CurrCPNum;
      while(CPlistForOneNode[CurrNodeIndex_m][0][1] > CurrCPNum-1) {
        CPlistForOneNode[CurrNodeIndex_m][0][1] -= 1;//change degree.
        DegreeChanged = true;
      }
      degree = CPlistForOneNode[CurrNodeIndex_m][0][1];
      //update CPlist
      changedBranch = &(CPlistForOneNode[CurrNodeIndex_m]);
      changedBranch->erase(changedBranch->begin() + CurrNodeIndex_n);
      CPlistMap.insert(CurrPressedNode->getComponentId(), CPlistForOneNode);

    }

    //update cpnum and degree for all nodes.
    Node_ *forewardNode = CurrPressedNode->getPrevNode();
    while(forewardNode != nullptr){
      forewardNode->setDegree(CurrCPNum-1, degree);
      if(DegreeChanged) SetDegreeOfTwoEdgeOfNode(forewardNode, degree);
      forewardNode = forewardNode->getPrevNode();
    }
    Node_ *backwardNode = CurrPressedNode->getNextNode();
    while(backwardNode != nullptr){
      backwardNode->setDegree(CurrCPNum-1, degree);
      if(DegreeChanged) SetDegreeOfTwoEdgeOfNode(backwardNode, degree);
      int index_n = backwardNode->getIndexN();
      backwardNode->setIndex(CurrNodeIndex_m, index_n-1);//update location of nodes after the CurrPressedNode.
      backwardNode = backwardNode->getNextNode();
    }
   
    //change value display
    emit PressNode(0, degree); 

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
  selectedList =	scene->selectedItems();
  if(!selectedList.empty()){
    Node_ * selectedNode;
    
    for (auto& selectedItem : selectedList) {
      if (selectedNode = qgraphicsitem_cast<Node_ *>(selectedItem)) {
          CurrPressedNode = selectedNode;
          deleteCurrCp();
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
  if(!(Key_R_pressed || Key_Z_pressed)){//To avoid multiple assignments.
    selectedList = scene->selectedItems();
    if(selectedList.size() == 1){
      OutLog<<"For Node-"<<node->getComponentId()<<", branch-"<<node->getIndexM()<<": moving node-"<<
      node->getIndexN()<<" to ("<<newPos.rx() + w/2<<", "<<newPos.ry() + h/2 <<")."<<endl<<endl;
    }
    //else OutLog<<"Moving multi-nodes."<<endl;
    
    if(CPlistMap.size() == 1){
      CPlistForOneNode[node->getIndexM()][node->getIndexN()][0] = newPos.rx() + w/2;
      CPlistForOneNode[node->getIndexM()][node->getIndexN()][1] = newPos.ry() + h/2;
    }
    else{
      vector<vector<Vector3<float>>> CPlist = CPlistMap[node->getComponentId()];
      CPlist[node->getIndexM()][node->getIndexN()][0] = newPos.rx() + w/2;
      CPlist[node->getIndexM()][node->getIndexN()][1] = newPos.ry() + h/2;
      //If there is already an item with the key key, that item's value is replaced with value.
      CPlistMap.insert(node->getComponentId(), CPlist);
    }
  }
}

void ManipulateCPs::Press_node(Node_ *node, int radius, int maxDegree, int degree){
  emit PressNode(radius, degree); 
  //cout<<"nodeID: "<<node->getComponentId()<<endl;;
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
  NoItemsInScene = true;
  UpdateBackground();
}
void ManipulateCPs::UpdateBackground()
{
  scaleView(pow(2.0, -0.1 / 240.0));
}

void ManipulateCPs::changeCurrNodeRInCplist(int r)
{
  CPlistForOneNode[CurrNodeIndex_m][CurrNodeIndex_n][2] = r;
  OutLog<<"Changing the radius of the node - "<<CurrNodeIndex_m
  <<" in branch - "<<CurrNodeIndex_n<<" to "<<r<<"."<<endl<<endl;
}
void ManipulateCPs::changeCurrbranchDegree(int d)
{
  CPlistForOneNode[CurrNodeIndex_m][0][1] = d;
  OutLog<<"Change the degree of branch - "<<CurrPressedNode->getIndexN()
  <<" to "<<d<<"."<<endl<<endl;
} 

void ManipulateCPs::ReconFromMovedCPs(dmdReconstruct *recon, bool max_tree)
{
  if(HaveShownCP)
  {
    if(CPlistMap.size() == 1) CPlistMap.insert(nodeIdForOneNode, CPlistForOneNode);

    recon->reconFromMovedCPs(CPlistMap, max_tree);

    UpdateBackground();
    
    //if(!MultiCPsDelete.empty()) MultiCPsDelete.clear();//To avoid being deleted next.
    AllItemsUnselected = true;
    AllItemsSelected = false;
    RScaleFactor = 1.0;
    
    if(HoriLine != nullptr)
    {
      scene->removeItem(HoriLine);
      scene->removeItem(VerLine);
      HoriLine = nullptr;
      VerLine = nullptr;
    }  
    RedCrossDrawn = false;
  }
  
}

bool ManipulateCPs::ReconImageFromMovedCPs(dmdReconstruct *recon, bool max_tree)
{
  if(HaveShownCP)
  {
    OutLog.close(); 
    vector<int> reconAll;
    reconAll.push_back(10000);//Just make sure reconstruct all nodes.
    recon->ReconstructMultiNode(Interp, reconAll, 0, max_tree);

    showBackgroundImg = recon->getOutQImage();
    drawQImage = true;

    scaleView(pow(2.0, -0.1 / 240.0));//Just make background update
    return true;
  }
  else return false;
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
  //1. update CPlist - add the branch
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

void ManipulateCPs::DeleteSelectedBranch()
{
  if(!selectedCPsForCopy.empty()){
    int countBranchNum = 0;
    vector<QPair<int,int> > BranchNumVec;

    Node_ * CurrPressedNode;
    for (auto& selectedItem : selectedCPsForCopy) {
      if (CurrPressedNode = qgraphicsitem_cast<Node_ *>(selectedItem)) {
        int BranchNum = CurrPressedNode->getIndexM();
        int NodeNum = CurrPressedNode->getComponentId();
        QPair<int, int> NodeBranchPair = qMakePair(NodeNum, BranchNum);
        if (std::find(BranchNumVec.begin(), BranchNumVec.end(), NodeBranchPair) == BranchNumVec.end())
        {// The branch where this CurrPressedNode is located has not been processed before
          countBranchNum++;
          BranchNumVec.push_back(NodeBranchPair);

          //These branches cannot be deleted first since we need them in the paste function.
          // 1. update CPlist - delete the branch of the CurrPressedNode
          /*vector<Vector3<float>> *changedBranch;
          if(CPlistMap.size() == 1 ) {
            changedBranch = &(CPlistForOneNode[BranchNum]);
            changedBranch->clear(); //This size will not change and CPlist[CurrNodeIndex_m] will be empty.
          }
          else {
            vector<vector<Vector3<float>>> CPlistForOneNode_ = CPlistMap[NodeNum];
            changedBranch = &(CPlistForOneNode_[BranchNum]);
            changedBranch->clear();
            CPlistMap.insert(NodeNum, CPlistForOneNode_);
          }
          */
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

        }
      }
    }
    OutLog<<"Cut "<<countBranchNum<<" branches."<<endl<<endl;
  }

}

void ManipulateCPs::FindCenter()
{
  CutPastedOnce = false;
  //if(!selectedCPsForCopy.empty()) selectedCPsForCopy.clear();
  if(!CutOrCopy) //Just in case you pressed 'X', and then pressed 'C' (You actually wanted to press 'V')
    selectedCPsForCopy =scene->selectedItems();

  if(!selectedCPsForCopy.empty()){
    int selectedSize = selectedCPsForCopy.size();
    int totalX = 0;
    int totalY = 0;

    //find the center.
    Node_ * selectedNode;
    for (auto& selectedItem : selectedCPsForCopy) {
      if (selectedNode = qgraphicsitem_cast<Node_ *>(selectedItem)) {
         QPointF pos = selectedNode->getPos();
         //cout<<"pos.rx() "<<pos.rx()<<" pos.ry() "<<pos.ry()<<endl;
         totalX += pos.rx();
         totalY += pos.ry();
      }
    }
    selectedCentralX = totalX/selectedSize;
    selectedCentralY = totalY/selectedSize;

    if(Key_X_pressed)
    {
      DeleteSelectedBranch();
      CutOrCopy = true;
    }
  }
  else{
    QMessageBox::information(0, "For your information",
         "If you want to copy some CPs, \n"
         "please select them first.");
  }
}

void ManipulateCPs::paste(){
  CutOrCopy = false;
  //cout<<"-- "<<selectedCPsForCopy.empty()<<endl;
if(CutPastedOnce) {
  QMessageBox::information(0, "For your information",
        "Note that the cut CPs can only be pasted once.\n");
}
else{
  if(!selectedCPsForCopy.empty()){
    int countBranchNum = 0;
    vector<QPair<int,int> > BranchNumVec;

    Node_ * selectedNode;
    for (auto& selectedItem : selectedCPsForCopy) {
      if (selectedNode = qgraphicsitem_cast<Node_ *>(selectedItem)) {
        int BranchNum = selectedNode->getIndexM();
        int NodeNum = selectedNode->getComponentId();
        QPair<int, int> NodeBranchPair = qMakePair(NodeNum, BranchNum);
        if (std::find(BranchNumVec.begin(), BranchNumVec.end(), NodeBranchPair) == BranchNumVec.end())
        {// The branch where this selectedNode is located has not been processed before
          countBranchNum++;
          BranchNumVec.push_back(NodeBranchPair);
          //1. update CPlist - add the branch of the selectedNode
          vector<Vector3<float>> addedBranch;

          if(CPlistMap.size() == 1){
            addedBranch = CPlistForOneNode[BranchNum];
            for(auto j = 0; j < addedBranch.size(); ++j){
              if(j > 0) 
              { 
                addedBranch[j][0] += offsetX;
                addedBranch[j][1] += offsetY;
              }
            }
            CPlistForOneNode.push_back(addedBranch);
          }
          else{//multi_node
            vector<vector<Vector3<float>>> CPlistForOneNode = CPlistMap[NodeNum];

            addedBranch = CPlistForOneNode[BranchNum];
            for(auto j = 0; j < addedBranch.size(); ++j){
              if(j > 0) 
              { 
                addedBranch[j][0] += offsetX;
                addedBranch[j][1] += offsetY;
              }
            }
            CPlistForOneNode.push_back(addedBranch);
            CPlistMap.insert(NodeNum, CPlistForOneNode);
          }
          if(Key_X_pressed){
            CutPastedOnce = true;
            //Then delete the branch that you cut
            vector<Vector3<float>> *changedBranch;
            if(CPlistMap.size() == 1) {
              changedBranch = &(CPlistForOneNode[BranchNum]);
              changedBranch->clear(); //This size will not change and CPlist[CurrNodeIndex_m] will be empty.
            }
            else {
              vector<vector<Vector3<float>>> CPlistForOneNode_ = CPlistMap[NodeNum];
              changedBranch = &(CPlistForOneNode_[BranchNum]);
              changedBranch->clear();
              CPlistMap.insert(NodeNum, CPlistForOneNode_);
            }

          }

          //2. Add nodes into the scene.
          Node_ *curr_node = addPastedCPIntoScene (selectedNode);
          Node_ *curr_node_ = curr_node;

          Node_ *forewardNode = selectedNode->getPrevNode();
          
          while(forewardNode != nullptr){
            Node_ *foreward_node = addPastedCPIntoScene (forewardNode);
            curr_node->setPrevNode(foreward_node);
            foreward_node->setNextNode(curr_node);
            //3.Add edges
            Edge *edge = new Edge(foreward_node, curr_node, curr_node->getDegree(), curr_node->getIndexM());
            edge->setComponentId(NodeNum);
            scene->addItem(edge);
            WholeEdgeList << edge;
            curr_node = foreward_node;
            forewardNode = forewardNode->getPrevNode();
          }
          curr_node->setPrevNode(nullptr);

          Node_ *backwardNode = selectedNode->getNextNode();
          while(backwardNode != nullptr){
            Node_*backward_node = addPastedCPIntoScene (backwardNode);
            backward_node->setPrevNode(curr_node_);
            curr_node_->setNextNode(backward_node);
            //3.Add edges
            Edge *edge = new Edge(curr_node_, backward_node, curr_node->getDegree(), curr_node->getIndexM());
            //edge->setComponentId(nodeIdForOneNode);
            scene->addItem(edge);
            WholeEdgeList << edge;
            curr_node_ = backward_node;
            backwardNode = backwardNode->getNextNode();
          }
           curr_node_->setNextNode(nullptr);
          
        }
  
      }
    }
    Key_X_pressed = false;
    OutLog<<countBranchNum<<" branches pasted."<<endl<<endl;
    
  }
  else{
    QMessageBox::information(0, "For your information",
        "If you want to paste some CPs, please select them\n"
        "first and then press 'C'/'X' key to copy/cut.");
  }
}
}
  
Node_ * ManipulateCPs::addPastedCPIntoScene (Node_ *CurrNode){
  
  QPointF pos = CurrNode->getPos();
  int index_m;
  int NodeNum = CurrNode->getComponentId();

  if(CPlistMap.size() == 1) index_m = CPlistForOneNode.size()-1;
  else index_m = CPlistMap[NodeNum].size()-1;

  Node_ * translatedNode =  new Node_(this);
  scene->addItem(translatedNode);
  translatedNode->setPos(pos.rx() + offsetX, pos.ry() + offsetY);
  translatedNode->setIndex(index_m, CurrNode->getIndexN());
  translatedNode->setRadius(CurrNode->getRadius());
  translatedNode->setDegree(CurrNode->getMaxDegree(), CurrNode->getDegree());
  translatedNode->setComponentId(NodeNum);//The new pasted component temporarily has the same NodeID as the one it 
                                          //copied from. When users click the 'sync' button. It will get its new nodeID
  return translatedNode;
}

void ManipulateCPs::updateCPlist(){

  selectedList = scene->selectedItems();
  
  if(!selectedList.empty()){
    Node_ * node;
    
    for (auto& selectedItem : selectedList) {
      
      if (node = qgraphicsitem_cast<Node_ *>(selectedItem)) {
          
        if(CPlistMap.size() == 1){
          CPlistForOneNode[node->getIndexM()][node->getIndexN()][0] = node->getPos().rx() + w/2;
          CPlistForOneNode[node->getIndexM()][node->getIndexN()][1] = node->getPos().ry() + h/2;
          CPlistForOneNode[node->getIndexM()][node->getIndexN()][2] *= RScaleFactor;
        }
        else{
          vector<vector<Vector3<float>>> CPlist = CPlistMap[node->getComponentId()];
          CPlist[node->getIndexM()][node->getIndexN()][0] = node->getPos().rx() + w/2;
          CPlist[node->getIndexM()][node->getIndexN()][1] = node->getPos().ry() + h/2;
          CPlist[node->getIndexM()][node->getIndexN()][2] *= RScaleFactor;
          //If there is already an item with the key key, that item's value is replaced with value.
          CPlistMap.insert(node->getComponentId(), CPlist);
        }
      }
    }
  }
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
  case Qt::Key_C:
    Key_C_pressed = true;
    FindCenter();
    break;
  case Qt::Key_X:
    Key_X_pressed = true;
    FindCenter();
    break;
  case Qt::Key_V:
    paste();
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
      if(event->isAutoRepeat())
      {
        event->ignore();
      }
      else{
        Key_R_pressed = false;
        updateCPlist();
        //RedCrossDrawn = false;
      }
      break;
    case Qt::Key_Z:
      if(event->isAutoRepeat())
      {
        event->ignore();
      }
      else{
        Key_Z_pressed = false;
        updateCPlist();
        //RedCrossDrawn = false;
      }
      break;  
  }
}

void ManipulateCPs::wheelEvent(QWheelEvent *event)
{
  //change the radius of the current CP
  if(Key_Shift_pressed){
    if(CPlistMap.size() > 1){
      Key_Shift_pressed = false;
      QMessageBox::information(0, "For your information",
         "Currently processing multiple nodes! Increasing or\n"
         "decreasing the radius only works for one node.");
    }
    else
    {
      int setR = CurrPressedNode->getRadius() + event->angleDelta().y() / 120.0;
        CurrPressedNode->setRadius(setR);
        CurrPressedNode->setPaintRadius(true);
        CurrPressedNode->update();
        //update CPlist
        changeCurrNodeRInCplist(setR);
        //change value display
        emit PressNode(setR, 0);
    }
  }
  //change the degree of the current CP
  else if(Key_D_pressed){
    if(CPlistMap.size() > 1){
      Key_D_pressed = false;
      QMessageBox::information(0, "For your information",
         "Currently processing multiple nodes! Increasing or\n"
         "decreasing the degree only works for one node.");
    }
    else
    {
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
  }
  //rotate current CP(s)
    else if(Key_R_pressed)
    {
      if(RedCrossDrawn){
        
        selectedList = scene->selectedItems();
        float angle;
        qreal cx = crossPoint.rx();
        qreal cy = crossPoint.ry();
        if(!selectedList.empty()){
          Node_ * selectedNode;
          QPointF StartPos, rotatedPos;
          angle = M_PI/180 * (event->angleDelta().y() / 120.0);
          for (auto& selectedItem : selectedList) {
            
            if (selectedNode = qgraphicsitem_cast<Node_ *>(selectedItem)) {
                StartPos = selectedNode->getPos();
                rotatedPos.rx() = (StartPos.rx() - cx)*qCos(angle) - (StartPos.ry() - cy)*qSin(angle) + cx;
                rotatedPos.ry() = (StartPos.rx() - cx)*qSin(angle) + (StartPos.ry() - cy)*qCos(angle) + cy;
                
                selectedNode->setPos(rotatedPos);
                //then MoveMultiPoint() will be automatically called.
            }
          }
        }
        if(angle > 0)
          OutLog<<"Rotating multiple CPs clockwise."<<endl<<endl;
        else
          OutLog<<"Rotating multiple CPs counterclockwise."<<endl<<endl; 
      }
      else{
        Key_R_pressed = false;
        QMessageBox::information(0, "For your information",
         "If you want to rotate the selected CPs, \n"
         "please press the rotation button first.");
      }
    }
    //zoom in/out current CP(s)
    else if(Key_Z_pressed)
      {
        if(RedCrossDrawn){
          float ScaleFactor;
          selectedList = scene->selectedItems();
          if(!selectedList.empty()){
            Node_ * selectedNode;
            QPointF StartPos, changedPos;
            ScaleFactor = (event->angleDelta().y() / 120.0) * 0.1 + 1.0;
            RScaleFactor *= ScaleFactor;
            //cout<<"ScaleFactor: "<<ScaleFactor<<endl;
            for (auto& selectedItem : selectedList) {
              if (selectedNode = qgraphicsitem_cast<Node_ *>(selectedItem)) {
                //change position
                  StartPos = selectedNode->getPos();
                  changedPos.rx() = crossPoint.rx() + (StartPos.rx() - crossPoint.rx()) * ScaleFactor;
                  changedPos.ry() = crossPoint.ry() + (StartPos.ry() - crossPoint.ry()) * ScaleFactor;
                  
                  selectedNode->setPos(changedPos);
                  //then MoveMultiPoint() will be automatically called.
                  //change radius (there'll be a small error because of 'int')
                  int setR = selectedNode->getRadius() * ScaleFactor;
                  selectedNode->setRadius(setR);
              }
            }
          }
          if(ScaleFactor > 1)
            OutLog<<"Scale-up multiple CPs."<<endl<<endl;
          else OutLog<<"Scale-down multiple CPs."<<endl<<endl;
        }
        else{
          Key_Z_pressed = false;
          QMessageBox::information(0, "For your information",
          "If you want to scale up/down the selected CPs, \n"
          "please press the scaling button first.");
        }
      }
      else scaleView(pow(2.0, -event->angleDelta().y() / 240.0));
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
      RedCrossDrawn = true;
    }
    else if(Key_C_pressed || Key_X_pressed){
      Key_C_pressed = false;
      //Key_X_pressed = false;
      offsetX = (int)scenePoint.x() - selectedCentralX;
      offsetY = (int)scenePoint.y() - selectedCentralY;
      }
      else{
        AllItemsSelected = false;
        AllItemsUnselected = false;
        isBranchSelected = false;
        
        //make edge unselected.
        if(!NoItemsInScene){
          for (Edge *edge : qAsConst(WholeEdgeList)){
            edge->setThickerSignal(false);
            edge->adjust();
          }
        }
        
      }
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

QList<Node_ *> ManipulateCPs::selectedCPs()
{
  QList<Node_ *> selectedNodes;
  
  for (QGraphicsItem *item : scene->selectedItems()) {
    Node_ *node = qgraphicsitem_cast<Node_ *>(item);
    if (node != nullptr)
      selectedNodes.append(node);
  }
  return selectedNodes;
}

QList<Node_ *> ManipulateCPs::allCPs()
{
  QList<Node_ *> all;
  for (QGraphicsItem *item : scene->items()) {
    Node_ *node = qgraphicsitem_cast<Node_ *>(item);
    if (node != nullptr)
      all.append(node);
  }
  return all;
}

void ManipulateCPs::translateCP(Node_ *cp, qreal dx, qreal dy)
{
  //using ControlPoint = vector<Vector3<float>>;
  //vector<ControlPoint> &CPlist = CPlistMap[cp->getComponentId()];

  //cp->setX(cp->x() + dx);
  //cp->setY(cp->y() + dy);
  if(CPlistMap.size() == 1){
    CPlistForOneNode[cp->getIndexM()][cp->getIndexN()][0] = cp->x() + dx + (w / 2);
    CPlistForOneNode[cp->getIndexM()][cp->getIndexN()][1] = cp->y() + dy + (h / 2);
  }
  else{
    vector<vector<Vector3<float>>> CPlist = CPlistMap[cp->getComponentId()];
    CPlist[cp->getIndexM()][cp->getIndexN()][0] = cp->x() + dx + (w / 2);
    CPlist[cp->getIndexM()][cp->getIndexN()][1] = cp->y() + dy + (h / 2);
    CPlistMap.insert(cp->getComponentId(), CPlist);
  }
}

void ManipulateCPs::scaleRadius(Node_ *cp, qreal scale)
{
  int setR = cp->getRadius() * scale;
  //cp->setRadius(setR);
  //cp->update();
  
  if(CPlistMap.size() == 1)
    CPlistForOneNode[cp->getIndexM()][cp->getIndexN()][2] = setR;
  else{
    using ControlPoint = vector<Vector3<float>>;
    vector<ControlPoint> &CPlist = CPlistMap[cp->getComponentId()];
    CPlist[cp->getIndexM()][cp->getIndexN()][2] = setR;
    CPlistMap.insert(cp->getComponentId(), CPlist);
  }
}

void ManipulateCPs::rotateCP(Node_ *cp, qreal cx, qreal cy, qreal angle)
{
  QPointF rotatedPoint;
  const QPointF &pos = cp->getPos();

  qreal rangle = qDegreesToRadians(angle);

  rotatedPoint.rx() = (pos.x() - cx)*qCos(rangle) - (pos.y() - cy)*qSin(rangle) + cx;
  rotatedPoint.ry() = (pos.x() - cx)*qSin(rangle) + (pos.y() - cy)*qCos(rangle) + cy;

  //cp->setPos(rotatedPoint);  

  if(CPlistMap.size() == 1){
    CPlistForOneNode[cp->getIndexM()][cp->getIndexN()][0] = rotatedPoint.rx() + w/2;
    CPlistForOneNode[cp->getIndexM()][cp->getIndexN()][1] = rotatedPoint.ry() + h/2;
  }
  else{
    vector<vector<Vector3<float>>> CPlist = CPlistMap[cp->getComponentId()];
    CPlist[cp->getIndexM()][cp->getIndexN()][0] = rotatedPoint.rx() + w/2;
    CPlist[cp->getIndexM()][cp->getIndexN()][1] = rotatedPoint.ry() + h/2;
    //If there is already an item with the key key, that item's value is replaced with value.
    CPlistMap.insert(cp->getComponentId(), CPlist);
  }

}

void ManipulateCPs::scaleCP(Node_ *cp, qreal cx, qreal cy, qreal scale)
{
  RScaleFactor *= scale;
  const QPointF &p = cp->getPos();
  QPointF changedPos;

  changedPos.rx() = cx + (p.x() - cx) * scale;
  changedPos.ry() = cy + (p.y() - cy) * scale;

  //cp->setPos(changedPos);

  int setR = cp->getRadius() * scale;
  //cp->setRadius(setR);

  if(CPlistMap.size() == 1){
    CPlistForOneNode[cp->getIndexM()][cp->getIndexN()][0] = changedPos.rx() + w/2;
    CPlistForOneNode[cp->getIndexM()][cp->getIndexN()][1] = changedPos.ry() + h/2;
    CPlistForOneNode[cp->getIndexM()][cp->getIndexN()][2] = setR;
  }
  else{
    vector<vector<Vector3<float>>> CPlist = CPlistMap[cp->getComponentId()];
    CPlist[cp->getIndexM()][cp->getIndexN()][0] = changedPos.rx() + w/2;
    CPlist[cp->getIndexM()][cp->getIndexN()][1] = changedPos.ry() + h/2;
    CPlist[cp->getIndexM()][cp->getIndexN()][2] = setR;
    //If there is already an item with the key key, that item's value is replaced with value.
    CPlistMap.insert(cp->getComponentId(), CPlist);
  }
}



void ManipulateCPs::drawPoint(qreal px, qreal py)
{
  QPointF scenePoint{px, py};
  HoriLine = new QGraphicsLineItem(scenePoint.x()-5, scenePoint.y(), scenePoint.x()+5, scenePoint.y());
  HoriLine->setPen(QPen(Qt::red, 2));
  scene->addItem(HoriLine);

  VerLine = new QGraphicsLineItem(scenePoint.x(), scenePoint.y()-5, scenePoint.x(), scenePoint.y()+5);
  VerLine->setPen(QPen(Qt::red, 2));
  scene->addItem(VerLine);
}