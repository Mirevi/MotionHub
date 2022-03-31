#include "PythonFaceSynthesizer.h"

#include <iostream>

namespace facesynthesizing::infrastructure::python {
	FaceSynthesizer::FaceSynthesizer() {
		executionStop = std::make_shared<std::atomic<bool>>();
		executionStop->store(false);
	}

	void FaceSynthesizer::setCaptureInfos(std::shared_ptr<usecases::CaptureDataInformation> infos)
	{
		PyGILState_STATE state = PyGILState_Ensure();
		try
		{
			captureInfos = infos;

			captureLocals = py::dict();
			captureLocals["options"] = captureInfosToDict();
			py::exec(
				"capture_validator = create_capture_validator(options)\n"
				, globals, captureLocals);
		}
		catch (py::error_already_set&)
		{
			handlePythonError();
		}
		PyEval_SaveThread();
	}
	py::dict FaceSynthesizer::captureInfosToDict()
	{
		py::dict dict;
		dict["face_aabbox_tracking_method"] = boundingBoxAlgorithmToPythonName(captureInfos->boundingBoxAlgorithm);
		dict["face_alignment_tracking_method"] = faceAlignmentAlgorithmToPythonName(captureInfos->faceAlignmentAlgorithm);
		dict["min_face_size"] = captureInfos->minFaceSize;
		dict["max_pitch"] = captureInfos->maxPitch;
		dict["max_roll"] = captureInfos->maxRoll;
		dict["max_yaw"] = captureInfos->maxYaw;
		dict["visualize"] = true;

		return dict;
	}
	bool FaceSynthesizer::validateCapture(usecases::CaptureDataPair capture)
	{
		PyGILState_STATE state = PyGILState_Ensure();
		bool isValid = false;
		try
		{
			captureLocals["image_color"] = imageToNpImage(capture.colorImage);

			py::object pyisValid =
				py::eval("is_capture_valid(capture_validator, image_color)", globals, captureLocals);
			isValid = py::extract<bool>(pyisValid);

			py::exec(
				"image_face_bb = capture_validator.get_face_bb_image()\n"
				"image_face_alignment = capture_validator.get_face_alignment_image()\n"

				"if image_face_bb is not None:\n"
				"	image_face_bb = switch_color_format(image_face_bb)\n"
				"if image_face_alignment is not None:\n"
				"	image_face_alignment = switch_color_format(image_face_alignment)\n"

				, globals, captureLocals
			);

			processCaptureImage("image_face_bb", usecases::ImageType::Capture_FaceBoundingBox);
			processCaptureImage("image_face_alignment", usecases::ImageType::Capture_FaceAlignment);
		}
		catch (py::error_already_set&)
		{
			handlePythonError();
		}
		PyEval_SaveThread();
		return isValid;
	}
	void FaceSynthesizer::processCaptureImage(std::string name, usecases::ImageType type)
	{
		py::extract<np::ndarray> npimageFaceAlignment(captureLocals[name]);
		if (!npimageFaceAlignment.check()) {
			faceSynthesizing->noImageAvailable(type);
			return;
		}

		std::shared_ptr<usecases::Image> imageFaceAlignment = npImageToImage(npimageFaceAlignment);
		imageFaceAlignment->type = type; 
		faceSynthesizing->visualizeImage(imageFaceAlignment);
	}

