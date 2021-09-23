#pragma once 

#include <QWidget>
#include <QString>
#include <ImageViewerWidget/ImageViewerWidget.hpp>
#include <MorphotreeWidget/Graphics/GNode.hpp>

#include "TreeVisualiser/TreeVisualiser.hpp"
#include "InteractiveSdmd.hpp"
#include <QDockWidget>

class MainWidget : public QWidget
{  
  Q_OBJECT
public:
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

protected slots:
  void imageMousePress(const QPointF &p);

protected:
  void wheelEvent(QWheelEvent *e);

  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);

  bool eventFilter(QObject *obj, QEvent *evt);

private:
  void createDockTreeVisualiser();
  void createDockWidgetSdmd();


private:
  bool ctrlDown_;

  bool needTreeVisualiserUpdate_;

  bool nodeSelectionByClickActivated_;

  QDockWidget *dockTreeVisualiser_;
  QDockWidget *dockWidgetSdmd_;

  ImageViewerWidget::ImageViewerWidget *imageViewer_;
  TreeVisualiser *treeVis_;

  InteractiveSdmd *Interactive_sdmd;
};