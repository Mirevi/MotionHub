#pragma once
#include "ConfigDllExportRenderManagement.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/Skeleton.h"
#include <osg/Group>
#include <osg/PositionAttitudeTransform>


//TODO: Rename to osgSkeleton, when all branches merged again
class OsgLine;
class OsgBone;

//TODO Derive from PostAtt or Group?
class RenderManagement_DLL_import_export OsgSkeleton
{

public:
	/*!
	 *
	 * default constructor
	 *
	 */
	OsgSkeleton(osg::ref_ptr<osg::Group> parentNode);
	~OsgSkeleton();
	
	void update(Skeleton skeleton);
	void removeAndDelete();


	/*!
	 *
	 * joint mesh array representing all 21 skeleton joints
	 *
	 */
private:
	osg::ref_ptr<osg::Group> m_parentNode;
	osg::ref_ptr<osg::Group> m_skeletonRootNode;
	std::vector<osg::ref_ptr<osg::PositionAttitudeTransform>> m_joints;
	std::vector<OsgBone*> m_bones;

	////juj löschen:
	//OsgLine* m_line;

	//osg::PositionAttitudeTransform* m_test;


};


// HWM: Skeleton and Bone Geschichte:
// Das Problem aktuell ist die saubere Kommunikation zwischen TrackerManagement und dem RenderManagement über entsprechende (generische?) Objekte.
// Zudem sollte es einigermaßen API-Nutzerfreundlich sein
// Es gibt jetzt ein OSGSkeleton, ein Skeleton(MMHUtils), ein OSGBone, und ein Joint (MMHUtils) ... die Klassen ohne OSG als Prefix werden in TrackerManagement
// und RenderManagement genutzt... jetzt ist die Überlegung den anderen Klassen auch OSG "aufzuziehen"?? Ich halte von dem ganzen "super" generischen Ansatz
// so oder so nicht viel und ich denke, es geht sehr auf die API-freundlichkeit des MMHs... daher sollte ich OSG ruihig überall durchziehen...nur ist die Frage, ob Utils dann
// die richtige Herberge für Skeleton und Joint ist?? Werden die noch von anderen Dlls genutzt?
// 