	void FaceSynthesizer::setConvertInfos(std::shared_ptr<usecases::ConvertDataInformation> infos)
	{
		PyGILState_STATE state = PyGILState_Ensure();
		try
		{
			convertInfos = infos;

			convertLocals = py::dict();
			convertLocals["options"] = convertInfosToDict();
			py::exec(
				"data_converter = create_convert_data(options)\n"
				, globals, convertLocals);
		}
		catch (py::error_already_set &)
		{
			handlePythonError();
		}
		PyEval_SaveThread();
	}
	py::dict FaceSynthesizer::convertInfosToDict()
	{
		py::dict dict;
		dict["continue_process"] = convertInfos->continueProcess;
		dict["depth_padding"] = convertInfos->depthPadding;
		dict["df_algorithm"] = depthFillingAlgorithmToPythonName(convertInfos->depthFillingAlgorithm);
		dict["df_blur_ksize"] = convertInfos->depthFillingBlurKernelSize;
		dict["df_ext_patch_size"] = convertInfos->depthFillingExtendedPatchSize;
		dict["df_image_size"] = convertInfos->depthFillingImageSize;
		dict["df_no_blur"] = !convertInfos->depthFillingUseBlur;
		dict["df_padding"] = convertInfos->depthFillingPadding;
		dict["df_patch_size"] = convertInfos->depthFillingPatchSize;
		dict["df_source_amount"] = convertInfos->depthFillingSourceAmount;
		dict["df_visualize"] = true;
		dict["eye_tracking_method"] = eyeTrackingAlgorithmToPythonName(convertInfos->eyeTrackingAlgorithm);
		dict["eye_tracking_stride"] = convertInfos->eyeTrackingStride;
		dict["eye_tracking_threshold"] = convertInfos->eyeTrackingThreshold;
		dict["face_depth"] = convertInfos->faceDepth;
		dict["face_tracking_method"] = faceTrackingAlgorithmToPythonName(convertInfos->faceTrackingAlgorithm);
		dict["feature_image_line_thickness"] = convertInfos->flmLineThickness;
		dict["horizontal_fov"] = convertInfos->horizontalFoV;
		dict["input_root"] = captureRoot.string();
		dict["name"] = convertInfos->name;
		dict["no_depth_filling"] = !convertInfos->useDepthFilling;
		dict["output_image_size"] = convertInfos->outputImageSize;
		dict["output_root"] = datasetRoot.string();
		dict["overwrite"] = convertInfos->overwrite;
		dict["padding"] = convertInfos->padding;
		dict["pca_image_components"] = convertInfos->pcaImageComponents;
		dict["pca_landmarks_components"] = convertInfos->pcaLandmarkComponents;
		dict["visualize"] = true;

		return dict;
	}
	void FaceSynthesizer::convertData()
	{
		PyGILState_STATE state = PyGILState_Ensure();

		py::exec(
			"convert_data(data_converter)\n"
			, globals, convertLocals
		);

		PyEval_SaveThread();
	}

