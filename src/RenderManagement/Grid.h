// ClayMore - Immersive Mesh Modelling --- Copyright (c) 2014-2017 Philipp Ladwig, Jannik Fiedler, Jan Beutgen
#pragma once

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>

enum GridMode
{
	SHOW_ALL,
	SHOW_X_Z
};


class Grid
{
	// #### CONSTRUCTOR & DESTRUCTOR ###############
	public:
		Grid(unsigned short lineCount = 5, GridMode mode = SHOW_ALL, float scaleIn = 1.0f, float lineWidthInRGBAxis = 5.0f, float lineWidthGreyAxis = 3.0f);
		~Grid();

	// #### PUBLIC METHODS ###############
		void initialize(int lineCount, float lineWidthInRGBAxis, float lineWidthGreyAxis);
		inline void attachToSceneGraph(osg::ref_ptr<osg::Group> pRootNode) { pRootNode->addChild(m_matrixTransform); }


	// #### MEMBER VARIABLES ###############
	private:
		GridMode m_mode;

		osg::ref_ptr<osg::MatrixTransform> m_matrixTransform;
		osg::ref_ptr<osg::Geometry> m_geometry;
		osg::ref_ptr<osg::Geometry> m_geometryAxes;

};