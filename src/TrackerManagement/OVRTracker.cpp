#include "OVRTracker.h"


//<OpenVR id = "Tracker:FOREARM_R" >
//< position>0.000000 - 0.050000 0.090000 < / position >
//< rotation>9.555434 - 129.125565 - 83.462875 < / rotation >
//< / OpenVR>
//<OpenVR id = "Tracker:FOREARM_L">
//< position>0.000000 0.090000 -0.050000 < / position >
//< rotation>8.627157 100.982559 83.464706 < / rotation >
//< / OpenVR>

static bool DEBUG_DEVICES_ON_START = true;

static bool DEBUG_HMD = false;

static bool DEBUG_HIP = false;

static bool SOLVE_IK = true;

static bool USE_DEBUG_POINT_COLLECTION = true;

static bool USE_XSENS_SKELETON = false;

static bool USE_TEST_SKELETON = true;

static bool EVAL_TIMING = true;
static bool EVAL_DISTANCE = true;

static bool SCALE_SKELETON = true;


#include <vector>
#include <numeric>

static std::chrono::steady_clock::time_point evalStart;

class Evaluation {

public:

	Evaluation() {
		values = new std::vector<float>();
	}

	~Evaluation() {
		delete values;
	}

	void clearValues() {
		values->clear();
	}

	void addValue(float value) {
		values->push_back(value);
	}

	float getAverage() {
		return std::reduce(values->begin(), values->end()) / static_cast<float>(values->size());
	}

	float getMedian() {
		size_t n = values->size() / 2;
		std::nth_element(values->begin(), values->begin() + n, values->end());
		return values->at(n);
	}

	float getMin(float percent = 1.0f) {

		float percentCount = fmax(ceil(values->size() / 100.0f * percent), 1.0f);

		std::sort(values->begin(), values->end(), std::less<float>());

		float min = 0;
		for (size_t i = 0; i < percentCount && i < values->size(); i++) {
			min += values->at(i);
		}

		return min / fmin(percentCount, values->size());
		//return *std::min_element(values->begin(), values->end());
	}

	float getMax(float percent = 1.0f) {

		float percentCount = fmax(ceil(values->size() / 100.0f * percent), 1.0f);

		std::sort(values->begin(), values->end(), std::greater<float>());

		float max = 0;
		for (size_t i = 0; i < percentCount && i < values->size(); i++) {
			max += values->at(i);
		}

		return max / fmin(percentCount, values->size());
		//return *std::max_element(values->begin(), values->end());
	}

private:
	std::vector<float>* values;
};

class EvaluationV {

public:

	EvaluationV() {
		values = new std::vector<float>();

		estimations = new std::vector<Vector3f>();
		ground_truth = new std::vector<Vector3f>();
	}

	~EvaluationV() {
		delete values;

		delete estimations;
		delete ground_truth;
	}

	void clearValues() {
		values->clear();

		estimations->clear();
		ground_truth->clear();
	}

	void addValue(float value) {
		values->push_back(value);
	}

	void addValue(Vector3f& estimation, Vector4f& truth) {
		estimations->push_back(estimation);

		ground_truth->push_back(Vector3f(truth.x(), truth.y(), truth.z()));
	}

	void fillValues() {

		values->clear();

		for (unsigned int i = 0; i < estimations->size() && i < ground_truth->size(); i++) {

			values->push_back(1000.0f * distance(estimations->at(i), ground_truth->at(i)));
		}
	}

	float getAverage() {
		return std::reduce(values->begin(), values->end()) / static_cast<float>(values->size());
	}

	float getMedian() {
		size_t n = values->size() / 2;
		std::nth_element(values->begin(), values->begin() + n, values->end());
		return values->at(n);
	}

	float getMin(float percent = 1.0f) {

		float percentCount = fmax(ceil(values->size() / 100.0f * percent), 1.0f);

		std::sort(values->begin(), values->end(), std::less<float>());

		float min = 0;
		for (size_t i = 0; i < percentCount && i < values->size(); i++) {
			min += values->at(i);
		}

		return min / fmin(percentCount, values->size());
		//return *std::min_element(values->begin(), values->end());
	}

	float getMax(float percent = 1.0f) {

		float percentCount = fmax(ceil(values->size() / 100.0f * percent), 1.0f);

		std::sort(values->begin(), values->end(), std::greater<float>());

		float max = 0;
		for (size_t i = 0; i < percentCount && i < values->size(); i++) {
			max += values->at(i);
		}

		return max / fmin(percentCount, values->size());
		//return *std::max_element(values->begin(), values->end());
	}

	float getMSE() {

		float mse = 0;

		for (unsigned int i = 0; i < values->size(); i++) {

			mse += values->at(i) * values->at(i);
		}

		mse = mse / values->size();
		
		return mse;
	}

private:
	std::vector<float>* values;

	std::vector<Vector3f>* estimations;
	std::vector<Vector3f>* ground_truth;
};


static Evaluation timingIK;
static Evaluation timingIKTest;

static EvaluationV distanceLeftLeg;
static EvaluationV distanceRightLeg;

static EvaluationV distanceLeftFoot;
static EvaluationV distanceRightFoot;

static EvaluationV distanceHead;

static EvaluationV distanceLeftArm;
static EvaluationV distanceRightArm;

static EvaluationV distanceLeftHand;
static EvaluationV distanceRightHand;

static EvaluationV distanceLeftArmTest;
static EvaluationV distanceRightArmTest;

static EvaluationV distanceLeftHandTest;
static EvaluationV distanceRightHandTest;

static EvaluationV distanceLeftLegTest;
static EvaluationV distanceRightLegTest;

static EvaluationV distanceLeftFootTest;
static EvaluationV distanceRightFootTest;


static std::string getData(Evaluation& evaluation, std::string description) {

	return std::string(description + " avg:" + toString(evaluation.getAverage()) + " median:" + toString(evaluation.getMedian()) + " min1:" + toString(evaluation.getMin(1.0f)) + " min5:" + toString(evaluation.getMin(5.0f)) + " max1:" + toString(evaluation.getMax(1.0f)) + " max5:" + toString(evaluation.getMax(5.0f)));
}

static std::string getData(EvaluationV& evaluation, std::string description) {

	float mse = evaluation.getMSE();
	float rmse = sqrt(mse);
	return std::string(description + " avg:" + toString(evaluation.getAverage()) + " median:" + toString(evaluation.getMedian()) + " mse:" + toString(mse) + " rmse:" + toString(rmse) + " min1:" + toString(evaluation.getMin(1.0f)) + " min5:" + toString(evaluation.getMin(5.0f)) + " max1:" + toString(evaluation.getMax(1.0f)) + " max5:" + toString(evaluation.getMax(5.0f)));
}

