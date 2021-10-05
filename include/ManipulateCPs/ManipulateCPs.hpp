#pragma once 

#include <QPixmap>
#include <QGraphicsView>
#include <dmdReconstruct.hpp>
#include <iostream>

class Node_;

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
  void ShowingCPs(vector<vector<Vector3<float>>> CPs);
  
  void itemMoved(int m, int n, QPointF Pos);
  void ReconFromMovedCPs(dmdReconstruct *recon, int intensity);

public slots:
  void zoomIn();
  void zoomOut();

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

  void drawBackground(QPainter *painter, const QRectF &rect) override;
  void scaleView(qreal scaleFactor);

private:
  QImage backgroundImg;
  int w, h;
  //QPixmap pixmap;
  QGraphicsScene *scene;
  vector<vector<Vector3<float>>> CPlist;
  
};