#include "TreeVisualiser/TreeVisualiserStylePanel.hpp"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

TreeVisualiserStylePanel::TreeVisualiserStylePanel(TreeVisualiser *treeVis, 
  QWidget *parent): QFrame{parent}, treeVis_{treeVis}
{
  QLayout *layout = new QVBoxLayout;
  layout->addItem(createTitle());
  setLayout(layout);
}

QLayout* TreeVisualiserStylePanel::createTitle()
{
  QLayout *layout = new QHBoxLayout;

  QLabel *titleLabel = new QLabel{tr("MorphoTree Style"), this};
  layout->addWidget(titleLabel);

  return layout;
}