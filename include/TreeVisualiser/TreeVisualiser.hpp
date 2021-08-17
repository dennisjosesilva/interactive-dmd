#pragma once 

#include <QWidget>
#include <MorphotreeWidget/MorphotreeWidget.hpp>

#include <morphotree/tree/mtree.hpp>
#include <morphotree/core/box.hpp>
#include <morphotree/core/alias.hpp>

class RecNodeButton;

class TreeVisualiser : public QWidget
{
Q_OBJECT

public:
  using MTree = morphotree::MorphologicalTree<morphotree::uint8>;
  using Box = morphotree::Box;
  using TreeSimplification = MorphotreeWidget::TreeSimplification;

  TreeVisualiser(QWidget *parent=nullptr);

  void loadImage(Box domain, const std::vector<morphotree::uint8> &f,
    std::shared_ptr<TreeSimplification> treeSimplification);

private:
  MorphotreeWidget::MorphotreeWidget *treeWidget_;
  RecNodeButton *binRecButton_;
  RecNodeButton *greyRecButton_;
};