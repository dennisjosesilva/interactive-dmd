#pragma once 

#include <QWidget>
#include <QString>
#include <ImageViewerWidget/ImageViewerWidget.hpp>
#include <IcicleMorphotreeWidget/Graphics/Node/GNode.hpp>

#include "TreeVisualiser/TreeVisualiser.hpp"
#include "InteractiveSdmd.hpp"
#include <QDockWidget>
#include "CustomWidgets/ClosableDockWidget.hpp"
#include "CustomWidgets/ThresholdControl.hpp"

enum class ReconMode { MorphoTree, SDMD };

class MainWidget : public QWidget
{  
  Q_OBJECT
public:
  using GNode = IcicleMorphotreeWidget::GNode;

  MainWidget(QWidget *parent=nullptr);

  bool loadImage(const QString& filename);
  bool saveImage(const QString& filename);

  const QImage& image() const { return imageViewer_->image(); }
  QImage& image() { return imageViewer_->image(); }
  
  inline TreeVisualiser *treeVisualiser() { return treeVis_; }
  QDockWidget *morphotreeDockWidget();
  QDockWidget *SdmdDockWidget() {return dockWidgetSdmd_;}

  MyDockWidget* createDockWidget(const QString &title, QWidget *widget);

  void updateTreeVisualiser();

  void zoomOut();
  void zoomIn();

  inline bool isNodeSelectionByClickActivated() const { return nodeSelectionByClickActivated_; }
  inline void setNodeSelectionByClickActivated(bool activated) { nodeSelectionByClickActivated_ = activated; }

  inline bool isHighlightNodeActivated() const { return highlightNode_; }
  void setHighlightNodeActivated(bool checked);
  
  void setImage(const QImage &image);

  ReconMode reconMode() const { return reconMode_; }
  void setReconMode(ReconMode reconMode) { reconMode_ = reconMode; }
  inline bool GetInterpState(){return ThresCtl->getInterpState();}

protected slots:
  void imageMousePress(const QPointF &p);
  void ChangeDisplayImg(QImage img);
  void DisplaySelectedNodes(vector<int> selectedIntensity);
  void DisplayOrigImg();
  

protected:
  void wheelEvent(QWheelEvent *e);

  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);

  bool eventFilter(QObject *obj, QEvent *evt);
  void treeVis_NodeSelected(GNode *node);
  void treeVis_NodeUnselected(GNode *node);

private:
  void createDockTreeVisualiser();
  void createDockWidgetSdmd();
  
  void highlightNode(GNode *node);

private:
  ReconMode reconMode_;

  bool ctrlDown_;

  bool needTreeVisualiserUpdate_;
  bool nodeSelectionByClickActivated_;

  bool highlightNode_;

  ClosableDockWidget *dockTreeVisualiser_;
  QDockWidget *dockWidgetSdmd_;

  ImageViewerWidget::ImageViewerWidget *imageViewer_;
  ThresholdControl *ThresCtl;
  TreeVisualiser *treeVis_;

  InteractiveSdmd *Interactive_sdmd;
  QImage newImage;
  bool HasUpdatedTreeVisualiser = false;
};