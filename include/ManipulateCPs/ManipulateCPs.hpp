#pragma once 

#include "ManipulateCPs/edge.hpp"
#include "ManipulateCPs/node.hpp"
#include <QPixmap>
#include <QGraphicsView>
#include <SDMD/dmdReconstruct.hpp>
#include <iostream>
#include <QGraphicsLineItem>
#include <fstream>

#include "ManipulateCPs/ControlPoint.hpp"

//class Edge;

class ManipulateCPs : public QGraphicsView
{
  Q_OBJECT

public:
  ManipulateCPs(int W, int H, QWidget *parent = nullptr);
  inline void setImage(const QImage &newImage) 
  { 
    backgroundImg = newImage; 
    w = newImage.width();
    h = newImage.height();
    
    }

  inline void UpdateWH(int width, int height) { w = width; h = height; }
  void Update();
  void UpdateBackground();
  void ShowingCPs();
  
  void changeCurrNodeRInCplist(int r);
  void changeCurrbranchDegree(int d);
  void ReconFromMovedCPs(dmdReconstruct *recon, bool max_tree);
  bool ReconImageFromMovedCPs(dmdReconstruct *recon, bool max_tree);
  void Press_node(Node_ *node, int radius, int maxDegree, int degree);
  //inline void setCurrentNodeIndex(int m, int n) {CurrNodeIndex_m = m; CurrNodeIndex_n = n;}
  //void TranspCurrPoint(Node_ *node);
  void MoveMultiPoint(Node_ *node, QPointF newPos);
  inline void getCPmap(QMap<unsigned int, vector<vector<Vector3<float>>>> CPmap) 
      {CPlistMap = CPmap; HaveShownCP = false;}
  void deleteCurrCp();
  void deleteMultiCp();
  void rotateCPsBtnPressed();
  void ZoomInOutBtn_pressed();
  void AddOneCp();
  void DeleteTheBranch();
  inline bool getItemsUnselectionState() {return AllItemsUnselected;}
  inline bool getItemsSelectionState() {return AllItemsSelected;}
  
  inline bool getRotateCPsState() {return rotateCPs;}
  //inline void closeLogFile() {OutLog.close();}
  void removeTwoEdgeOfNode (Node_ *CurrNode);
  void SetDegreeOfTwoEdgeOfNode (Node_ *CurrNode, int degree);
  void FindCenter();
  void paste();
  void updateCPlist();
  Node_ * addPastedCPIntoScene (Node_ *CurrNode);
  void DeleteSelectedBranch();

// ========================================================
// Code based manipulation functions
// ========================================================
  QList<Node_ *> selectedCPs();
  QList<Node_ *> allCPs();

  void translateCP(Node_ *cp, qreal dx, qreal dy);
  void scaleRadius(Node_ *cp, qreal scale);
  void rotateCP(Node_ *cp, qreal cx, qreal cy, qreal angle);
  void scaleCP(Node_ *cp, qreal cx, qreal cy, qreal scale);

  void updateParams(const QVector<ControlPoint> &cps);

  void drawPoint(qreal px, qreal py);
  inline void setInterpState(bool interp){Interp = interp;}

public: 
signals: 
  void PressNode(int radius, int degree); 
  //void setUnSync();

public slots:
  void zoomIn();
  void zoomOut();

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

  void drawBackground(QPainter *painter, const QRectF &rect) override;
  void scaleView(qreal scaleFactor);

private:
  int DetermineLocation(QPointF pressedPoint);
  void updateAddingCP(int index_n, QPointF point);
  void AddNewBranch(QPointF point);
  QImage backgroundImg;
  int w, h;
  int CurrNodeIndex_m, CurrNodeIndex_n;
  //QPixmap pixmap;
  QGraphicsScene *scene;
  QMap<unsigned int, vector<vector<Vector3<float>>>> CPlistMap;
  vector<vector<Vector3<float>>> CPlistForOneNode;
  unsigned int nodeIdForOneNode;
  QVector<Edge*> WholeEdgeList;
  Node_ *CurrPressedNode;
  Node_ *CurrPrevNode, *CurrNextNode;
  bool isBranchSelected = false;
  bool AddCPbuttonPressed = false;
  //vector<Node_ *> MultiCPsDelete;
  bool AllItemsUnselected = false;
  bool AllItemsSelected = false;
  bool Key_Shift_pressed = false;
  bool Key_D_pressed = false;
  bool Key_R_pressed = false;
  bool Key_Z_pressed = false;
  bool Key_C_pressed = false;
  bool Key_X_pressed = false;
  //bool Key_A_pressed = false;
  QImage showBackgroundImg;
  bool drawQImage = false;
  bool rotateCPs = false, ZoomInOut=false;
  bool RedCrossDrawn = false;
  QPointF crossPoint;
  QGraphicsLineItem* HoriLine = nullptr;
  QGraphicsLineItem* VerLine = nullptr;
  float RScaleFactor = 1.0;
  ofstream OutLog;
  QList<QGraphicsItem*> selectedList;
  QList<QGraphicsItem*> selectedCPsForCopy;
  bool HaveShownCP = false;
  bool Interp = false;
};