#pragma once

#include "../usecases/FaceSynthesizingDataStructures.h"

#include <QLabel>
#include <QImage>

namespace usecases = facesynthesizing::domain::usecases;

namespace facesynthesizing::infrastructure::qtgui {
	void visualizeImage(std::shared_ptr<usecases::Image> image, QLabel* container, QImage::Format format);
}