	void FaceSynthesizer::setTrainInfos(std::shared_ptr<usecases::TrainingInformation> infos)
	{
		PyGILState_STATE state = PyGILState_Ensure();
		try
		{
			std::cout << "setTrainInfos" << std::endl;

			trainingInfos = infos;

			trainingLocals = py::dict();
			trainingLocals["options"] = trainingInfosToDict();
			py::exec(
				"training = create_train(options)\n"
				, globals, trainingLocals);
		}
		catch (py::error_already_set&)
		{
			handlePythonError();
		}
		PyEval_SaveThread();
	}
	py::dict FaceSynthesizer::trainingInfosToDict()
	{
		py::dict dict;
		dict["continue_process"] = trainingInfos->continueProcess;
		dict["overwrite"] = trainingInfos->overwrite;
		dict["name"] = trainingInfos->name;
		dict["color_format"] = colorFormatToPythonName(trainingInfos->colorFormat);
		dict["continue_epoch"] = trainingInfos->continueEpoch;
		dict["dataset_name"] = trainingInfos->datasetName;
		dict["batch_size"] = trainingInfos->trainingBatchSize;
		dict["eval_batch_size"] = trainingInfos->evaluationBatchSize;
		dict["data_loader_threads"] = 0;
		dict["dataset_image_size"] = trainingInfos->datasetImageSize;
		dict["max_dataset_size"] = trainingInfos->maxDatasetSize;
		dict["max_eval_dataset_size"] = trainingInfos->maxDatasetSize;
		dict["use_serial_batches"] = trainingInfos->useSerialBatches;
		dict["no_depth_mask"] = !trainingInfos->useDepthMask;
		dict["model_type"] = modelTypeToPythonName(trainingInfos->modelType);
		dict["netG"] = generatorArchitectureToPythonName(trainingInfos->generatorArchitecture, trainingInfos->datasetImageSize);
		dict["netD"] = discriminatorArchitectureToPythonName(trainingInfos->discriminatorArchitecture);
		dict["gan_mode"] = ganModeToPythonName(trainingInfos->ganMode);
		dict["ngf"] = trainingInfos->generatorFilters;
		dict["ndf"] = trainingInfos->discriminatorFilters;
		dict["norm_type"] = normalizationLayerTypeToPythonName(trainingInfos->normalizationLayerType);
		dict["no_dropout"] = !trainingInfos->useDropout;
		dict["num_D"] = trainingInfos->discriminators;
		dict["n_layers_D"] = trainingInfos->discriminatorLayers;
		dict["use_mapping_network"] = trainingInfos->useMappingNetwork;
		dict["netM"] = mappingNetworkToPythonName(trainingInfos->mappingNetwork);
		dict["n_epochs"] = trainingInfos->lrConsistentEpochs;
		dict["n_epochs_decay"] = trainingInfos->lrDecayingEpochs;
		dict["beta1"] = trainingInfos->beta1;
		dict["learning_rate"] = trainingInfos->learningRate;
		dict["lr_policy"] = learningRatePolicyToPythonName(trainingInfos->learningRatePolicy);
		dict["lr_decay_iters"] = trainingInfos->stepDecayIterations;
		dict["init_type"] = weightInitTypeToPythonName(trainingInfos->weightInitType);
		dict["init_gain"] = trainingInfos->weightInitGain;
		dict["no_L1_loss"] = !trainingInfos->useL1Loss;
		dict["use_gan_feat_loss"] = trainingInfos->useFeatureMatchingLoss;
		dict["use_lpips_loss"] = trainingInfos->useLPIPSLoss;
		dict["use_cycle_loss"] = trainingInfos->useCycleLoss;
		dict["lambda_L1"] = trainingInfos->lambdaL1;
		dict["lambda_gan_feat"] = trainingInfos->lambdaFeatureMatching;
		dict["lambda_lpips"] = trainingInfos->lambdaLPIPS;
		dict["lambda_cycle_forward"] = trainingInfos->lambdaCycleForward;
		dict["lambda_cycle_backward"] = trainingInfos->lambdaCycleBackward;
		dict["save_epoch_freq"] = trainingInfos->checkpointFrequency;
		dict["eval_epoch_freq"] = trainingInfos->evaluationFrequency;
		dict["pixel_acc_thresh"] = trainingInfos->pixelAccuracyMetricThreshold;
		dict["visdom_env"] = trainingInfos->visdomEnvironment;
		dict["visdom_ncols"] = trainingInfos->visdomImageColumns;
		dict["no_visdom"] = !trainingInfos->useVisdom;
		dict["visdom_port"] = trainingInfos->visdomServerPort;
		dict["no_visdom_server_start"] = true;
		dict["visdom_url"] = trainingInfos->visdomServerURL;
		dict["dataset_root"] = datasetRoot.string();
		dict["output_root"] = checkpointsRoot.string();

		return dict;
	}
	void FaceSynthesizer::train()
	{
		PyGILState_STATE state = PyGILState_Ensure();

		py::exec(
			"train(training)\n"
			, globals, trainingLocals
		);

		PyEval_SaveThread();
	}

	void FaceSynthesizer::requestExecutionStopAsync()
	{
		executionStop->store(true);
	}
	void FaceSynthesizer::resetExecutionStop()
	{
		executionStop->store(false);
	}

