#include "QtUtil.h"

namespace facesynthesizing::infrastructure::qtgui {
	void visualizeImage(std::shared_ptr<usecases::Image> image, QLabel* container, QImage::Format format)
	{
        if (image != nullptr) {
            QImage qimage = QImage(image->data.get(), image->width, image->height, format);
            qimage = qimage.scaled(container->width(), container->height(), Qt::AspectRatioMode::KeepAspectRatio);
            container->setPixmap(QPixmap::fromImage(qimage));
        }
        else
            container->clear();
	}
}