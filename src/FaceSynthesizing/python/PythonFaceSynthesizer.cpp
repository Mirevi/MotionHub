#include "PythonFaceSynthesizer.h"

#include <iostream>

namespace facesynthesizing::infrastructure::python {
	void FaceSynthesizer::setCaptureInfos(std::shared_ptr<usecases::CaptureDataInformation> infos)
	{
		std::cout << "setCaptureInfos" << std::endl;
		captureInfos = infos;

		/*
		 continue_process: False                         
        eval_image_amount: 250                           
face_aabbox_tracking_method: SynergyNet                    
face_alignment_tracking_method: SynergyNet                    
          image_id_offset: 0                             
                max_pitch: 10.0                          
                 max_roll: 10.0                          
                  max_yaw: 10.0                          
                     name: new_data                      
              output_root: C:\develop\Projects\Face Tracking\Face-Avatar-Synthesizer\datasets\unprocessed
                overwrite: False                         
       train_image_amount: 1500                          
                visualize: False      
		*/

		try
		{
			py::dict captureOptions;
			captureOptions["face_aabbox_tracking_method"] = usecases::boundingBoxAlgorithmToString(captureInfos->boundingBoxAlgorithm);
			captureOptions["face_alignment_tracking_method"] = usecases::faceAlignmentAlgorithmToString(captureInfos->faceAlignmentAlgorithm);
			captureOptions["max_pitch"] = captureInfos->maxPitch;
			captureOptions["max_roll"] = captureInfos->maxRoll;
			captureOptions["max_yaw"] = captureInfos->maxYaw;
			captureOptions["visualize"] = captureInfos->visualize;

			py::dict locals;
			locals["capture_infos"] = captureOptions;

			py::exec(
				"from data.validation import CaptureValidator\n"
				"from config import new_standard_config, ConfigType\n"
				"from tracking.face_aabbox_tracking import FaceAABBoxTrackingMethodCOP\n"
				"from tracking.face_alignment_tracking import FaceAlignmentTrackingMethodCOP\n"
				"\n"
				"capture_validator_config = new_standard_config(ConfigType.Capture_Validator, capture_infos)\n"
				"capture_validator_config.gather_options()\n"
				"capture_validator_config.print(detailed_package_description=False)\n"
				"capture_validator = CaptureValidator(capture_validator_config)\n",
				globals, locals);
			captureValidator = locals["capture_validator"];
		}
		catch (py::error_already_set const&)
		{
			PyErr_Print();
			PyErr_Clear();
		}
	}
	bool FaceSynthesizer::validateCapture(usecases::CaptureDataPair capture)
	{
		try
		{
			py::dict locals;
			locals["capture_validator"] = captureValidator;
			locals["image_color"] = imageToNpImage(capture.colorImage);

			py::object pyisValid = py::eval("runner.apply_async(is_capture_valid, [capture_validator, image_color]).get()", globals, locals);
			bool isValid = py::extract<bool>(pyisValid);

			if (!captureInfos->visualize)
				return isValid;

			py::exec(
				"image_face_alignment = capture_validator.get_face_alignment_image()\n"

				"if image_face_alignment is not None and image_face_alignment.shape[2] == 3:\n"
				"	image_face_alignment = append_alpha_channel(image_face_alignment)\n"
				, globals, locals
			);

			py::extract<np::ndarray> npimageFaceAlignment(locals["image_face_alignment"]);
			if (!npimageFaceAlignment.check()) {
				faceSynthesizing->noImageAvailable(usecases::ImageType::Capture_FaceAlignment);
				return isValid;
			}


			std::shared_ptr<usecases::Image> imageFaceAlignment = npImageToImage(npimageFaceAlignment);
			imageFaceAlignment->type = usecases::ImageType::Capture_FaceAlignment;
			faceSynthesizing->visualizeImage(imageFaceAlignment);

			return isValid;
		}
		catch (py::error_already_set const&)
		{
			PyErr_Print();
			PyErr_Clear();
		}
		return false;
	}

	void FaceSynthesizer::setGlobals(py::object globals)
	{
		this->globals = globals;
		try
		{
			importPythonModules();
			addStandardImportsToGlobals();
			definePythonFunctions();
			addRunnerToGlobals();
		}
		catch (py::error_already_set const&)
		{
			PyErr_Print();
			PyErr_Clear();
			exit(1);
		}
	}
	void FaceSynthesizer::importPythonModules()
	{
		python_import_module("util", globals);
		python_import_module("config", globals);
		python_import_module("data.validation", globals);
		python_import_module("tracking.face_aabbox_tracking", globals);
		python_import_module("tracking.face_alignment_tracking", globals);
	}
	void FaceSynthesizer::addStandardImportsToGlobals()
	{
		py::dict locals;
		py::exec(
			"import cv2 as cv\n"
			"import numpy as np\n"
			"from multiprocessing.pool import ThreadPool\n"
			, globals, locals);
		globals["cv"] = locals["cv"];
		globals["np"] = locals["np"];
		globals["ThreadPool"] = locals["ThreadPool"];
	}
	void FaceSynthesizer::definePythonFunctions()
	{
		py::dict locals;
		py::exec(
			"def append_alpha_channel(image_color):\n"
			"	alpha_channel = np.full((image_color.shape[0], image_color.shape[0], 1), 255, np.uint8)\n"
			"	return np.concatenate((image_color, alpha_channel), axis=2)\n"

			"def is_capture_valid(capture_validator, image_color):\n"
			"	return capture_validator.is_color_image_valid(image_color[:, :, 0:3])\n"

			, globals, locals
		);
		globals["append_alpha_channel"] = locals["append_alpha_channel"];
		globals["is_capture_valid"] = locals["is_capture_valid"];
	}
	void FaceSynthesizer::addRunnerToGlobals()
	{
		py::object runner = py::eval(
			"ThreadPool(processes = 1)"
			, globals);
		globals["runner"] = runner;
	}
	void FaceSynthesizer::setFaceSynthesizingProjectInteractor(std::shared_ptr<usecases::FaceSynthesizingProjectInteractor> faceSynthesizing)
	{
		this->faceSynthesizing = faceSynthesizing;
	}

	/*np::ndarray FaceSynthesizer::imageToNumpy(std::shared_ptr<usecases::Image> image)
	{
		std::size_t datasize = image->getDataSize();
		Py_intptr_t shape[1] = { datasize };
		np::ndarray result = np::zeros(1, shape, np::dtype::get_builtin<uint8_t>());
		std::copy(image->data.get(), image->data.get() + datasize, reinterpret_cast<uint8_t*>(result.get_data()));
		return result;
	}*/
}