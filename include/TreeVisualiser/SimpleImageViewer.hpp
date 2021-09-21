#include <ImageViewerWidget/ImageViewerWidget.hpp>
#include <QWidget>

class QPushButton;

class SimpleImageViewer : public QWidget
{
Q_OBJECT

protected:
  using ImageViewerWidget = ImageViewerWidget::ImageViewerWidget;

public:
  SimpleImageViewer(QWidget *parent=nullptr);

  inline void setImage(const QImage &newImage) { imageViewer_->setImage(newImage); }

  QSize sizeHint() const override;

protected slots:
  void zoomIn_onClick();
  void zoomOut_onClick();

protected:
  void wheelEvent(QWheelEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);

  bool eventFilter(QObject *obj, QEvent *e);

protected:
  bool isCtrlDown_;

  ImageViewerWidget *imageViewer_;
};

