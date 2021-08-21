#pragma once 

#include <QWidget>
#include <QString>
#include <ImageViewerWidget/ImageViewerWidget.hpp>
#include <MorphotreeWidget/Graphics/GNode.hpp>

#include "TreeVisualiser/TreeVisualiser.hpp"

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

  MyDockWidget* createDockWidget(const QString &title, QWidget *widget);

  void updateTreeVisualiser();

private:
  void createDockTreeVisualiser();

private:
  bool needTreeVisualiserUpdate_;

  QDockWidget *dockTreeVisualiser_;

  ImageViewerWidget::ImageViewerWidget *imageViewer_;
  TreeVisualiser *treeVis_;
};