#pragma once 

#include "ManipulateCPs/edge.hpp"
#include "ManipulateCPs/node.hpp"
#include <QPixmap>
#include <QGraphicsView>
#include <dmdReconstruct.hpp>
#include <iostream>

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
  void ShowingCPs();
  
  void itemMoved(QPointF Pos);
  void changeCurrNodeR(int r);
  void changeCurrbranchDegree(int d);
  void ReconFromMovedCPs(dmdReconstruct *recon, int intensity);
  void ReconImageFromMovedCPs(dmdReconstruct *recon);
  void Press_node(Node_ *node, int radius, int maxDegree, int degree);
  //inline void setCurrentNodeIndex(int m, int n) {CurrNodeIndex_m = m; CurrNodeIndex_n = n;}
  void TranspCurrPoint(Node_ *node);
  void MoveMultiPoint(Node_ *node, QPointF newPos);
  inline void setCPs(vector<vector<Vector3<float>>> CPs) {CPlist = CPs;}
  void deleteCurrCp();
  void deleteMultiCp();
  void AddOneCp();
  void DeleteLastTwoCPs(Node_ *CurrNode);
  inline bool getItemsUnselectionState() {return AllItemsUnselected;}

public: 
signals: 
  void PressNode(int radius, int maxDegree, int degree); 

public slots:
  void zoomIn();
  void zoomOut();

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  //void mouseReleaseEvent(QMouseEvent *event) override;

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
  vector<vector<Vector3<float>>> CPlist;
  QVector<Edge*> WholeEdgeList;
  Node_ *CurrPressedNode;
  Node_ *CurrPrevNode, *CurrNextNode;
  bool isBranchSelected = false;
  bool AddCPbuttonPressed = false;
  vector<Node_ *> MultiCPsDelete;
  bool AllItemsUnselected = false;
  
};