static void clearEvalValues() {

	if (EVAL_TIMING) {
		timingIK.clearValues();
		timingIKTest.clearValues();
	}

	if (EVAL_DISTANCE) {

		distanceLeftLeg.clearValues();
		distanceRightLeg.clearValues();

		distanceLeftFoot.clearValues();
		distanceRightFoot.clearValues();

		distanceLeftArm.clearValues();
		distanceRightArm.clearValues();

		distanceLeftHand.clearValues();
		distanceRightHand.clearValues();

		distanceHead.clearValues();

		distanceLeftArmTest.clearValues();
		distanceRightArmTest.clearValues();

		distanceLeftHandTest.clearValues();
		distanceRightHandTest.clearValues();

		distanceLeftLegTest.clearValues();
		distanceRightLegTest.clearValues();

		distanceLeftFootTest.clearValues();
		distanceRightFootTest.clearValues();
	}
}

static void startTimer() {
	Console::logWarning("");
	Console::logWarning("Start Evaluation Timer");

	evalStart = std::chrono::high_resolution_clock::now();
}

static void printEvalValues() {

	auto evalEnd = std::chrono::high_resolution_clock::now();

	auto duration = evalEnd - evalStart;

	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
	duration -= seconds;

	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

	Console::logWarning("");
	Console::logWarning(std::to_string(seconds.count()) + ":" + std::to_string(milliseconds.count()));

	if (EVAL_TIMING) {
		Console::logWarning(getData(timingIK, "Timing"));

		if (USE_TEST_SKELETON) {
			Console::logError(getData(timingIKTest, "TimingT"));
		}

		Console::logWarning("");
	}

	if (EVAL_DISTANCE) {

		//distanceLeftLeg.fillValues();
		//Console::logWarning(getData(distanceLeftLeg, "LeftLeg"));

		//distanceRightLeg.fillValues();
		//Console::logWarning(getData(distanceRightLeg, "RightLeg"));

		distanceLeftFoot.fillValues();
		Console::logWarning(getData(distanceLeftFoot, "LeftFoot"));

		distanceRightFoot.fillValues();
		Console::logWarning(getData(distanceRightFoot, "RightFoot"));

		distanceLeftArm.fillValues();
		Console::logWarning(getData(distanceLeftArm, "LeftArm"));

		distanceRightArm.fillValues();
		Console::logWarning(getData(distanceRightArm, "RightArm"));

		distanceLeftHand.fillValues();
		Console::logWarning(getData(distanceLeftHand, "LeftHand"));

		distanceRightHand.fillValues();
		Console::logWarning(getData(distanceRightHand, "RightHand"));


		distanceHead.fillValues();
		Console::logWarning(getData(distanceHead, "Head"));


		if (USE_TEST_SKELETON) {

			Console::logError("");

			//distanceLeftLegTest.fillValues();
			//Console::logError(getData(distanceLeftLegTest, "LeftLegT"));

			//distanceRightLegTest.fillValues();
			//Console::logError(getData(distanceRightLegTest, "RightLegT"));

			//distanceLeftFootTest.fillValues();
			//Console::logError(getData(distanceLeftFootTest, "LeftFootT"));

			//distanceRightFootTest.fillValues();
			//Console::logError(getData(distanceRightFootTest, "RightFootT"));

			distanceLeftArmTest.fillValues();
			Console::logError(getData(distanceLeftArmTest, "LeftArmT"));

			distanceRightArmTest.fillValues();
			Console::logError(getData(distanceRightArmTest, "RightArmT"));

			distanceLeftHandTest.fillValues();
			Console::logError(getData(distanceLeftHandTest, "LeftHandT"));

			distanceRightHandTest.fillValues();
			Console::logError(getData(distanceRightHandTest, "RightHandT"));
		}
	}
}

static float distance4(Vector4f a, Vector4f b) {
	return distance(Vector3f(a.x(), a.y(), a.z()), Vector3f(b.x(), b.y(), b.z()));
}

static float distance34(Vector3f a, Vector4f b) {
	return distance(a, Vector3f(b.x(), b.y(), b.z()));
}

static bool isEvaluating;

static void StartEvaluating() {

	if (isEvaluating) {
		return;
	}

	isEvaluating = true;

	clearEvalValues();
	startTimer();
}

static void StopEvaluating() {

	if (!isEvaluating) {
		return;
	}

	isEvaluating = false;

	printEvalValues();
	clearEvalValues();
}

static std::string DebugIdentifier(std::string identifier) {

	if (identifier == "LHR-774276F7") {
		return "HEAD";
	}
	else if (identifier == "LHR-F0DCF515") {
		return "HAND_L";
	}
	else if (identifier == "LHR-9AC396F1") {
		return "HAND_R";
	}
	else if (identifier == "LHR-E6910BC0") {
		return "FOOT_L";
	}
	else if (identifier == "LHR-6E2EA3BC") {
		return "FOOT_R";
	}
	else if (identifier == "LHR-61441A45") {
		return "HIPS";
	}
	else if (identifier == "LHR-227770AA") {
		return "FOREARM_L";
	}
	else if (identifier == "LHR-EB83669E") {
		return "FOREARM_R";
	}
	else {
		return "! NOT FOUND !";
	}
}

OVRTracker::OVRTracker() {

	// Initialize OpenVR & IK Solvers
	init();
}

OVRTracker::OVRTracker(int id, NetworkManager* networkManager, ConfigManager* configManager) {

	m_networkManager = networkManager;
	m_configManager = configManager;

	//create new Properties object
	m_properties = new Properties();

	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_openVR_" + toString(id);

	//default is enabled
	m_isEnabled = true;

	// Initialize OpenVR, Config & IK Solvers
	init();

	readOffsetFromConfig();
}

OVRTracker::~OVRTracker() {

	clearPointers();
}

