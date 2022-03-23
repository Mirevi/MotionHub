#pragma push_macro("slots") // otherwise python slots crashes with qt slots
#undef slots
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#pragma pop_macro("slots")