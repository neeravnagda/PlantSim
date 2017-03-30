#ifndef RTREETYPES_H_
#define RTREETYPES_H_

#include <boost/tuple/tuple.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

/// @file RTreeTypes.h
/// @brief this includes type definitions needed for boost geometry
/// @author Neerav Nagda
/// @date 19/03/17

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

//Define a point type as 3 floating point values in a cartesian coordinate system
typedef bg::model::point<float, 3, bg::cs::cartesian> point_t;

//Define segment and box to use the point type for its construction
typedef bg::model::segment<point_t> segment_t;
typedef bg::model::box<point_t> box_t;

//Define the tuple for values to add to the R-tree
//The first is the geometry type
//The second and third are IDs of the plant and branch
typedef std::tuple<box_t,unsigned,unsigned> rTreeElement;

//Define the R-tree to store rTreeElement values with a quadratic algorithm
typedef bgi::rtree<rTreeElement, bgi::quadratic<16> > rTree_t;

#endif // RTREETYPES_H_