void OVRTracker::start() {

	// start tracking in tracking system
	trackingSystem->start();

	// Refresh values from config
	refresh(true);

	// set tracking to true
	m_isTracking = true;

	//there schould only be one skeleton in the file
	m_countDetectedSkeleton = m_skeletonPool.size();

	//skeleton was added/removed, so UI updates
	m_hasSkeletonPoolChanged = true;

	// Init IKSolvers
	initIKSolvers();

	// Clear point collection
	m_pointCollection.clear();

	// Loop over all devices and convert them to points
	for (const auto& device : trackingSystem->Devices) {

		// Cast device class to point type
		Point::PointType pointType = static_cast<Point::PointType>(device.deviceClass);

		// Add point to collection
		m_pointCollection.addPoint(device.index, pointType);

		// Update Joint if assigned
		Joint::JointNames joint = config->getJoint(device.index);
		if (joint != Joint::NDEF) {
			m_pointCollection.updatePoint(device.index, joint);
		}
	}

	if (USE_DEBUG_POINT_COLLECTION) {
		m_pointCollection.addPoint(Joint::SPINE, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::CHEST, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::NECK, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::HEAD, Point::PointType::Rigidbody);

		m_pointCollection.addPoint(Joint::UPLEG_L, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::LEG_L, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::FOOT_L, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::TOE_L, Point::PointType::Rigidbody);

		m_pointCollection.addPoint(Joint::UPLEG_R, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::LEG_R, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::FOOT_R, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::TOE_R, Point::PointType::Rigidbody);

		m_pointCollection.addPoint(Joint::SHOULDER_L, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::ARM_L, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::FOREARM_L, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::HAND_L, Point::PointType::Rigidbody);

		m_pointCollection.addPoint(Joint::SHOULDER_R, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::ARM_R, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::FOREARM_R, Point::PointType::Rigidbody);
		m_pointCollection.addPoint(Joint::HAND_R, Point::PointType::Rigidbody);
	}

	// register tracker as observers
	trackingSystem->registerButtonPressObserver(this);

	if (DEBUG_DEVICES_ON_START) {
		Console::log("");
		for (const auto& device : trackingSystem->Devices) {
			Console::log(toString((int)device.index) + " " + OpenVRTracking::toString(device.deviceClass) + " " + device.identifier + " " + Joint::toString(config->getJoint(device.index)));
			Console::log(DebugIdentifier(device.identifier));
		}
		Console::log("");
	}

	// Update Confidence for each tracked joint
	for (const auto& device : trackingSystem->Devices) {
		auto deviceJoint = config->getJoint(device.index);
		auto skeletonJoint = hierarchicSkeleton->getJoint(deviceJoint);

		if (deviceJoint != Joint::NDEF && skeletonJoint != nullptr) {
			skeletonJoint->setConfidence(Joint::HIGH);
		}
	}

	// Refresh IK Solvers
	refreshIKSolvers(false);

	// start tracking thread
	Tracker::start();

	clearEvalValues();
}

void OVRTracker::refresh(bool overrideDefaults) {

	// Refresh config from xml
	m_configManager->refresh();

	// Read global offsets
	readOffsetFromConfig();

	// Update prediction time from config
	//float mmhDelay = 0.0255f; // 0.0255f
	float predictionTime = config->readPredictionTime();
	if (predictionTime > 0.0f) {
		Console::log("OVRTracker::refresh: prediction time " + toString(predictionTime) + " s");
		trackingSystem->setPredictionTime(predictionTime);
	}

	// Read assigned devices from config
	config->readAssignedDevices();

	// Assign remaining devices
	config->updateUserDeviceRoles();

	// Read offset from config
	config->readOffsets();

	// Should use TestSkeleton?
	if (USE_TEST_SKELETON) {
		// Enable TestSkeleton if HintDevice is configured
		if (config->getDeviceIndex(Joint::LEG_L) >= 0
			|| config->getDeviceIndex(Joint::LEG_R) >= 0
			|| config->getDeviceIndex(Joint::FOREARM_L) >= 0
			|| config->getDeviceIndex(Joint::FOREARM_R) >= 0)
		{
			useTestSkeleton = true;
		}
	}

	// Init skeleton & set scale from config
	hierarchicSkeleton->setScale(config->readScale());
	if (USE_XSENS_SKELETON) {
		hierarchicSkeleton->initXsens();
	}
	else {
		hierarchicSkeleton->init();
	}

	if (useTestSkeleton) {
		testHierarchicSkeleton->setScale(config->readScale());
		if (USE_XSENS_SKELETON) {
			testHierarchicSkeleton->initXsens();
		}
		else {
			testHierarchicSkeleton->init();
		}
	}

	// Update Filters
	int i = 0;
	float beta = 1.0f;
	float cutoff = 0.007f;

	for (auto& device : trackingSystem->Devices) {
		Joint::JointNames joint = config->getJoint(device.index);

		// TODO: Parameter der 1 Euro Filter mit Framerate abstimmen

		if (EVAL_DISTANCE) {
			trackingSystem->positionFilters[i].setParams(30, beta, cutoff);
			trackingSystem->rotationFilters[i].setParams(60, beta, cutoff);
		}
		else if (joint == Joint::HAND_L || joint == Joint::FOREARM_L || joint == Joint::FOOT_L || joint == Joint::LEG_L) {
			trackingSystem->positionFilters[i].setParams(1, beta, cutoff);
			trackingSystem->rotationFilters[i].setParams(2, beta, cutoff);
		}
		else if (joint == Joint::HIPS) {
			trackingSystem->positionFilters[i].setParams(60, beta, cutoff);
			trackingSystem->rotationFilters[i].setParams(120, beta, cutoff);
		}
		else {
			trackingSystem->positionFilters[i].setParams(30, beta, cutoff);
			trackingSystem->rotationFilters[i].setParams(60, beta, cutoff);
		}

		i++;
	}

	// Refresh IKSolvers (if initialized)
	refreshIKSolvers(overrideDefaults);
}

void OVRTracker::stop() {

	// write settings & calibration to config 
	config->write();
	config->writeScale(hierarchicSkeleton->getScale());

	// remove tracker from observers
	trackingSystem->removeButtonPressObserver(this);

	Tracker::stop();

	if (EVAL_DISTANCE || EVAL_TIMING) {
		printEvalValues();
		clearEvalValues();
	}
}

void OVRTracker::init() {

	// Create & init OpenVR Tracking
	trackingSystem = new OpenVRTracking();

	try {
		trackingSystem->init();
	}
	catch (Exception exception) {
		delete trackingSystem;
		throw exception;
	}

	// Create IK Skeleton
	hierarchicSkeleton = new HierarchicSkeleton(m_properties->id);
	testHierarchicSkeleton = new HierarchicSkeleton(m_properties->id + 100);

	skeleton = new Skeleton(m_properties->id);
	testSkeleton = new Skeleton(m_properties->id + 100);

	// Init Config & try to write default config values
	config = new OpenVRConfig(m_configManager, trackingSystem);
	config->writeDefaults();
}

