#include "QtUtil.h"

#include <iostream>

namespace facesynthesizing::infrastructure::qtgui {
	void visualizeImage(std::shared_ptr<usecases::Image> image, QLabel* container, QLabel* identifier)
	{
        if (identifier != nullptr && image != nullptr) {
            identifier->setText(QString::fromStdString(usecases::imageTypeToString(image->type)));
        }

        if (image != nullptr) {
            QImage qimage = QImage(image->data.get(), image->width, image->height, 
                static_cast<int>(image->width * image->channels * image->bytesPerChannelValue), 
                qformatFromImage(image));
            qimage = qimage.scaled(container->width(), container->height(), Qt::AspectRatioMode::KeepAspectRatio);
            container->setPixmap(QPixmap::fromImage(qimage));
        }
        else
            container->clear();
	}
    QImage::Format qformatFromImage(std::shared_ptr<usecases::Image> image)
    {
        if (image->channels == 1 && image->bytesPerChannelValue == 1)
            return QImage::Format::Format_Grayscale8;
        else if (image->channels == 3 && image->bytesPerChannelValue == 1)
            return QImage::Format::Format_RGB888;
        else if (image->channels == 4 && image->bytesPerChannelValue == 1)
            return QImage::Format::Format_ARGB32;

        throw std::exception("Cannot get QImage::Format from image: Unknown Format!");
    }
}