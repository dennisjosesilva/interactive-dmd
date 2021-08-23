#pragma once 

#include <QWidget>
#include <QMap>

#include <MorphotreeWidget/Graphics/GNode.hpp>
#include <MorphotreeWidget/MorphotreeWidget.hpp>
#include <ImageViewerWidget/ImageViewerWidget.hpp>

#include <morphotree/tree/mtree.hpp>
#include <morphotree/core/box.hpp>
#include <morphotree/core/alias.hpp>

#include <QDockWidget>
#include <QString>

class RecNodeButton;
class QGraphicsSceneMouseEvent;
class MainWidget;

class MyDockWidget : public QDockWidget
{
Q_OBJECT

public:
  using uint32 = morphotree::uint32;
  using GNode = MorphotreeWidget::GNode;

  MyDockWidget(const QString &title, QWidget *mainwindow);

  inline GNode *gnode() { return gnode_; } 
  inline const GNode *gnode() const { return gnode_; }
  inline void setGNode(GNode *gnode) { gnode_ = gnode; }
  
  void closeEvent(QCloseEvent *e) override;
  
public:
signals:
  void closed(MyDockWidget *dock);

private:
  GNode* gnode_;
};


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
  
  
public slots:
  void binRecDock_onClose(MyDockWidget *dock);
  void binRecDockPlus_onClose(MyDockWidget *dock);

private:
  struct NodeDockPair { 
    GNode *node;
    MyDockWidget *dock;

    inline bool isEmpty() { return node == nullptr && dock == nullptr; }
  };


private:
  MorphotreeWidget::MorphotreeWidget *treeWidget_;
  RecNodeButton *binRecButton_;
  RecNodeButton *greyRecButton_;
  morphotree::Box domain_;

  MyDockWidget *binRecDock_;
  QMap<uint32, MyDockWidget *> binRecPlusDeck_;

  MainWidget *mainWidget_;
};