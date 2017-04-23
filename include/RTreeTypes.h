#ifndef RTREETYPES_H_
#define RTREETYPES_H_

#include <tuple>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/uuid/uuid.hpp>

//----------------------------------------------------------------------------------------------------------------------
/// @file RTreeTypes.h
/// @brief this includes type definitions needed for boost geometry
/// @author Neerav Nagda
/// @date 19/03/17
//----------------------------------------------------------------------------------------------------------------------

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

//Define a point type as 3 floating point values in a cartesian coordinate system
using point_t = bg::model::point<float, 3, bg::cs::cartesian>;

//Define segment and box to use the point type for its construction
using segment_t = bg::model::segment<point_t>;
using box_t = bg::model::box<point_t>;

//Define the tuple for values to add to the R-tree
//The first is the geometry type
//The second is the ID of the branch
//The third is the node within the branch
using rTreeElement = std::tuple<segment_t, boost::uuids::uuid, unsigned>;

//Define a R-tree with segments
using rTree_t = bgi::rtree<rTreeElement, bgi::quadratic<16> >;

#endif // RTREETYPES_H_