	void FaceSynthesizer::newPythonImage(py::object pyImage, std::string identifier)
	{
		usecases::ImageType imageType = pyImageIdentifierToImageType(identifier);
		if (imageType == usecases::ImageType::Unknown)
			return;

		py::extract<np::ndarray> npImage(pyImage);
		if (!npImage.check()) {
			faceSynthesizing->noImageAvailable(imageType);
			return;
		}

		std::shared_ptr<usecases::Image> image = npImageToImage(npImage);
		image->type = imageType;
		faceSynthesizing->visualizeImage(image);
	}
	void FaceSynthesizer::newPythonStatus(std::string statusMessage)
	{
		faceSynthesizing->newProjectStatusMessage(statusMessage);
	}
	void FaceSynthesizer::updatePythonStatus(std::string statusMessage)
	{
		faceSynthesizing->updateProjectStatusMessage(statusMessage);
	}
	bool FaceSynthesizer::isExecutionStopRequested()
	{
		return executionStop->load();
	}

	void FaceSynthesizer::handlePythonError()
	{
		PythonErrorMetadata metadata = getPythonErrorMetadata();
		PyErr_Print();
		PyErr_Clear();
		throw PythonException(metadata);
	}
	PythonErrorMetadata FaceSynthesizer::getPythonErrorMetadata()
	{
		PyObject* ptype, * pvalue, * ptraceback;
		PyErr_Fetch(&ptype, &pvalue, &ptraceback);

		std::string errorMessage = "Unknown";
		std::string filename = "Unknown";
		std::string funcname = "Unknown";
		long lineNumber = -1;

		if (pvalue != nullptr) {
			py::handle<> hvalue(pvalue);
			py::object value(hvalue);

			py::dict locals;
			locals["value"] = value;
			errorMessage = py::extract<std::string>(py::eval("str(value)", globals, locals));
		}
		if (ptraceback != nullptr) {
			py::handle<> hTraceback(ptraceback);
			py::object traceback(hTraceback);

			py::dict locals;
			locals["traceback"] = traceback;
			filename = py::extract<std::string>(py::eval("str(traceback.tb_frame.f_code.co_filename)", globals, locals));
			funcname = py::extract<std::string>(py::eval("str(traceback.tb_frame.f_code.co_name)", globals, locals));
			lineNumber = py::extract<long>(py::eval("traceback.tb_lineno", globals, locals));
		}

		PythonErrorMetadata metadata = PythonErrorMetadata();
		metadata.message = errorMessage;
		metadata.filename = filename;
		metadata.functionName = funcname;
		metadata.linenumber = lineNumber;

		return metadata;
	}

