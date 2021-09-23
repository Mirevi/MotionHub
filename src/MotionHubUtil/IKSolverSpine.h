#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "IKSolver.h"

/*!
 * \class IKSolverSpine
 *
 * \brief Extends IKSolver and implements a IK Solver to solve chains from Hip to Head
 */
class MotionHubUtil_DLL_import_export IKSolverSpine : public IKSolver {

public:

	/*!
	 * Default Constructor
	 */
	IKSolverSpine();

	/*!
	 * Constructor with given spine, chest, neck and head joints
	 *
	 * \param spine the spine joint
	 * \param chest the chest joint
	 * \param neck the neck joint
	 * \param head the head joint
	 */
	IKSolverSpine(HierarchicJoint* spine, HierarchicJoint* chest, HierarchicJoint* neck, HierarchicJoint* head);

	/*!
	 * Stores a reference to hip joint
	 */
	void setHip(HierarchicJoint* joint);

	/*!
	 * Initializes the IKSolver and stores default values
	 */
	virtual void init() override;

	/*!
	 * Solves current chain to a given position & rotation
	 * 
	 * \param position the target position
	 * \param rotation the target rotation
	 */
	virtual void solve(Vector3f position, Quaternionf rotation) override;

protected:

	/*!
	 * Saves default joint positions and rotations
	 */
	virtual void saveDefaultState() override;

	/*!
	 * Restores default joint positions and rotations
	 */
	virtual void loadDefaultState() override;

	/*!
	 * Bends Joints towards head position
	 */
	void solveCCD();

	/*!
	 * Twists Joints towards head rotation
	 */
	void solveTwist();

protected:

	IKJoint spineJoint;
	IKJoint chestJoint;
	IKJoint neckJoint;
	IKJoint headJoint;

	HierarchicJoint* hip;

	Quaternionf headRelativeToHip;

	std::vector<Quaternionf> defaultRotations;

	std::vector<IKJoint*> twistJoints;
	std::vector<float> twistWeights;

	float ccdSlerpX = 1.0f;
	float ccdSlerpZ = 0.9f;

	float twistSlerpY = 0.5f;

	std::vector<IKJoint*> ccdJoints;

	std::vector<Quaternionf> ccdDefaultRotations;

	std::vector<float> ccdBendDelta;

	bool onlyXZCCD = true;
	bool lerpTest = false;
};