void OVRTracker::track() {

	// Update offsets from Inspector
	trackingSystem->setOffsets(m_properties->positionOffset, m_rotationOffset, m_properties->scaleOffset);

	// Poll Events in the Tracking System
	trackingSystem->pollEvents();

	// Update device poses
	trackingSystem->receiveDevicePoses();

	// Calibration toggle & Calibration
	if (GetAsyncKeyState('C') & 0x8000) {
		Console::log("OVRTracker: Should Calibrate");
		shouldCalibrate = true;
	}
	//else if (GetAsyncKeyState(0x20) & 0x8000) { // Space
	//	calibrate();
	//}

	// Calibration for target joints
	if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000) {
		calibrateDeviceToJointOffset(Joint::FOOT_L);
	}
	else if (GetAsyncKeyState(VK_NUMPAD3) & 0x8000) {
		calibrateDeviceToJointOffset(Joint::FOOT_R);
	}
	else if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000) {
		calibrateDeviceToJointOffset(Joint::HIPS);
	}
	else if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000) {
		calibrateDeviceToJointOffset(Joint::HAND_L);
	}
	else if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000) {
		calibrateDeviceToJointOffset(Joint::HAND_R);
	}

	/*if (GetAsyncKeyState('I') & 0x8000) {
		SOLVE_IK = true;
	}*/

	if (GetAsyncKeyState('R') & 0x8000) {
		refresh(false);
	}

	// Read Poses from TrackingSystem
	auto hipPose = getAssignedPose(Joint::HIPS);
	auto headPose = getAssignedPose(Joint::HEAD);

	auto leftLegPose = getAssignedPose(Joint::LEG_L);
	auto leftFootPose = getAssignedPose(Joint::FOOT_L);

	auto rightLegPose = getAssignedPose(Joint::LEG_R);
	auto rightFootPose = getAssignedPose(Joint::FOOT_R);

	auto leftForearmPose = getAssignedPose(Joint::FOREARM_L);
	auto leftHandPose = getAssignedPose(Joint::HAND_L);

	auto rightForearmPose = getAssignedPose(Joint::FOREARM_R);
	auto rightHandPose = getAssignedPose(Joint::HAND_R);

	if (SOLVE_IK) {

		bool hasLeftFootPose = !leftFootPose.isNull();
		bool hasRightFootPose = !rightFootPose.isNull();

		bool hasLeftLegPose = !leftLegPose.isNull();
		bool hasRightLegPose = !rightLegPose.isNull();

		bool hasLeftForearmPose = !leftForearmPose.isNull();
		bool hasRightForearmPose = !rightForearmPose.isNull();


		std::chrono::steady_clock::time_point start;
		std::chrono::steady_clock::time_point end;
		long long microseconds;


		// Hint Hip with both feets
		if (hasLeftFootPose && hasRightFootPose) {
			ikSolverHip->hint(leftFootPose.position, rightFootPose.position);
		}

		if (useTestSkeleton) {
			// Hint Hip with both feets
			if (hasLeftFootPose && hasRightFootPose) {
				testIkSolverHip->hint(leftFootPose.position, rightFootPose.position);
			}
		}

		// Hint LeftLeg
		if (hasLeftLegPose) {
			ikSolverLeftLeg->hint(leftLegPose.position, leftLegPose.rotation);
		}

		// Hint RightLeg
		if (hasRightLegPose) {
			ikSolverRightLeg->hint(rightLegPose.position, rightLegPose.rotation);
		}

		// Hint LeftArm
		if (hasLeftForearmPose) {
			ikSolverLeftArm->hint(leftForearmPose.position, leftForearmPose.rotation);
			ikSolverLeftArm->DebugPos4 = leftForearmPose.position;
		}

		// Hint RightArm
		if (hasRightForearmPose) {
			ikSolverRightArm->hint(rightForearmPose.position, rightForearmPose.rotation);
			ikSolverRightArm->DebugPos4 = rightForearmPose.position;
		}


		if (EVAL_TIMING) {
			start = std::chrono::high_resolution_clock::now();
		}


		// Solve Hip
		ikSolverHip->solve(hipPose.position, hipPose.rotation);

		// Solve Spine
		ikSolverSpine->solve(headPose.position, headPose.rotation);

		// Solve LeftLeg
		ikSolverLeftLeg->solve(leftFootPose.position, leftFootPose.rotation);

		// Solve RightLeg
		ikSolverRightLeg->solve(rightFootPose.position, rightFootPose.rotation);

		// Solve LeftArm
		ikSolverLeftArm->solve(leftHandPose.position, leftHandPose.rotation);

		// Solve RightArm
		ikSolverRightArm->solve(rightHandPose.position, rightHandPose.rotation);

		if (EVAL_TIMING) {
			end = std::chrono::high_resolution_clock::now();

			microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

			timingIK.addValue(microseconds);
		}


		if (useTestSkeleton) {

			start = std::chrono::high_resolution_clock::now();

			// Solve Hip
			testIkSolverHip->solve(hipPose.position, hipPose.rotation);

			// Solve Spine
			testIkSolverSpine->solve(headPose.position, headPose.rotation);

			// Solve LeftLeg
			testIkSolverLeftLeg->solve(leftFootPose.position, leftFootPose.rotation);

			// Solve RightLeg
			testIkSolverRightLeg->solve(rightFootPose.position, rightFootPose.rotation);

			// Solve LeftArm
			testIkSolverLeftArm->solve(leftHandPose.position, leftHandPose.rotation);

			// Solve RightArm
			testIkSolverRightArm->solve(rightHandPose.position, rightHandPose.rotation);

			if (EVAL_TIMING) {
				end = std::chrono::high_resolution_clock::now();

				microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

				timingIKTest.addValue(microseconds);
			}
		}
	}

	hierarchicSkeleton->insert();
	if (testHierarchicSkeleton != nullptr) {
		testHierarchicSkeleton->insert();
	}

	auto poses = trackingSystem->Poses;

	skeleton->m_joints = hierarchicSkeleton->skeleton.m_joints;
	if (useTestSkeleton) {
		testSkeleton->m_joints = testHierarchicSkeleton->skeleton.m_joints;
	}
	//hierarchicSkeleton->insert(nullptr);

	if (EVAL_DISTANCE) {

		/*float multi = 1000.0f;

		float footL = multi * distance34(leftFootPose.position, skeleton->m_joints[Joint::FOOT_L].getJointPosition());
		float footR = multi * distance34(rightFootPose.position, skeleton->m_joints[Joint::FOOT_R].getJointPosition());
		float handL = multi * distance34(leftHandPose.position, skeleton->m_joints[Joint::HAND_L].getJointPosition());
		float handR = multi * distance34(rightHandPose.position, skeleton->m_joints[Joint::HAND_R].getJointPosition());
		float head = multi * distance34(headPose.position, skeleton->m_joints[Joint::HEAD].getJointPosition());

		float armL = multi * distance34(leftForearmPose.position, skeleton->m_joints[Joint::FOREARM_L].getJointPosition());
		float armR = multi * distance34(rightForearmPose.position, skeleton->m_joints[Joint::FOREARM_R].getJointPosition());*/

		distanceLeftLeg.addValue(leftLegPose.position, skeleton->m_joints[Joint::LEG_L].getJointPosition());
		distanceRightLeg.addValue(rightLegPose.position, skeleton->m_joints[Joint::LEG_R].getJointPosition());

		distanceLeftFoot.addValue(leftFootPose.position, skeleton->m_joints[Joint::FOOT_L].getJointPosition());
		distanceRightFoot.addValue(rightFootPose.position, skeleton->m_joints[Joint::FOOT_R].getJointPosition());

		distanceLeftArm.addValue(leftForearmPose.position, skeleton->m_joints[Joint::FOREARM_L].getJointPosition());
		distanceRightArm.addValue(rightForearmPose.position, skeleton->m_joints[Joint::FOREARM_R].getJointPosition());

		distanceLeftHand.addValue(leftHandPose.position, skeleton->m_joints[Joint::HAND_L].getJointPosition());
		distanceRightHand.addValue(rightHandPose.position, skeleton->m_joints[Joint::HAND_R].getJointPosition());

		distanceHead.addValue(headPose.position, skeleton->m_joints[Joint::HEAD].getJointPosition());

		if (useTestSkeleton) {
			/*handL = multi * distance34(leftHandPose.position, testSkeleton->m_joints[Joint::HAND_L].getJointPosition());
			handR = multi * distance34(rightHandPose.position, testSkeleton->m_joints[Joint::HAND_R].getJointPosition());

			armL = multi * distance34(leftForearmPose.position, testSkeleton->m_joints[Joint::FOREARM_L].getJointPosition());
			armR = multi * distance34(rightForearmPose.position, testSkeleton->m_joints[Joint::FOREARM_R].getJointPosition());*/

			distanceLeftLegTest.addValue(leftLegPose.position, testSkeleton->m_joints[Joint::LEG_L].getJointPosition());
			distanceRightLegTest.addValue(rightLegPose.position, testSkeleton->m_joints[Joint::LEG_R].getJointPosition());

			distanceLeftFootTest.addValue(leftFootPose.position, testSkeleton->m_joints[Joint::FOOT_L].getJointPosition());
			distanceRightFootTest.addValue(rightFootPose.position, testSkeleton->m_joints[Joint::FOOT_R].getJointPosition());

			distanceLeftArmTest.addValue(leftForearmPose.position, testSkeleton->m_joints[Joint::FOREARM_L].getJointPosition());
			distanceRightArmTest.addValue(rightForearmPose.position, testSkeleton->m_joints[Joint::FOREARM_R].getJointPosition());

			distanceLeftHandTest.addValue(leftHandPose.position, testSkeleton->m_joints[Joint::HAND_L].getJointPosition());
			distanceRightHandTest.addValue(rightHandPose.position, testSkeleton->m_joints[Joint::HAND_R].getJointPosition());
		}
	}


	// Lock point collection mutex
	m_pointCollectionLock.lock();

	int i = 0;
	for (auto& device : trackingSystem->Devices) {
		// Read pose from device index
		auto& pose = poses[i];

		auto joint = config->getJoint(device.index);
		if (joint == Joint::HAND_L || joint == Joint::FOOT_L) {
			pose = trackingSystem->FilteredPoses[i];
		}

		// Convert rotation & update point
		Quaternionf rotation = Quaternionf(-pose.rotation.y(), -pose.rotation.z(), pose.rotation.w(), pose.rotation.x());
		m_pointCollection.updatePoint(device.index, pose.position, rotation);

		i++;
	}

	if (USE_DEBUG_POINT_COLLECTION) {
		m_pointCollection.updatePoint(Joint::SPINE, ikSolverSpine->DebugPos1, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::CHEST, ikSolverSpine->DebugPos2, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::NECK, ikSolverSpine->DebugPos3, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::HEAD, ikSolverSpine->DebugPos4, Quaternionf::Identity());

		m_pointCollection.updatePoint(Joint::UPLEG_L, ikSolverLeftLeg->DebugPos1, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::LEG_L, ikSolverLeftLeg->DebugPos2, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::FOOT_L, ikSolverLeftLeg->DebugPos3, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::TOE_L, ikSolverLeftLeg->DebugPos4, Quaternionf::Identity());

		m_pointCollection.updatePoint(Joint::UPLEG_R, ikSolverRightLeg->DebugPos1, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::LEG_R, ikSolverRightLeg->DebugPos2, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::FOOT_R, ikSolverRightLeg->DebugPos3, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::TOE_R, ikSolverRightLeg->DebugPos4, Quaternionf::Identity());

		m_pointCollection.updatePoint(Joint::SHOULDER_L, ikSolverLeftArm->DebugPos1, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::ARM_L, ikSolverLeftArm->DebugPos2, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::FOREARM_L, ikSolverLeftArm->DebugPos3, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::HAND_L, ikSolverLeftArm->DebugPos4, Quaternionf::Identity());

		m_pointCollection.updatePoint(Joint::SHOULDER_R, ikSolverRightArm->DebugPos1, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::ARM_R, ikSolverRightArm->DebugPos2, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::FOREARM_R, ikSolverRightArm->DebugPos3, Quaternionf::Identity());
		m_pointCollection.updatePoint(Joint::HAND_R, ikSolverRightArm->DebugPos4, Quaternionf::Identity());
	}

	// Unlock point collection mutex
	m_pointCollectionLock.unlock();

	extractSkeleton();

	m_isDataAvailable = true;
}

