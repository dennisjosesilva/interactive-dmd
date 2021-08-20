#pragma once 

#include <QWidget>
#include <QMap>

#include <MorphotreeWidget/Graphics/GNode.hpp>
#include <MorphotreeWidget/MorphotreeWidget.hpp>
#include <ImageViewerWidget/ImageViewerWidget.hpp>

#include <morphotree/tree/mtree.hpp>
#include <morphotree/core/box.hpp>
#include <morphotree/core/alias.hpp>

class RecNodeButton;
class QGraphicsSceneMouseEvent;
class QDockWidget;
class MainWidget;

class TreeVisualiser : public QWidget
{
Q_OBJECT

public:
  using MTree = morphotree::MorphologicalTree<morphotree::uint8>;
  using Box = morphotree::Box;
  using TreeSimplification = MorphotreeWidget::TreeSimplification;
  using GNode = MorphotreeWidget::GNode;
  using ImageViewerWidget = ImageViewerWidget::ImageViewerWidget;
  using uint8 = morphotree::uint8;
  using uint32 = morphotree::uint32;

  TreeVisualiser(MainWidget *mainWidget);

  void loadImage(Box domain, const std::vector<morphotree::uint8> &f,
    std::shared_ptr<TreeSimplification> treeSimplification);

protected:
  void nodeMousePress(GNode *node,  QGraphicsSceneMouseEvent *e);

  std::vector<uint8> bool2UInt8(const std::vector<bool> binimg) const;
  void binRecDock_onDestroy(QObject *dock);
  void binRecDockPlus_onDestroy(QObject *dock);

private:
  struct NodeDockPair { 
    GNode *node;
    QDockWidget *dock;

    inline bool isEmpty() { return node == nullptr && dock == nullptr; }
  };


private:
  MorphotreeWidget::MorphotreeWidget *treeWidget_;
  RecNodeButton *binRecButton_;
  RecNodeButton *greyRecButton_;
  morphotree::Box domain_;

  NodeDockPair binRecDock_;
  GNode *lastbinRecNodeSelected_;
  QMap<uint32, NodeDockPair> binRecPlusDock_;


  MainWidget *mainWidget_;
};