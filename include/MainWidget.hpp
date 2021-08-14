#pragma once 

#include <QWidget>
#include <QString>
#include <ImageViewerWidget/ImageViewerWidget.hpp>
#include <MorphotreeWidget/MorphotreeWidget.hpp>
#include <MorphotreeWidget/Graphics/GNode.hpp>

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

private slots:
  void morphotreeWidget_onNodeMousePress(MorphotreeWidget::GNode *node);
  void morphotreeWidget_onNodeMouseRelease(MorphotreeWidget::GNode *node);
  void morphotreeWidget_onNodeMouseDoubleClick(MorphotreeWidget::GNode *node);

private:
  void createDockMorphotreeWidget();

private:
  bool needMorphotreeWidgetUpdate_;

  QDockWidget *dockMorphotreeWidget_;

  ImageViewerWidget::ImageViewerWidget *imageViewer_;
  MorphotreeWidget::MorphotreeWidget *morphotreeWidget_;
};