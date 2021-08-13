#pragma once 

#include <QWidget>
#include <QString>
#include <ImageViewerWidget/ImageViewerWidget.hpp>
#include <MorphotreeWidget/MorphotreeWidget.hpp>

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
  
  inline MorphotreeWidget::MorphotreeWidget *morphotreeWidget() { return morphotreeWidget_; }
  QDockWidget *morphotreeDockWidget();

  void updateMorphotreeWidget();

private:
  void createDockMorphotreeWidget();

private:
  bool needMorphotreeWidgetUpdate_;

  QDockWidget *dockMorphotreeWidget_;

  ImageViewerWidget::ImageViewerWidget *imageViewer_;
  MorphotreeWidget::MorphotreeWidget *morphotreeWidget_;
};