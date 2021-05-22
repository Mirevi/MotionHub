#pragma once
#include "ConfigDllExportRenderManagement.h"
#include "osg/PositionAttitudeTransform"

//class ConfigManager;

class RenderManagement_DLL_import_export OsgAxesCross : public osg::PositionAttitudeTransform
{
public:
	OsgAxesCross(osg::ref_ptr<osg::Group> nodeToAttachTo);
    ~OsgAxesCross();

    void createGeometry();

	void setScale(int scale);
	int getScale();


	void setHeightX(float height);
	float getHeightX();

	void setHeightY(float height);
	float getHeightY();

	void setHeightZ(float height);
	float getHeightZ();


	void setThicknessX(float thickness);
	float getThicknessX();

	void setThicknessY(float thickness);
	float getThicknessY();

	void setThicknessZ(float thickness);
	float getThicknessZ();

private:
	float m_scale;
	float m_heightX;
	float m_heightY;
	float m_heightZ;
	float m_thicknessX;
	float m_thicknessY;
	float m_thicknessZ;

};