	void FaceSynthesizer::setGlobals(py::object globals)
	{
		this->globals = globals;
		try
		{
			importPythonModules();
			addStandardImportsToGlobals();
			definePythonFunctions();
			definePythonClasses();
			addObjectsToGlobals();
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
		python_import_module("config", globals);
		python_import_module("data.validation", globals);
		python_import_module("convert_data", globals);
		python_import_module("train", globals);
		python_import_module("util", globals);
	}
	void FaceSynthesizer::addStandardImportsToGlobals()
	{
		py::dict locals;
		py::exec(
			"import sys\n"
			"import traceback\n"
			"import cv2 as cv\n"
			"import numpy as np\n"
			"from multiprocessing.pool import ThreadPool\n"
			"from config import new_standard_config, ConfigType\n"
			"from data.validation import CaptureValidator\n"
			"from convert_data import ConvertData\n"
			"from train import Train\n"
			"from util import "
			"VisualizationListener, ExecutionStopper, ColorFormat, switch_color_format\n"
			"from mmh_wrapper import MMHFaceSynthesizer"
			, globals, locals);
		globals.attr("update")(locals);
	}
	void FaceSynthesizer::definePythonFunctions()
	{
		py::dict locals;
		py::exec(
			"def create_capture_validator(options: dict):\n"
			"	capture_validator_config = new_standard_config(ConfigType.Capture_Validator, options)\n"
			"	capture_validator_config.gather_options()\n"
			"	capture_validator_config.print(detailed_package_description=False)\n"
			"	capture_validator = CaptureValidator(capture_validator_config)\n"
			"	return capture_validator\n"

			"def create_convert_data(options: dict):\n"
			"	convert_data_config = new_standard_config(ConfigType.Convert, options)\n"
			"	convert_data_config.gather_options()\n"
			"	convert_data_config.print(detailed_package_description=False)\n"
			"	convert_data = ConvertData(convert_data_config, mmh_visualizer, mmh_execution_stopper)\n"
			"	print('created ConvertData instance')\n"
			"	return convert_data\n"

			"def create_train(options: dict):\n"
			"	train_config = new_standard_config(ConfigType.Train, options)\n"
			"	train_config.gather_options()\n"
			"	train_config.print(detailed_package_description=False)\n"
			"	training = Train(train_config, mmh_visualizer, mmh_execution_stopper)\n"
			"	return training\n"

			"def append_alpha_channel(image_color):\n"
			"	alpha_channel = np.full((image_color.shape[0], image_color.shape[0], 1), 255, np.uint8)\n"
			"	return np.concatenate((image_color, alpha_channel), axis=2)\n"

			"def is_capture_valid(capture_validator, image_color):\n"
			"	return capture_validator.is_color_image_valid(image_color[:, :, 0:3])\n"

			"def convert_data(data_converter):\n"
			"	data_converter()\n"

			"def train(training):\n"
			"	training()\n"

			, globals, locals
		);
		globals.attr("update")(locals);
	}
	void FaceSynthesizer::definePythonClasses()
	{
		py::dict locals;
		py::exec(
			"class VisualizerMMHAdapter(VisualizationListener):\n"
			"	def __init__(self, mmh_interface):\n"
			"		self.mmh = mmh_interface\n"

			"	def new_image(self, image, identifier, color_format: ColorFormat):\n"
			"		if image.dtype == np.float64:\n"
			"			image = image.astype(np.uint8)\n"
			"		\n"
			"		if color_format is not None and color_format is not ColorFormat.RGB:\n"
			"			image = switch_color_format(image)\n"
			"		\n"
			"		self.mmh.new_python_image(image, identifier)\n"

			"	def visualize(self):\n"
			"		pass\n"

			"	def new_status(self, status_message):\n"
			"		self.mmh.new_python_status(status_message)\n"

			"	def update_status(self, status_message):\n"
			"		self.mmh.update_python_status(status_message)\n"


			"class MMHExecutionStopper(ExecutionStopper):\n"
			"	def __init__(self, mmh_interface):\n"
			"		self.mmh = mmh_interface\n"

			"	def is_requesting_stop(self):\n"
			"		return self.mmh.is_execution_stop_requested()\n"

			, globals, locals
		);
		globals.attr("update")(locals);
	}
	void FaceSynthesizer::addObjectsToGlobals()
	{
		std::cout << "addObjectsToGlobals" << std::endl;
		py::dict locals;
		locals["mmh_interface"] = this;
		py::exec(
			//"runner = ThreadPool(processes=1)\n"
			"mmh_visualizer = VisualizerMMHAdapter(mmh_interface)\n"
			"mmh_execution_stopper = MMHExecutionStopper(mmh_interface)\n"
			"data_converter = None\n"
			, globals, locals
		);
		globals.attr("update")(locals);
		std::cout << "addObjectsToGlobals finished" << std::endl;
	}
	void FaceSynthesizer::setFaceSynthesizingProjectInteractor(std::shared_ptr<usecases::FaceSynthesizingProjectInteractor> faceSynthesizing)
	{
		this->faceSynthesizing = faceSynthesizing;
	}
	void FaceSynthesizer::setCaptureRoot(fs::path root)
	{
		this->captureRoot = root;
	}
	void FaceSynthesizer::setDatasetRoot(fs::path root)
	{
		this->datasetRoot = root;
	}
	void FaceSynthesizer::setCheckpointsRoot(fs::path root)
	{
		this->checkpointsRoot = root;
	}
}