#include "ManipulateCPs/DataAugmentationDialog.hpp"
#include "ManipulateCPs/ManipulateCPs.hpp"
#include "ManipulateCPs/node.hpp"

#include <QDoubleSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include <QGraphicsScene>

#include <random>
#include <chrono>

// ======================================================================
// MinMaxRandomNumGenerator 
// ============================================== ========================
MinMaxRandomNumGenerator::MinMaxRandomNumGenerator(float min, float max)
  : min_{min}, max_{max},
    dist_{(min + max) / 2.0f, max-min},
    generator_{std::chrono::system_clock::now().time_since_epoch().count()}
{
}

MinMaxRandomNumGenerator::MinMaxRandomNumGenerator(unsigned int seed, 
  float min, float max)
  : min_{min}, max_{max},
    dist_{(min + max) / 2.0f, max-min},
    generator_{seed}
{

}

float MinMaxRandomNumGenerator::gen() 
{
  float val;
  do {
    val = dist_(generator_);

    // while val is outside [min,max] interval,
    // we should still generating samples.
  } while (!(min_ <= val && val <= max_));

  return val;
}

// ===========================================================================
// DataAugmentationDialog 
// ===========================================================================
DataAugmentationDialog::DataAugmentationDialog(ManipulateCPs *manipulateCPs,
  QWidget *parent)
  : QDialog{parent}, manipulateCPs_{manipulateCPs}
{
  QVBoxLayout *layout = new QVBoxLayout;
  inputLayout_ = new QGridLayout;

  createDXInput();
  createDYInput();
  createRadiusInput();
  createRotationInput();
  createScaleInput();
  createGenerateBtn();
  
  layout->addLayout(inputLayout_);
  setLayout(layout);
}

void DataAugmentationDialog::createDXInput()
{
  dxCheckBox_ = new QCheckBox{tr("dx"), this};
  dxCheckBox_->setChecked(true);
  connect(dxCheckBox_, &QCheckBox::stateChanged, this, 
    &DataAugmentationDialog::dxCheckBox_stateChanged);

  dxStartSpinBox_ = new QDoubleSpinBox{this};
  dxStartSpinBox_->setRange(-99.99, 99.99);
  dxStartSpinBox_->setValue(-0.5);

  dxEndSpinBox_ = new QDoubleSpinBox{this};
  dxEndSpinBox_->setRange(-99.99, 99.99);
  dxEndSpinBox_->setValue(0.5);

  inputLayout_->addWidget(dxCheckBox_, 0, 0);
  inputLayout_->addWidget(new QLabel{"from", this}, 0, 1);
  inputLayout_->addWidget(dxStartSpinBox_, 0, 2);
  inputLayout_->addWidget(new QLabel{"to", this}, 0, 3);
  inputLayout_->addWidget(dxEndSpinBox_, 0, 4);
  inputLayout_->addWidget(new QLabel{"pixels", this}, 0, 5);
}

void DataAugmentationDialog::createDYInput()
{
  dyCheckBox_ = new QCheckBox{tr("dy"), this};
  dyCheckBox_->setChecked(true);
  connect(dyCheckBox_, &QCheckBox::stateChanged, this, 
    &DataAugmentationDialog::dyCheckBox_stateChanged);

  dyStartSpinBox_ = new QDoubleSpinBox{this}; 
  dyStartSpinBox_->setRange(-99.99, 99.99);
  dyStartSpinBox_->setValue(-0.5);

  dyEndSpinBox_ = new QDoubleSpinBox{this};
  dyEndSpinBox_->setRange(-99.99, 99.99);
  dyEndSpinBox_->setValue(0.5);

  inputLayout_->addWidget(dyCheckBox_, 1, 0);
  inputLayout_->addWidget(new QLabel{"from", this}, 1, 1);
  inputLayout_->addWidget(dyStartSpinBox_, 1, 2);
  inputLayout_->addWidget(new QLabel{"to", this}, 1, 3);
  inputLayout_->addWidget(dyEndSpinBox_, 1, 4);
  inputLayout_->addWidget(new QLabel{"pixels", this}, 1, 5);
}