void OVRTracker::extractSkeleton() {

	m_skeletonPoolLock.lock();

	// Get id from tracker properties
	int id = m_properties->id;

	// Try to find skeleton with id
	auto skeletonIterator = m_skeletonPool.find(id);

	// Skeleton found -> update joints
	if (skeletonIterator != m_skeletonPool.end()) {

		// update all joints of existing skeleon with new data
		skeletonIterator->second.m_joints = skeleton->m_joints;
	}
	// Skeleton not found -> insert new
	else {

		m_skeletonPool.insert({ id, *skeleton });

		//skeleton was added, so UI updates
		m_hasSkeletonPoolChanged = true;

		Console::log("OVRTracker::extractSkeleton(): Created new skeleton with id = " + std::to_string(id));
	}

	if (useTestSkeleton) {
		id = id + 100;

		skeletonIterator = m_skeletonPool.find(id);

		// Skeleton found -> update joints
		if (skeletonIterator != m_skeletonPool.end()) {

			// update all joints of existing skeleon with new data
			skeletonIterator->second.m_joints = testSkeleton->m_joints;
		}
		// Skeleton not found -> insert new
		else {

			m_skeletonPool.insert({ id, *testSkeleton });

			//skeleton was added, so UI updates
			m_hasSkeletonPoolChanged = true;

			Console::log("OVRTracker::extractSkeleton(): Created new skeleton with id = " + std::to_string(id));
		}

	}

	m_skeletonPoolLock.unlock();
}

