#pragma once 

#include <QWidget>
#include <QString>
#include <ImageViewerWidget/ImageViewerWidget.hpp>
#include <IcicleMorphotreeWidget/Graphics/Node/GNode.hpp>

#include "TreeVisualiser/TreeVisualiser.hpp"
#include "SDMD/InteractiveSdmd.hpp"
#include <QDockWidget>
#include "CustomWidgets/ClosableDockWidget.hpp"
#include "CustomWidgets/ThresholdControl.hpp"

enum class ReconMode { MorphoTree, SDMD };

class MainWindow;

class MainWidget : public QWidget
{  
  Q_OBJECT
public:
  using GNode = IcicleMorphotreeWidget::GNode;
  using MorphoTreeType = IcicleMorphotreeWidget::MorphoTreeType;

  MainWidget(MainWindow *mainWindow, QWidget *parent=nullptr);

  bool loadImage(const QString& filename);
  bool saveImage(const QString& filename);

  const QImage& image() const { return imageViewer_->image(); }
  QImage& image() { return imageViewer_->image(); }
  
  inline TreeVisualiser *treeVisualiser() { return treeVis_; }
  QDockWidget *morphotreeDockWidget(
    MorphoTreeType mtreeType = MorphoTreeType::MAX_TREE_8C);
  QDockWidget *SdmdDockWidget() {return dockWidgetSdmd_;}

  MyDockWidget* createDockWidget(const QString &title, QWidget *widget);

  void updateTreeVisualiser(MorphoTreeType mtreeType = MorphoTreeType::MAX_TREE_8C);

  void zoomOut();
  void zoomIn();

  inline bool isNodeSelectionByClickActivated() const { return nodeSelectionByClickActivated_; }
  inline void setNodeSelectionByClickActivated(bool activated) { nodeSelectionByClickActivated_ = activated; }

  inline bool isHighlightNodeActivated() const { return highlightNode_; }
  void setHighlightNodeActivated(bool checked);
  
  inline bool  needTreeVisualiserUpdate() const { return needTreeVisualiserUpdate_; }
  inline bool &needTreeVisualiserUpdate() { return needTreeVisualiserUpdate_; }
  inline void setNeedTreeVisualiserUpdate(bool val) { needTreeVisualiserUpdate_ = val; }

  void setImage(const QImage &image);

  void markTreeAsUnsynchronized();
  void markTreeAsSynchronized();
  void CPviewer_show_message(int WhichMessage);

  ReconMode reconMode() const { return reconMode_; }
  void setReconMode(ReconMode reconMode) { reconMode_ = reconMode; }
  inline bool GetInterpState(){return ThresCtl->getInterpState();}
  inline bool getUpperState() {return ThresCtl->getUpperState();}
  inline float getSaliencyValue() {return ThresCtl->getSaliencyVal();}
  inline float getHDValue() {return ThresCtl->getHDVal();}
   
  inline TreeVisualiser *getTreeVis() {return treeVis_;};
  inline MainWindow *getMainWindow() { return mainWindow_; }

  inline void clearNodeHighlight() { imageViewer_->removeOverlay(); treeVis_->clearNodeSelection(); }

protected slots:
  void imageMousePress(const QPointF &p, QMouseEvent *e);
  void ChangeDisplayImg(QImage img);
  void DisplaySelectedNodes(vector<int> selectedIntensity);
  void DisplayOrigImg();
  

protected:
  void wheelEvent(QWheelEvent *e);

  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);

  bool eventFilter(QObject *obj, QEvent *evt);
  void treeVis_NodeSelected();
  void treeVis_NodeUnselected();

private:
  void createDockTreeVisualiser();
  void createDockWidgetSdmd();
  
  void highlightNodes();

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

  MainWindow *mainWindow_;
};