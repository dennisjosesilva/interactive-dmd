#include "TreeVisualiser/TreeVisualiserStylePanel.hpp"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QDoubleSpinBox>


TreeVisualiserStylePanel::TreeVisualiserStylePanel(TreeVisualiser *treeVis, 
  QWidget *parent): QFrame{parent}, treeVis_{treeVis}
{
  QLayout *layout = new QVBoxLayout;
  layout->addItem(createTitle());
  layout->addWidget(createRenderStyleSection());
  layout->addItem(createMeasuresSection());
  setLayout(layout);
}

QLayout* TreeVisualiserStylePanel::createTitle()
{
  
  QLayout *layout = new QVBoxLayout;
  
  QLabel *titleLabel = new QLabel{tr("MorphoTree Style"), this};
  titleLabel->setStyleSheet(
    "font-weight: bold;"
  );
  layout->addWidget(titleLabel);  
  layout->addWidget(createHLine());

  return layout;
}

QWidget *TreeVisualiserStylePanel::createHLine()
{
  QFrame *line = new QFrame{this};
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  return line;
}

QGroupBox *TreeVisualiserStylePanel::createRenderStyleSection()
{
  QLayout *layout = new QVBoxLayout;

  QGroupBox *groupBox = new QGroupBox{tr("Render Style")};
  QRadioButton *flatRadioButton = new QRadioButton{tr("Flat")};
  QRadioButton *gradientRadioButton = new QRadioButton{tr("Gradient")};
  gradientRadioButton->setChecked(true);

  layout->addWidget(flatRadioButton);
  layout->addWidget(gradientRadioButton);

  groupBox->setLayout(layout);
  return groupBox;
}

QLayout *TreeVisualiserStylePanel::createMeasuresSection()
{
  QLayout *layout = new QVBoxLayout; 
  QLabel *label = new QLabel{tr("Measures"), this};
  label->setStyleSheet("font-weight: bold;");
  layout->addWidget(label);
  layout->addWidget(createHLine());

  QLayout *unitHeightLayout = new QHBoxLayout;
  QLabel *uniHeightLabel = new QLabel{tr("Unit Height: "), this};
  
  QDoubleSpinBox *unitHeightSpinBox = new QDoubleSpinBox{this};
  unitHeightSpinBox->setRange(0.1, 80);
  unitHeightSpinBox->setSingleStep(0.5);
  unitHeightSpinBox->setValue(5.0);
  unitHeightLayout->addWidget(uniHeightLabel);
  unitHeightLayout->addWidget(unitHeightSpinBox);

  layout->addItem(unitHeightLayout);
  return layout;
}