void OVRTracker::initIKSolvers() {

	// Init Hip IKSolver
	if (ikSolverHip != nullptr) delete ikSolverHip;
	ikSolverHip = new IKSolverHip(&hierarchicSkeleton->hips);
	ikSolverHip->setLeftLeg(&hierarchicSkeleton->leftUpLeg, &hierarchicSkeleton->leftFoot);
	ikSolverHip->setRightLeg(&hierarchicSkeleton->rightUpLeg, &hierarchicSkeleton->rightFoot);
	ikSolverHip->init();

	// Init Spine IKSolver
	if (ikSolverSpine != nullptr) delete ikSolverSpine;
	ikSolverSpine = new IKSolverSpine(&hierarchicSkeleton->spine, &hierarchicSkeleton->chest, &hierarchicSkeleton->neck, &hierarchicSkeleton->head);
	ikSolverSpine->setHip(&hierarchicSkeleton->hips);
	ikSolverSpine->init();

	// Init LeftLeg IKSolver
	if (ikSolverLeftLeg != nullptr) delete ikSolverLeftLeg;
	ikSolverLeftLeg = new IKSolverLeg(&hierarchicSkeleton->leftUpLeg, &hierarchicSkeleton->leftLeg, &hierarchicSkeleton->leftFoot);
	//ikSolverLeftLeg->seToe(&hierarchicSkeleton->leftToe);
	ikSolverLeftLeg->init();

	// Init RightLeg IKSolver
	if (ikSolverRightLeg != nullptr) delete ikSolverRightLeg;
	ikSolverRightLeg = new IKSolverLeg(&hierarchicSkeleton->rightUpLeg, &hierarchicSkeleton->rightLeg, &hierarchicSkeleton->rightFoot);
	//ikSolverRightLeg->seToe(&hierarchicSkeleton->rightToe);
	ikSolverRightLeg->init();

	// Init LeftArm IKSolver
	if (ikSolverLeftArm != nullptr) delete ikSolverLeftArm;
	ikSolverLeftArm = new IKSolverArm(&hierarchicSkeleton->leftShoulder, &hierarchicSkeleton->leftArm, &hierarchicSkeleton->leftForeArm, &hierarchicSkeleton->leftHand);
	ikSolverLeftArm->setChest(&hierarchicSkeleton->chest);
	ikSolverLeftArm->init();

	// Init RightArm IKSolver
	if (ikSolverRightArm != nullptr) delete ikSolverRightArm;
	ikSolverRightArm = new IKSolverArm(&hierarchicSkeleton->rightShoulder, &hierarchicSkeleton->rightArm, &hierarchicSkeleton->rightForeArm, &hierarchicSkeleton->rightHand);
	ikSolverRightArm->setChest(&hierarchicSkeleton->chest);
	ikSolverRightArm->init();

	// Solve in T-Pose
	ikSolverHip->solve(hierarchicSkeleton->hips.getGlobalPosition(), hierarchicSkeleton->hips.getGlobalRotation());
	ikSolverSpine->solve(hierarchicSkeleton->head.getGlobalPosition(), hierarchicSkeleton->head.getGlobalRotation());
	ikSolverLeftLeg->solve(hierarchicSkeleton->leftFoot.getGlobalPosition(), hierarchicSkeleton->leftFoot.getGlobalRotation());
	ikSolverRightLeg->solve(hierarchicSkeleton->rightFoot.getGlobalPosition(), hierarchicSkeleton->rightFoot.getGlobalRotation());
	ikSolverLeftArm->solve(hierarchicSkeleton->leftHand.getGlobalPosition(), hierarchicSkeleton->leftHand.getGlobalRotation());
	ikSolverRightArm->solve(hierarchicSkeleton->rightHand.getGlobalPosition(), hierarchicSkeleton->rightHand.getGlobalRotation());

	if (useTestSkeleton) {
		// Init Hip IKSolver
		if (testIkSolverHip != nullptr) delete testIkSolverHip;
		testIkSolverHip = new IKSolverHip(&testHierarchicSkeleton->hips);
		testIkSolverHip->setLeftLeg(&testHierarchicSkeleton->leftUpLeg, &testHierarchicSkeleton->leftFoot);
		testIkSolverHip->setRightLeg(&testHierarchicSkeleton->rightUpLeg, &testHierarchicSkeleton->rightFoot);
		testIkSolverHip->init();

		// Init Spine IKSolver
		if (testIkSolverSpine != nullptr) delete testIkSolverSpine;
		testIkSolverSpine = new IKSolverSpine(&testHierarchicSkeleton->spine, &testHierarchicSkeleton->chest, &testHierarchicSkeleton->neck, &testHierarchicSkeleton->head);
		testIkSolverSpine->setHip(&testHierarchicSkeleton->hips);
		testIkSolverSpine->init();

		// Init LeftLeg IKSolver
		if (testIkSolverLeftLeg != nullptr) delete testIkSolverLeftLeg;
		testIkSolverLeftLeg = new IKSolverLeg(&testHierarchicSkeleton->leftUpLeg, &testHierarchicSkeleton->leftLeg, &testHierarchicSkeleton->leftFoot);
		testIkSolverLeftLeg->init();

		// Init RightLeg IKSolver
		if (testIkSolverRightLeg != nullptr) delete testIkSolverRightLeg;
		testIkSolverRightLeg = new IKSolverLeg(&testHierarchicSkeleton->rightUpLeg, &testHierarchicSkeleton->rightLeg, &testHierarchicSkeleton->rightFoot);
		testIkSolverRightLeg->init();

		// Init LeftArm IKSolver
		if (testIkSolverLeftArm != nullptr) delete testIkSolverLeftArm;
		testIkSolverLeftArm = new IKSolverArm(&testHierarchicSkeleton->leftShoulder, &testHierarchicSkeleton->leftArm, &testHierarchicSkeleton->leftForeArm, &testHierarchicSkeleton->leftHand);
		testIkSolverLeftArm->setChest(&testHierarchicSkeleton->chest);
		testIkSolverLeftArm->init();

		// Init RightArm IKSolver
		if (testIkSolverRightArm != nullptr) delete testIkSolverRightArm;
		testIkSolverRightArm = new IKSolverArm(&testHierarchicSkeleton->rightShoulder, &testHierarchicSkeleton->rightArm, &testHierarchicSkeleton->rightForeArm, &testHierarchicSkeleton->rightHand);
		testIkSolverRightArm->setChest(&testHierarchicSkeleton->chest);
		testIkSolverRightArm->init();

		// Solve in T-Pose
		testIkSolverHip->solve(testHierarchicSkeleton->hips.getGlobalPosition(), testHierarchicSkeleton->hips.getGlobalRotation());
		testIkSolverSpine->solve(testHierarchicSkeleton->head.getGlobalPosition(), testHierarchicSkeleton->head.getGlobalRotation());
		testIkSolverLeftLeg->solve(testHierarchicSkeleton->leftFoot.getGlobalPosition(), testHierarchicSkeleton->leftFoot.getGlobalRotation());
		testIkSolverRightLeg->solve(testHierarchicSkeleton->rightFoot.getGlobalPosition(), testHierarchicSkeleton->rightFoot.getGlobalRotation());
		testIkSolverLeftArm->solve(testHierarchicSkeleton->leftHand.getGlobalPosition(), testHierarchicSkeleton->leftHand.getGlobalRotation());
		testIkSolverRightArm->solve(testHierarchicSkeleton->rightHand.getGlobalPosition(), testHierarchicSkeleton->rightHand.getGlobalRotation());
	}
}

