#include "PythonImageWrapper.h"

#include <iostream>

namespace facesynthesizing::infrastructure::python {
	np::ndarray imageToNpImage(std::shared_ptr<usecases::Image> image)
	{
		py::tuple shape = py::make_tuple(image->height, image->width, image->channels);
		size_t pixelValueSize = sizeof(uint8_t) * image->bytesPerChannelValue;
		py::tuple stride = py::make_tuple(
			pixelValueSize * shape[2] * shape[1], 
			pixelValueSize * shape[2],
			pixelValueSize);
		np::dtype type = np::dtype::get_builtin<uint8_t>();

		return np::from_data(image->data.get(), type, shape, stride, py::object());
	}
	std::shared_ptr<usecases::Image> npImageToImage(np::ndarray npArray)
	{
		const int arrayDimensions = npArray.get_nd();

		if (arrayDimensions < 2 || arrayDimensions > 3) {
			std::string message = "Cant convert npimage with length of shape " + arrayDimensions;
			message += " to image.";
			throw std::invalid_argument(message);
		}

		std::shared_ptr<usecases::Image> image = std::make_shared<usecases::Image>();
		image->width = npArray.shape(1);
		image->height = npArray.shape(0);
		image->channels = (arrayDimensions == 2) ? 1 : npArray.shape(2);
		image->bytesPerChannelValue = (arrayDimensions == 2) ? npArray.strides(1) : npArray.strides(2);
		image->bytesPerChannelValue /= sizeof(uint8_t);
		size_t dataSize = image->getDataSize();
		uint8_t* buffer = reinterpret_cast<uint8_t*>(npArray.get_data());
		uint8_t* dataArray = new uint8_t[dataSize];
		std::copy(buffer, buffer + dataSize, dataArray);

		image->data = std::shared_ptr<uint8_t[]>(dataArray, usecases::array_deleter<uint8_t>());

		return image;
	}
}