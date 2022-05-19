#pragma once 

#include <morphotree/core/box.hpp>
#include <IcicleMorphotreeWidget/Filtering/TreeFiltering.hpp>
#include <IcicleMorphotreeWidget/Filtering/AreaTreeFiltering.hpp>
#include "ImageViewerWidget/ImageViewerWidget.hpp"
#include <QDialog>
#include <QImage>

class QSpinBox;
class QLabel;

class AreaTreeFilteringDialog : public QDialog
{
public:
  using Box = morphotree::Box;
  using uint8 = morphotree::uint8;
  using MTree = morphotree::MorphologicalTree<uint8>;
  using MTreeType = morphotree::MorphoTreeType;
  using ImageViewerWidget = ImageViewerWidget::ImageViewerWidget;
  using AreaTreeFiltering = IcicleMorphotreeWidget::AreaTreeFiltering;
  using AreaTreeFilteringPtr = std::unique_ptr<AreaTreeFiltering>;

  AreaTreeFilteringDialog(const Box &domain, const std::vector<uint8> &f,
    QWidget *parent=nullptr);

  
  inline QImage resultImage() { return resultImage_.copy(); }

  
protected slots:
  void spinBoxArea_onValueChanged(int val);
  void btnApply_onClick();
  void btnCancel_onClick();


private:
  MTree tree_;
  QSpinBox *spinBoxArea_;
  QLabel *numberOfNodesLabel_;
  Box domain_;

  AreaTreeFilteringPtr filtering_;
  ImageViewerWidget *imageViewer_;

  QImage resultImage_;
};