void OVRTracker::refreshIKSolvers(bool overrideDefault) {

	if (ikSolverHip != nullptr) {
		ikSolverHip->refresh(overrideDefault);
	}

	if (ikSolverLeftLeg != nullptr) {
		ikSolverLeftLeg->refresh(overrideDefault);
	}

	if (ikSolverRightLeg != nullptr) {
		ikSolverRightLeg->refresh(overrideDefault);
	}

	if (ikSolverLeftArm != nullptr) {
		ikSolverLeftArm->refresh(overrideDefault);
	}

	if (ikSolverRightArm != nullptr) {
		ikSolverRightArm->refresh(overrideDefault);
	}

	if (hierarchicSkeleton != nullptr) {
		hierarchicSkeleton->invalidateGlobals();
	}



	if (testIkSolverHip != nullptr) {
		testIkSolverHip->refresh(overrideDefault);
	}

	if (testIkSolverLeftLeg != nullptr) {
		testIkSolverLeftLeg->refresh(overrideDefault);
	}

	if (testIkSolverRightLeg != nullptr) {
		testIkSolverRightLeg->refresh(overrideDefault);
	}

	if (testIkSolverLeftArm != nullptr) {
		testIkSolverLeftArm->refresh(overrideDefault);
	}

	if (testIkSolverRightArm != nullptr) {
		testIkSolverRightArm->refresh(overrideDefault);
	}

	if (testHierarchicSkeleton != nullptr) {
		testHierarchicSkeleton->invalidateGlobals();
	}
}

OpenVRTracking::DevicePose OVRTracker::getAssignedPose(Joint::JointNames joint) {

	// Get the pose from config & tracking system 
	auto pose = config->getPoseWithOffset(joint);

	// TODO: Debug entfernen
	switch (joint) {
	case Joint::HIPS:
		if (DEBUG_HIP) {
			pose.position = Vector3f(0, 1.0f, 0);
			//pose.rotation = Quaternionf::Identity();
		}
		break;
	case Joint::FOREARM_L:
		break;
	case Joint::HAND_L:
		break;
	case Joint::FOREARM_R:
		break;
	case Joint::HAND_R:
		break;
	case Joint::LEG_L:
		break;
	case Joint::FOOT_L:
		break;
	case Joint::LEG_R:
		break;
	case Joint::FOOT_R:
		break;
	case Joint::HEAD:
		if (DEBUG_HMD)
			pose.position = Vector3f(0, 1.6f, 0);
		break;
	}

	return pose;
}

void OVRTracker::calibrate() {

	// Only calibrate if desired
	if (!shouldCalibrate) {
		return;
	}

	// Reset calibration flag
	shouldCalibrate = false;

	calibrateDeviceRoles();

	calibrateDeviceToJointOffsets();

	calibrateScale();

	calibrateHintOffsets();

	refreshIKSolvers();

	Console::log("OVRTracker::calibrate");
}

void OVRTracker::calibrateDeviceRoles() {

	auto calibratedDeviceRoles = trackingSystem->getCalibratedDeviceRoles();

	if (calibratedDeviceRoles.size() > 0) {
		config->clearJointToDevice();

		// Write calibrated DeviceRoles to Config
		for (const auto& deviceJointPair : calibratedDeviceRoles) {

			auto device = trackingSystem->Devices[deviceJointPair.first];
			config->assignJointToDevice(deviceJointPair.second, device.index);
		}

		Console::log("Calibrated Device Roles");
	}
}

void OVRTracker::calibrateDeviceToJointOffsets() {

	config->calibrateDeviceToJointOffsets();

	Console::log("Calibrated Device Joint Offsets");
}

void OVRTracker::calibrateDeviceToJointOffset(Joint::JointNames joint) {

	config->calibrateDeviceToJointOffset(joint);

	Console::log("Calibrated Device Joint Offset: " + Joint::toString(joint));
}

