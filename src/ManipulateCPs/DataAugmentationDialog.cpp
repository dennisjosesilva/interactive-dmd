#include "ManipulateCPs/DataAugmentationDialog.hpp"
#include "ManipulateCPs/ManipulateCPs.hpp"
#include "ManipulateCPs/node.hpp"

#include "SDMD/dmdReconstruct.hpp"

#include <QDoubleSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QLineEdit>
#include <QSpinBox>
#include <limits>
#include <QFileDialog>
#include <QComboBox>
#include <QDir>

#include <QGraphicsScene>

#include <random>
#include <chrono>

// ======================================================================
// MinMaxRandomNumGenerator 
// ============================================== ========================
MinMaxRandomNumGenerator::MinMaxRandomNumGenerator(float min, float max)
  : min_{min}, max_{max},
    dist_{(min + max) / 2.0f, max-min},
    generator_{std::random_device()()}
{ }

MinMaxRandomNumGenerator::MinMaxRandomNumGenerator(unsigned int seed, 
  float min, float max)
  : min_{min}, max_{max},
    dist_{(min + max) / 2.0f, max-min},
    generator_{seed}
{ }

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
  bool maxTree, dmdReconstruct *recon, QWidget *parent)
  : QDialog{parent}, manipulateCPs_{manipulateCPs}, 
    recon_{recon}, maxTree_{maxTree}
{
  QVBoxLayout *layout = new QVBoxLayout;
  inputLayout_ = new QGridLayout;

  createDisplacementInput();
  createRadiusInput();
  createRotationInput();
  createScaleInput();
  
  layout->addLayout(inputLayout_);

  layout->addItem(createGenerateBtn());

  layout->addWidget(createHLineFrame());
  layout->addSpacing(18);
  layout->addItem(createMultiSampleGeneratorPanel());

  setLayout(layout);
}