void DataAugmentationDialog::createRadiusInput()
{
  radiusCheckBox_ = new QCheckBox{tr("radius"), this};
  radiusCheckBox_->setChecked(true);
  connect(radiusCheckBox_, &QCheckBox::stateChanged, this, 
    &DataAugmentationDialog::radiusCheckBox_stateChanged);

  radiusStartSpinBox_ = new QDoubleSpinBox{this};
  radiusStartSpinBox_->setRange(0.0, 99.99);
  radiusStartSpinBox_->setValue(0.5);
  
  radiusEndSpinBox_ = new QDoubleSpinBox{this};
  radiusEndSpinBox_->setRange(0.0, 99.99);
  radiusEndSpinBox_->setValue(2.0);

  inputLayout_->addWidget(radiusCheckBox_, 2, 0);
  inputLayout_->addWidget(new QLabel{"from", this}, 2, 1);
  inputLayout_->addWidget(radiusStartSpinBox_, 2, 2);
  inputLayout_->addWidget(new QLabel{"to", this}, 2, 3);
  inputLayout_->addWidget(radiusEndSpinBox_, 2, 4);
  inputLayout_->addWidget(new QLabel{"times", this}, 2, 5);
}

void DataAugmentationDialog::createRotationInput()
{
  rotationCheckBox_ = new QCheckBox{tr("rotation"), this};
  rotationCheckBox_->setChecked(true);
  connect(rotationCheckBox_, &QCheckBox::stateChanged, this, 
    &DataAugmentationDialog::rotationCheckBox_stateChanged);

  rotationStartSpinBox_ = new QDoubleSpinBox{this};
  rotationStartSpinBox_->setRange(0.0, 360.0);
  rotationStartSpinBox_->setValue(0.0);

  rotationEndSpinBox_ = new QDoubleSpinBox{this};
  rotationEndSpinBox_->setRange(0.0, 360.0);
  rotationEndSpinBox_->setValue(5.0);

  inputLayout_->addWidget(rotationCheckBox_, 3, 0);
  inputLayout_->addWidget(new QLabel{"from", this}, 3, 1);
  inputLayout_->addWidget(rotationStartSpinBox_, 3, 2);
  inputLayout_->addWidget(new QLabel{"to", this}, 3, 3);
  inputLayout_->addWidget(rotationEndSpinBox_, 3, 4);
  inputLayout_->addWidget(new QLabel{"degrees"}, 3, 5);
}

void DataAugmentationDialog::createScaleInput()
{
  scaleCheckBox_ = new QCheckBox{tr("scale"), this};
  scaleCheckBox_->setChecked(true);
  connect(scaleCheckBox_, &QCheckBox::stateChanged, this, 
    &DataAugmentationDialog::scaleCheckBox_stateChanged);

  scaleStartSpinBox_ = new QDoubleSpinBox{this};
  scaleStartSpinBox_->setRange(0.0, 99.99);
  scaleStartSpinBox_->setValue(0.5);

  scaleEndSpinBox_ = new QDoubleSpinBox{this};
  scaleEndSpinBox_->setRange(0.0, 99.99);
  scaleEndSpinBox_->setValue(2.0);

  inputLayout_->addWidget(scaleCheckBox_, 4, 0);
  inputLayout_->addWidget(new QLabel{"from", this}, 4, 1);
  inputLayout_->addWidget(scaleStartSpinBox_, 4, 2);
  inputLayout_->addWidget(new QLabel{"to", this}, 4, 3);
  inputLayout_->addWidget(scaleEndSpinBox_, 4, 4);
  inputLayout_->addWidget(new QLabel{"times", this}, 4, 5);
}

void DataAugmentationDialog::createGenerateBtn()
{
  generateBtn_ = new QPushButton(tr("Generate changes"), this);
  connect(generateBtn_, &QPushButton::clicked, this, 
    &DataAugmentationDialog::generateBtn_onClicked);
  inputLayout_->addWidget(generateBtn_, 5, 5);
}

// ============================================================
// CHECK BOX EVENT 
// ============================================================
void DataAugmentationDialog::dxCheckBox_stateChanged(int state)
{
  if (state == Qt::Checked) {
    dxStartSpinBox_->setEnabled(true);
    dxEndSpinBox_->setEnabled(true);
  }
  else {
    dxStartSpinBox_->setEnabled(false);
    dxEndSpinBox_->setEnabled(false);
  }
}