void OVRTracker::calibrateScale() {

	enableCalibrationMode();

	Vector3f oldScale = hierarchicSkeleton->getScale();

	// Reset Skeleton scale & pose
	hierarchicSkeleton->reset();

	// Calculate position between feet
	Vector3f betweenToes = lerp(hierarchicSkeleton->leftToe.getGlobalPosition(), hierarchicSkeleton->rightToe.getGlobalPosition(), 0.5f);

	auto headPose = config->getPoseWithOffset(Joint::HEAD, false);
	auto leftHandPose = config->getPoseWithOffset(Joint::HAND_L, false);
	auto rightHandPose = config->getPoseWithOffset(Joint::HAND_R, false);

	float controllerdistance = distance(leftHandPose.position, rightHandPose.position);
	float beforeScaleDist = distance(hierarchicSkeleton->leftHand.getGlobalPosition(), hierarchicSkeleton->rightHand.getGlobalPosition());

	// Scale Skeleton
	Vector3f scale = config->getCalibratedScale(hierarchicSkeleton);

	if (!SCALE_SKELETON) {
		//scale = Vector3f::Ones();
	}

	// Is scale invalid?
	if (scale.x() < 0.0f || scale.y() < 0.0f) {
		// Revert scaling
		hierarchicSkeleton->setScale(oldScale);
		if (USE_XSENS_SKELETON) {
			hierarchicSkeleton->initXsens();
		}
		else {
			hierarchicSkeleton->init();
		}

		disableCalibrationMode();

		Console::logError("OVRTracker::calibrateScale: Invalid Scale x or y < 0");
		return;
	}

	hierarchicSkeleton->setScale(scale);
	if (USE_XSENS_SKELETON) {
		hierarchicSkeleton->initXsens();
	}
	else {
		hierarchicSkeleton->init();
	}

	refreshIKSolvers(true);

	float afterScaleDist = distance(hierarchicSkeleton->leftHand.getGlobalPosition(), hierarchicSkeleton->rightHand.getGlobalPosition());

	// Calibrate Hip Offset
	config->calibrateDeviceToJointOffset(Joint::HIPS);

	// Solve Hip with identity rotation
	auto hipPose = config->getPoseWithOffset(Joint::HIPS, false);
	ikSolverHip->solve(hipPose.position, Quaternionf::Identity());

	// y diff ermitteln und auf hip offset rechnen?
	Vector3f newBetweenToes = lerp(hierarchicSkeleton->leftToe.getGlobalPosition(), hierarchicSkeleton->rightToe.getGlobalPosition(), 0.5f);

	Vector3f toeOffset = newBetweenToes - betweenToes;

	Console::logError("feetOffset: " + toString(toeOffset));
	Console::logWarning("controllerdistance: " + toString(controllerdistance));
	Console::logWarning("beforeScaleDist: " + toString(beforeScaleDist));
	Console::logWarning("afterScaleDist: " + toString(afterScaleDist));

	// Read current hip offset from config
	Vector3f hipOffset = config->getOffsetPosition(Joint::HIPS);

	Vector3f headToHip = hipPose.position - headPose.position;

	Vector3f hipHeadOffset = projectOnPlane(headToHip, hipPose.rotation * Vector3f(0, 0, 1));

	Vector3f newHipOffset = hipOffset + Vector3f(hipHeadOffset.x(), -toeOffset.y(), 0);
	//Vector3f newHipOffset = hipOffset + Vector3f(0, -toeOffset.y(), 0);
	config->setOffsetPosition(Joint::HIPS, newHipOffset);

	hipPose = config->getPoseWithOffset(Joint::HIPS, false);
	ikSolverHip->solve(hipPose.position, hipPose.rotation);

	Console::log("Hip y Offset: " + toString(toeOffset.y()));

	Console::logError(toString(distance(betweenToes, newBetweenToes)));

	Vector3f leftHandOffset = leftHandPose.position - lerp(leftHandPose.position, rightHandPose.position, 0.25f);
	Vector3f rightHandOffset = rightHandPose.position - lerp(rightHandPose.position, leftHandPose.position, 0.25f);

	ikSolverSpine->solve(headPose.position, headPose.rotation);
	ikSolverLeftArm->solve(leftHandPose.position + leftHandOffset.normalized() * 0.2f, leftHandPose.rotation);
	ikSolverRightArm->solve(rightHandPose.position + rightHandOffset.normalized() * 0.2f, leftHandPose.rotation);

	// Solve LeftFoot & RightFoot lower than Zero to get max stretched distance
	auto leftFootPose = config->getPoseWithOffset(Joint::FOOT_L, false);
	auto rightFootPose = config->getPoseWithOffset(Joint::FOOT_R, false);
	ikSolverLeftLeg->solve(leftFootPose.position + Vector3f(0, -0.5f, 0), leftFootPose.rotation);
	ikSolverRightLeg->solve(rightFootPose.position + Vector3f(0, -0.5f, 0), rightFootPose.rotation);

	// Is LeftFoot tracked? -> Calculate Y Offset
	if (config->isJointAssigned(Joint::FOOT_L)) {
		auto leftFootPose = config->getPoseWithOffset(Joint::FOOT_L, false);
		Vector3f newOffsetL = (hierarchicSkeleton->leftFoot.getGlobalPosition()) - leftFootPose.position;

		Vector3f offsetL = config->getUserOffsetPosition(Joint::FOOT_L);
		offsetL.y() += newOffsetL.y();

		config->setUserOffsetPosition(Joint::FOOT_L, offsetL);
	}

	// Is RightFoot tracked? -> Calculate Y Offset
	if (config->isJointAssigned(Joint::FOOT_R)) {
		auto rightFootPose = config->getPoseWithOffset(Joint::FOOT_R, false);
		Vector3f newOffsetR = (hierarchicSkeleton->leftFoot.getGlobalPosition()) - rightFootPose.position;

		Vector3f offsetR = config->getUserOffsetPosition(Joint::FOOT_R);
		offsetR.y() += newOffsetR.y();

		config->setUserOffsetPosition(Joint::FOOT_R, offsetR);
	}

	float afterSolveDist = distance(hierarchicSkeleton->leftHand.getGlobalPosition(), hierarchicSkeleton->rightHand.getGlobalPosition());
	Console::logWarning("afterSolveDist: " + toString(afterSolveDist));
	Console::logWarning("L -> LC: " + toString(distance(hierarchicSkeleton->leftHand.getGlobalPosition(), leftHandPose.position)));
	Console::logWarning("R -> RC: " + toString(distance(hierarchicSkeleton->rightHand.getGlobalPosition(), rightHandPose.position)));

	float xDelta = afterScaleDist / afterSolveDist;
	scale.x() *= xDelta;
	scale.z() *= xDelta;

	if (!SCALE_SKELETON) {
		//scale = Vector3f::Ones();
	}

	hierarchicSkeleton->setScale(scale);
	if (USE_XSENS_SKELETON) {
		hierarchicSkeleton->initXsens();
	}
	else {
		hierarchicSkeleton->init();
	}

	if (useTestSkeleton) {
		testHierarchicSkeleton->setScale(scale);
		if (USE_XSENS_SKELETON) {
			testHierarchicSkeleton->initXsens();
		}
		else {
			testHierarchicSkeleton->init();
		}
	}

	refreshIKSolvers(true);

	// Write scale to config for Unity process
	config->writeScale(scale);

	disableCalibrationMode();
}

void OVRTracker::calibrateHintOffsets() {

	Joint::JointNames limbJoints[]{
		Joint::LEG_L,
		Joint::LEG_R,
		Joint::FOREARM_L,
		Joint::FOREARM_R
	};

	enableCalibrationMode();

	for (auto limbJoint : limbJoints) {
		int deviceIndex = config->getDeviceIndex(limbJoint);

		// Skip if limb is not assigned
		if (deviceIndex == -1) {
			continue;
		}

		// get joint from hierarchic skeleton
		HierarchicJoint* joint = hierarchicSkeleton->getJoint(limbJoint);

		// get device pose for joint
		auto limbDevicePose = config->getPose(limbJoint);

		// TODO: Kalibrierung der Rotation testen
		/*if (limbJoint == Joint::FOREARM_L) {
			config->calibrateDeviceToJointOffset(limbJoint);
		}
		else {
			Quaternionf offsetRot = limbDevicePose->rotation.inverse() * joint->getGlobalRotation();
			config->setOffsetRotation(limbJoint, offsetRot);
		}*/

		Quaternionf offsetRot = limbDevicePose->rotation.inverse() * joint->getGlobalRotation();
		config->setOffsetRotation(limbJoint, offsetRot);
	}

	disableCalibrationMode();
}

void OVRTracker::notify(Subject* subject) {

	// Cast subject to OpenVR button subject
	OpenVRButtonSubject* ovrButtonSubject = dynamic_cast<OpenVRButtonSubject*>(subject);

	// subject == OpenVRButtonSubject?
	if (ovrButtonSubject != nullptr) {

		switch (ovrButtonSubject->getButtonState()) {

			// Calibrate on trigger pressed
		case vr::EVRButtonId::k_EButton_SteamVR_Trigger:

			//Console::log("k_EButton_SteamVR_Trigger: " + toString(ikSolverHip->DebugBool2));

			calibrate();

			break;

		case vr::EVRButtonId::k_EButton_SteamVR_Touchpad:

			//Console::log("k_EButton_SteamVR_Touchpad: " + toString(ikSolverHip->DebugBool3));

			//StartEvaluating();
			Console::log("Scaling: " + toString(hierarchicSkeleton->getScale()));

			ikSolverLeftArm->DebugBool3 = !ikSolverLeftArm->DebugBool3;
			ikSolverRightArm->DebugBool3 = !ikSolverRightArm->DebugBool3;

			Console::log("k_EButton_SteamVR_Touchpad: " + toString(ikSolverLeftArm->DebugBool3));

			break;

		case vr::EVRButtonId::k_EButton_ApplicationMenu:

			//Console::log("k_EButton_ApplicationMenu: " + toString(ikSolverHip->DebugBool1));

			//StopEvaluating();
			ikSolverLeftArm->DebugBool1 = !ikSolverLeftArm->DebugBool1;
			ikSolverRightArm->DebugBool1 = !ikSolverRightArm->DebugBool1;

			Console::log("k_EButton_ApplicationMenu: " + toString(ikSolverLeftArm->DebugBool1));

			break;
		}
	}
}

std::string OVRTracker::getTrackerType() {

	return "OpenVR";
}