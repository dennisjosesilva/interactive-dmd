#pragma once 

#include <QWidget>
#include <QMap>

#include <MorphotreeWidget/Graphics/GNode.hpp>
#include <MorphotreeWidget/MorphotreeWidget.hpp>
#include "TreeVisualiser/SimpleImageViewer.hpp"

#include <morphotree/tree/mtree.hpp>
#include <morphotree/core/box.hpp>
#include <morphotree/core/alias.hpp>

#include <QDockWidget>
#include <QString>

#include "dmdProcess.hpp"
#include "dmdReconstruct.hpp"

class RecNodeButton;
class QGraphicsSceneMouseEvent;
class MainWidget;
class QSlider;

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
  using NodePtr = typename MTree::NodePtr;
  using TreeSimplification = MorphotreeWidget::TreeSimplification;
  using GNode = MorphotreeWidget::GNode;  
  using uint8 = morphotree::uint8;
  using uint32 = morphotree::uint32;

  TreeVisualiser(MainWidget *mainWidget);

  void loadImage(Box domain, const std::vector<morphotree::uint8> &f);

  void selectNodeByPixel(int x, int y);

protected:  
  std::vector<uint8> bool2UInt8(const std::vector<bool> &binimg) const;
  
  // FIELD<float> *binimageToField(const std::vector<uint32> &pidx) const;
  FIELD<float> *binimageToField(const std::vector<bool> &bimg) const;
  void registerDMDSkeletons();

  void reconstructBinaryImage(SimpleImageViewer *iv, NodePtr node);
  void reconstructGreyImage(SimpleImageViewer *iv, NodePtr node);

  QLayout* createButtons();
  QLayout* createTreeSimplificationControls();

  std::shared_ptr<TreeSimplification> duplicateTreeSimplification();  

protected slots:
  void nodeMousePress(GNode *node, QGraphicsSceneMouseEvent *e);

  void binRecDock_onClose(MyDockWidget *dock);
  void greyRecDock_onClose(MyDockWidget *dock);

  void binRecBtn_press();
  void binRecPlusBtn_press();
  void greyRecBtn_press();
  void greyRecPlusBtn_press();  

  void inspectNodePlusBtn_press();
  void inspectNodeMinusBtn_press();

  void numberLeavesSlider_onValueChange(int val);
  void areaSlider_onValueChange(int val);
  void areaDiffSlider_onValueChange(int val);

private:
  MorphotreeWidget::MorphotreeWidget *treeWidget_;
  morphotree::Box domain_;

  QSlider *numberLeavesSlider_;
  QLabel *numberLeavesValueLabel_;

  QSlider *areaSlider_;
  QLabel *areaValueLabel_;

  QSlider *areaDiffSlider_;
  QLabel *areaDiffValueLabel_;

  GNode* curNodeSelection_;

  MyDockWidget *binRecDock_;
  MyDockWidget *greyRecDock_;

  MainWidget *mainWidget_;
  std::shared_ptr<TreeSimplification> treeSimplification_;

  dmdProcess dmd_;
  dmdReconstruct dmdrecon_;
};