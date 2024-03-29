#pragma once 

#include <QWidget>
#include <QImage>
#include <QMap>
#include <QVector>

#include <IcicleMorphotreeWidget/Graphics/Node/GNode.hpp>
#include <IcicleMorphotreeWidget/IcicleMorphotreeWidget.hpp>
#include <IcicleMorphotreeWidget/Graphics/ColorBar.hpp>
#include <IcicleMorphotreeWidget/Graphics/Node/OpenGLGNodeFactory.hpp>
#include "TreeVisualiser/SimpleImageViewer.hpp"
#include "TreeVisualiser/AttributeComputer.hpp"
#include "CustomWidgets/TitleColorBar.hpp"

#include <morphotree/tree/mtree.hpp>
#include <morphotree/core/box.hpp>
#include <morphotree/core/alias.hpp>
#include "ManipulateCPs/CpViewer.hpp"

#include <QDockWidget>
#include <QString>

#include "SDMD/dmdProcess.hpp"
#include "SDMD/dmdReconstruct.hpp"

class RecNodeButton;
class CollapsableWidget;
class QGraphicsSceneMouseEvent;
class MainWidget;
class QSlider;
class QComboBox;

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

private:
  enum NodeSelectionMode 
  {
    NoNodeSelected,
    OneNodeSelected,
    MoreThanOneNodeSelected
  };

public:
  using MTree = morphotree::MorphologicalTree<morphotree::uint8>;
  using Box = morphotree::Box;
  using NodePtr = typename MTree::NodePtr;  
  using ColorBar = IcicleMorphotreeWidget::ColorBar;
  using GNode = IcicleMorphotreeWidget::GNode;  
  using uint8 = morphotree::uint8;
  using uint32 = morphotree::uint32;  
  using OpenGLFactoryPresetPtr = std::shared_ptr<IcicleMorphotreeWidget::Preset>;
  using DefaultPreset = IcicleMorphotreeWidget::DefaultPreset;
  using MorphoTreeType = IcicleMorphotreeWidget::MorphoTreeType;
  using GrayScaleProfile = IcicleMorphotreeWidget::GrayScaleProfile;  

  TreeVisualiser(MainWidget *mainWidget);

  void loadImage(Box domain, const std::vector<morphotree::uint8> &f,
    MorphoTreeType mtreeType = MorphoTreeType::MAX_TREE_8C);

  void selectNodeByPixel(int x, int y, bool isCtrlDown=false);  
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

  GNode *curSelectedNode();
  GNode *curSelectedNode() const;
  inline bool hasNodeSelected() const { return curSelectedNodeIndex_ != InvalidNodeIndex; }
  void goToParent();
  std::vector<bool> recSimpleNode() const;
  std::vector<bool> recFullNode() const;
  std::vector<bool> morphoRecSelectedNodes() const;

  void useBilinearGradientStyle(OpenGLFactoryPresetPtr preset=
    std::make_shared<DefaultPreset>());
  void useGradientGNodeStyle();
  void useFixedColorGNodeStyle();
  void useBezierFuncGNodeStyle();

  inline void resetCache() { treeWidget_->resetCachedContent(); }
  inline void updateTreeRendering() { treeWidget_->updateTreeRendering(); }
  
  inline uint numberOfSelectedNodes() const { return selectedNodes_.count(); }

  //FIELD<float>* SDMDReconstruction(unsigned int id);
  std::vector<bool> SDMDRecontructionSelectedNodes();

  bool isNodeSelectedForReconstruction(unsigned int id) const { return selectedNodesForRec_[id]; }
  bool &isNodeSelectedForReconstruction(unsigned int id) { return selectedNodesForRec_[id]; }
  void setNodeSelectedForReconstruction(unsigned int id, bool val) { selectedNodesForRec_[id] = val; }

  const QVector<bool> &selectedNodesForReconstruction() const { return selectedNodesForRec_; }
  QVector<bool> &selectedNodesForReconstruction() { return selectedNodesForRec_; }

  void selectNodesForRecBasedOnIntensities(const std::vector<int> &sIntensities);

  IcicleMorphotreeWidget::IcicleMorphotreeWidget *treeWidget() { return treeWidget_; }

  void clearNodeSelection();

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

  void updateTransparencyOfTheNodes();  

public: 
signals: 
  void associateNodeToSkeleton(int numberOfNodes);
  void nodeSelectionChanged();
  void ChangeCentralWidget(CpViewer *cv);
  // void nodeUnselected();
 

protected slots:
  void nodeMousePress(GNode *node, QGraphicsSceneMouseEvent *e);

  void binRecDock_onClose(MyDockWidget *dock);

  void binRecBtn_press();
  void fitToWindowBtn_press();
  void zoomInBtn_press();
  void zoomOutBtn_press();
  void rotateWidgetBtn_press();
  void SplineManipulateBtn_press();
  void skelRecBtn_press(); 
  void selectDescendantNodes_press();
  void removeSkelBtn_press();
  void incNodeReconBtn_press();
  void remNodeReconBtn_press();
  void updateCustomTreeVisualisationWhenRedraw();

  void treeWidget_keyPress(QKeyEvent *e);

private:
  const uint32 InvalidNodeIndex = std::numeric_limits<uint32>::max();

  uint32 curSelectedNodeIndex_;
  bool shouldUpdateCustomTreeRedraw_;

  uint32 maxValue_;
  IcicleMorphotreeWidget::IcicleMorphotreeWidget *treeWidget_;
  morphotree::Box domain_;

  MyDockWidget *binRecDock_;
  MyDockWidget *SplineManipDock_;
  MyDockWidget *skelRecDock_;
  MyDockWidget *removeSkelDock_;

  MainWidget *mainWidget_;
    
  dmdProcess dmd_;
  dmdReconstruct* dmdrecon_;

  AttributeComputer attrCompueter_;
  TitleColorBar *curColorBar_;
  CpViewer *cv = nullptr;

  QVector<bool> selectedNodesForRec_;
  bool gradientRenderStyle_;
  QMap<uint32, GNode *> selectedNodes_;
  bool FirstCreateCpviewer = true;

  CollapsableWidget *collapsableStyle_;

  GrayScaleProfile grayscaleProfile_;
};
