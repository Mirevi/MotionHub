#include "GeodeFinder.h"
#include <iostream>

// This method gets called for every child of the given node. If it is a Geode, save the address to a member.
void GeodeFinder::apply(osg::Node& node)
{

	osg::Geode* geode = dynamic_cast<osg::Geode*>(&node);
	if (geode) {
		m_geode = geode;
	}

	// Traverse the rest
	traverse(node);

}


osg::ref_ptr<osg::Geode> GeodeFinder::getFoundGeode()
{

	return m_geode;

}


