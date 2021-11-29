#pragma once 

#include <QWidget>
#include <QImage>
#include <QMap>

#include <IcicleMorphotreeWidget/Graphics/Node/GNode.hpp>
#include <IcicleMorphotreeWidget/IcicleMorphotreeWidget.hpp>
#include <IcicleMorphotreeWidget/Graphics/ColorBar.hpp>
#include "TreeVisualiser/SimpleImageViewer.hpp"
#include "TreeVisualiser/AttributeComputer.hpp"
#include "CustomWidgets/TitleColorBar.hpp"

#include <morphotree/tree/mtree.hpp>
#include <morphotree/core/box.hpp>
#include <morphotree/core/alias.hpp>
#include "ManipulateCPs/CpViewer.hpp"

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
  using GNode = IcicleMorphotreeWidget::GNode;

  MyDockWidget(const QString &title, QWidget *mainwindow);

  inline GNode *gnode() { return gnode_; } 
  inline const GNode *gnode() const { return gnode_; }
  inline void setGNode(GNode *gnode) { gnode_ = gnode; }
  
  void closeEvent(QCloseEvent *e) override;
  
  QSize sizeHint() const override;

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
  using ColorBar = IcicleMorphotreeWidget::ColorBar;
  using GNode = IcicleMorphotreeWidget::GNode;  
  using uint8 = morphotree::uint8;
  using uint32 = morphotree::uint32;

  TreeVisualiser(MainWidget *mainWidget);

  void loadImage(Box domain, const std::vector<morphotree::uint8> &f);
  void selectNodeByPixel(int x, int y);  
  inline uint32 numberOfNodesOfTree() { return treeWidget_->mtree().numberOfNodes(); }  
  void registerDMDSkeletons();
  
  void showArea();
  void showPerimeter();
  void showVolume();
  void showCircularity();
  void showComplexity();
  void showNumberOfSkeletonPoints();

  void clearAttributes();

  Box domain() const { return domain_; }

  inline bool hasNodeSelected() const { return curNodeSelection_ != nullptr; }
  inline GNode* curSelectedNode() { return curNodeSelection_; }
  std::vector<bool> recSimpleNode() const;
  std::vector<bool> recFullNode() const;

  void useGradientGNodeStyle();
  void useFixedColorGNodeStyle();

  float unitHeightNode() const;
  void setUnitHeightNode(float val);

  inline void updateTreeRendering() { treeWidget_->updateTreeRendering(); }

protected:  
  std::vector<uint8> bool2UInt8(const std::vector<bool> &binimg) const;
  
  // FIELD<float> *binimageToField(const std::vector<uint32> &pidx) const;
  FIELD<float> *binImageToField(const std::vector<bool> &bimg) const;
  FIELD<float> *greyImageToField(const std::vector<uint8> &img) const;
  QImage fieldToQImage(FIELD<float> *field) const; 

  void reconstructBinaryImage(SimpleImageViewer *iv, NodePtr node);
  void reconstructGreyImage(SimpleImageViewer *iv, NodePtr node);

  QLayout* createButtons();  

  inline const MTree& mtree() const { return treeWidget_->mtree(); }
  inline uint32 numberOfNodesMtree() const { return treeWidget_->mtree().numberOfNodes(); }

public: 
signals: 
  void associateNodeToSkeleton(int numberOfNodes);
  void nodeSelected(GNode *node);
  void nodeUnselected(GNode *node);

protected slots:
  void nodeMousePress(GNode *node, QGraphicsSceneMouseEvent *e);

  void binRecDock_onClose(MyDockWidget *dock);
  void greyRecDock_onClose(MyDockWidget *dock);
  void skelRecDock_onClose(MyDockWidget *dock);
  void removeSkelDock_onClose(MyDockWidget *dock);

  void binRecBtn_press();
  void binRecPlusBtn_press();
  void greyRecBtn_press();
  void greyRecPlusBtn_press(); 
  void skelRecBtn_press(); 
  void removeSkelBtn_press();
  
private:
  uint32 maxValue_;
  IcicleMorphotreeWidget::IcicleMorphotreeWidget *treeWidget_;
  morphotree::Box domain_;
  
  GNode* curNodeSelection_;

  MyDockWidget *binRecDock_;
  MyDockWidget *greyRecDock_;
  MyDockWidget *skelRecDock_;
  MyDockWidget *removeSkelDock_;

  MainWidget *mainWidget_;
    
  dmdProcess dmd_;
  dmdReconstruct* dmdrecon_;

  AttributeComputer attrCompueter_;
  TitleColorBar *curColorBar_;
  CpViewer *cv = nullptr;
};