void DataAugmentationDialog::createDisplacementInput()
{
  displacementCheckBox_ = new QCheckBox{tr("displacement"), this};
  displacementCheckBox_->setChecked(true);
  connect(displacementCheckBox_, &QCheckBox::stateChanged, this,
    &DataAugmentationDialog::displacementCheckBox_stateChanged);

  displacementSpinBox_ = new QDoubleSpinBox{this};
  displacementSpinBox_->setRange(0, 1000);
  displacementSpinBox_->setValue(5);

  inputLayout_->addWidget(displacementCheckBox_, 0, 0);
  inputLayout_->addWidget(new QLabel{"radius"}, 0, 1);
  inputLayout_->addWidget(displacementSpinBox_, 0, 2);
  inputLayout_->addWidget(new QLabel{"pixels"}, 0, 3);
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
  rotationStartSpinBox_->setRange(-360.0, 360.0);
  rotationStartSpinBox_->setValue(-15.0);

  rotationEndSpinBox_ = new QDoubleSpinBox{this};
  rotationEndSpinBox_->setRange(-360.0, 360.0);
  rotationEndSpinBox_->setValue(15.0);

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

QLayout *DataAugmentationDialog::createGenerateBtn()
{
  QHBoxLayout *hlayout = new QHBoxLayout;

  hlayout->setAlignment(Qt::AlignRight);

  generateBtn_ = new QPushButton(tr("Generate changes"), this);
  connect(generateBtn_, &QPushButton::clicked, this, 
    &DataAugmentationDialog::generateBtn_onClicked);
  hlayout->addWidget(generateBtn_);
  return hlayout;
}

// =======================================================================
// CHECK BOX EVENT 
// =======================================================================
void DataAugmentationDialog::displacementCheckBox_stateChanged(int state)
{
  if (state == Qt::Checked) {
    displacementSpinBox_->setEnabled(true);
  }
  else {
    displacementSpinBox_->setEnabled(false);
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
  generateRandomChanges();
}

void DataAugmentationDialog::generateTranslation(const QList<Node_ *> cps)
{
  float disRadius = static_cast<float>(displacementSpinBox_->value());

  MinMaxRandomNumGenerator dispRandGenDX{-disRadius, disRadius};
  MinMaxRandomNumGenerator dispRandGenDY{-disRadius, disRadius};

  for (Node_ *cp : cps) {
    qreal dx = 0.0f;
    qreal dy = 0.0f;

    dx = dispRandGenDX.gen();
    dy = dispRandGenDY.gen();
  
    manipulateCPs_->translateCP(cp, dx, dy);
  }
}

void DataAugmentationDialog::generateRadius(const QList<Node_ *> &cps)
{
  MinMaxRandomNumGenerator randGen{ 
    static_cast<float>(radiusStartSpinBox_->value()),
    static_cast<float>(radiusEndSpinBox_->value())};
  
  for (Node_ *cp : cps){
    float r = randGen.gen();
    qDebug()<<" randValue: " << r;
    manipulateCPs_->scaleRadius(cp, randGen.gen());
  }
}

void DataAugmentationDialog::generateRotation(const QList<Node_ *> &cps)
{
  MinMaxRandomNumGenerator randGen{ 
    static_cast<float>(rotationStartSpinBox_->value()), 
    static_cast<float>(rotationEndSpinBox_->value()) };

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
  MinMaxRandomNumGenerator randGen{ 
    static_cast<float>(scaleStartSpinBox_->value()), 
    static_cast<float>(scaleEndSpinBox_->value()) };

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

QLayout *DataAugmentationDialog::createMultiSampleGeneratorPanel()
{
  QVBoxLayout *vlayout = new QVBoxLayout;
  
  QLabel *titleLabel = new QLabel{tr("Multi Sample Generation"), this};
  QFont font = titleLabel->font();
  font.setPointSize(18);
  font.setBold(true);
  font.setUnderline(true);
  titleLabel->setFont(font);
  titleLabel->setAlignment(Qt::AlignCenter);
  vlayout->addWidget(titleLabel);
  
  vlayout->addSpacing(12);

  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->setSpacing(5);

  QLabel *directoryLabel = new QLabel{"directory: ", this};
  directoryLineEdit_ = new QLineEdit{this};
  directoryLineEdit_->setText(tr("resized-image"));
  QPushButton *browserBtn = new QPushButton{this};
  browserBtn->setText(tr("Browser..."));
  connect(browserBtn, &QPushButton::clicked, this, 
    &DataAugmentationDialog::browserBtn_onClick);
  gridLayout->addWidget(directoryLabel, 0, 0);
  gridLayout->addWidget(directoryLineEdit_, 0, 1);
  gridLayout->addWidget(browserBtn, 0, 2);

  QLabel *basenameLabel = new QLabel{"basename: ", this};
  basenameLineEdit_ = new QLineEdit{this};
  basenameLineEdit_->setText("");
  gridLayout->addWidget(basenameLabel, 1, 0);
  gridLayout->addWidget(basenameLineEdit_);

  QLabel *fileExtLabel = new QLabel{tr("file extension: "), this};
  fileExtComboBox_ = new QComboBox{this};
  fileExtComboBox_->addItems({"png", "pgm", "jpg", "tif", "gif"});
  fileExtComboBox_->setCurrentIndex(0);
  gridLayout->addWidget(fileExtLabel, 2, 0);
  gridLayout->addWidget(fileExtComboBox_, 2, 1);

  QLabel *startIndexLabel = new QLabel{"start index: ", this};
  startIndexSpinBox_ = new QSpinBox{this};
  startIndexSpinBox_->setRange(0, std::numeric_limits<int>::max());
  startIndexSpinBox_->setValue(0);
  startIndexSpinBox_->setSingleStep(1);
  gridLayout->addWidget(startIndexLabel, 3, 0);
  gridLayout->addWidget(startIndexSpinBox_, 3, 1);

  QLabel *numberOfSamplesLabel = new QLabel{"number of samples: ", this};
  numberOfSamplesSpinBox_ = new QSpinBox{this};
  numberOfSamplesSpinBox_->setRange(1, 100);
  numberOfSamplesSpinBox_->setSingleStep(1);
  gridLayout->addWidget(numberOfSamplesLabel, 4, 0);
  gridLayout->addWidget(numberOfSamplesSpinBox_, 4, 1);

  QHBoxLayout *hlayout = new QHBoxLayout;
  hlayout->setAlignment(Qt::AlignRight);

  generateMultiBtn_ = new QPushButton{tr("Generate multi samples"), this};
  connect(generateMultiBtn_, &QPushButton::clicked, this, 
    &DataAugmentationDialog::generateMultiBtn_onClick);
  hlayout->addWidget(generateMultiBtn_);

  vlayout->addLayout(gridLayout);
  vlayout->addSpacing(18);
  vlayout->addLayout(hlayout);
  return vlayout;
}

QFrame *DataAugmentationDialog::createHLineFrame()
{
  QFrame *hline = new QFrame;
  hline->setFrameShape(QFrame::HLine);
  return hline;
}

void DataAugmentationDialog::browserBtn_onClick()
{
  // QFileDialog dialog{this};
  // dialog.setOption(QFileDialog::Option::ShowDirsOnly, true);
  // dialog.setAcceptMode(QFileDialog::AcceptOpen);
  // dialog.setFileMode(QFileDialog::Directory);
  
  
  // bool accept = dialog.exec() == QFileDialog::Accepted;

  // if (accept && dialog.selectedFiles().count() > 0) {
  //   const QString directory = dialog.selectedFiles().constFirst();
  //   directoryLineEdit_->setText(directory);
  // }

  QString directory = QFileDialog::getExistingDirectory(0, 
    tr("Select output directory"), QDir::currentPath(), QFileDialog::Option::ShowDirsOnly);
  directoryLineEdit_->setText(directory);
}

void DataAugmentationDialog::generateMultiBtn_onClick()
{
  int startIndex = startIndexSpinBox_->value();
  int numberOfSamples = numberOfSamplesSpinBox_->value();
  QString directory = directoryLineEdit_->text();
  QString basename = basenameLineEdit_->text();

  for (int i=0; i < numberOfSamples; ++i) {
    int curIdx = startIndex + i;
    QString imgExt = fileExtComboBox_->currentText();
    QString ofile = tr("%0/%1%2.%3").arg(directory, basename, 
      QString::number(curIdx), imgExt);
    
    generateRandomChanges();
    QImage img = reconImage();

    img.save(ofile);

    qDebug() << tr("%0 produced with random changes.").arg(ofile);
  }

  accept();
}

void DataAugmentationDialog::generateRandomChanges()
{
  QList<Node_ *> cps = manipulateCPs_->selectedCPs();
  if (cps.size() == 0)
    cps = manipulateCPs_->allCPs();

  if (displacementCheckBox_->isChecked())
    generateTranslation(cps);

  if (radiusCheckBox_->isChecked())
    generateRadius(cps);

  if (rotationCheckBox_->isChecked())
    generateRotation(cps);

  if (scaleCheckBox_->isChecked())
    generateScale(cps);
}

QImage DataAugmentationDialog::reconImage()
{
  manipulateCPs_->ReconFromMovedCPs(recon_, maxTree_);
  manipulateCPs_->ReconImageFromMovedCPs(recon_, maxTree_);

  return recon_->getOutQImage();
}