void DataAugmentationDialog::dyCheckBox_stateChanged(int state)
{
  if (state == Qt::Checked) {
    dyStartSpinBox_->setEnabled(true);
    dyEndSpinBox_->setEnabled(true);
  }
  else {
    dyStartSpinBox_->setEnabled(false);
    dyEndSpinBox_->setEnabled(false);
  }
}

void DataAugmentationDialog::radiusCheckBox_stateChanged(int state)
{
  if (state == Qt::Checked) {
    radiusStartSpinBox_->setEnabled(true);
    radiusEndSpinBox_->setEnabled(true);
  }
  else {
    radiusStartSpinBox_->setEnabled(false);
    radiusEndSpinBox_->setEnabled(false);
  }
}

void DataAugmentationDialog::rotationCheckBox_stateChanged(int state)
{
  if (state == Qt::Checked) {
    rotationStartSpinBox_->setEnabled(true);
    rotationEndSpinBox_->setEnabled(true);
  }
  else {
    rotationStartSpinBox_->setEnabled(false);
    rotationEndSpinBox_->setEnabled(false);
  }
}

void DataAugmentationDialog::scaleCheckBox_stateChanged(int state)
{
  if (state == Qt::Checked) {
    scaleStartSpinBox_->setEnabled(true);
    scaleEndSpinBox_->setEnabled(true);
  }
  else {
    scaleStartSpinBox_->setEnabled(false);
    scaleEndSpinBox_->setEnabled(false);
  }
}

// =============================================================================
// Generate random change
// =============================================================================
void DataAugmentationDialog::generateBtn_onClicked()
{
  QList<Node_ *> cps = manipulateCPs_->selectedCPs();
  if (cps.size() == 0)
    cps = manipulateCPs_->allCPs();

  generateTranslation(cps);

  if (radiusCheckBox_->isChecked())
    generateRadius(cps);

  if (rotationCheckBox_->isChecked())
    generateRotation(cps);

  if (scaleCheckBox_->isChecked())
    generateScale(cps);
}

void DataAugmentationDialog::generateTranslation(const QList<Node_ *> cps)
{
  MinMaxRandomNumGenerator dyRandGen{dyStartSpinBox_->value(), 
    dyEndSpinBox_->value()};

  MinMaxRandomNumGenerator dxRandGen{dxStartSpinBox_->value(), 
    dxEndSpinBox_->value()};
  
  for (Node_ *cp : cps) {
    qreal dx = 0.0f;
    qreal dy = 0.0f;

    if (dxCheckBox_->isChecked()) {
      dx = dxRandGen.gen();
    }
    else if (dyCheckBox_->isChecked()) {      
      dy = dyRandGen.gen();
    }
    manipulateCPs_->translateCP(cp, dx, dy);
  }
}

void DataAugmentationDialog::generateRadius(const QList<Node_ *> &cps)
{
  MinMaxRandomNumGenerator randGen{ radiusStartSpinBox_->value(),
    radiusEndSpinBox_->value() };
  
  for (Node_ *cp : cps){
    float r = randGen.gen();
    qDebug()<<" randValue: " << r;
    manipulateCPs_->scaleRadius(cp, randGen.gen());
  }
}

void DataAugmentationDialog::generateRotation(const QList<Node_ *> &cps)
{
  MinMaxRandomNumGenerator randGen{ rotationStartSpinBox_->value(), 
    rotationEndSpinBox_->value() };

  qreal cx = 0;
  qreal cy = 0;

  for (Node_ *cp : cps) {
    cx += cp->x();
    cy += cp->y();
  }

  cx /= static_cast<qreal>(cps.size());
  cy /= static_cast<qreal>(cps.size());

  qreal angle = randGen.gen();
  for (Node_ *cp : cps) {
    manipulateCPs_->rotateCP(cp, cx, cy, angle);
  }
}

void DataAugmentationDialog::generateScale(const QList<Node_ *> &cps)
{
  MinMaxRandomNumGenerator randGen{ scaleStartSpinBox_->value(), 
    scaleEndSpinBox_->value() };

  qreal cx = 0;
  qreal cy = 0;

  for (Node_ *cp : cps) {
    cx += cp->x();
    cy += cp->y();
  }

  cx /= static_cast<qreal>(cps.size());
  cy /= static_cast<qreal>(cps.size());

  qreal scale = randGen.gen();
  for (Node_ *cp : cps) {
    manipulateCPs_->scaleCP(cp, cx, cy, scale);
  }
}