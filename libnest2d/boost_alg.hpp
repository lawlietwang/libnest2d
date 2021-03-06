#ifndef BOOST_ALG_HPP
#define BOOST_ALG_HPP

#ifndef DISABLE_BOOST_SERIALIZE
    #include <sstream>
#endif

#include <boost/geometry.hpp>

// this should be removed to not confuse the compiler
// #include <libnest2d.h>

namespace bp2d {

using libnest2d::TCoord;
using libnest2d::PointImpl;
using Coord = TCoord<PointImpl>;
using libnest2d::PolygonImpl;
using libnest2d::PathImpl;
using libnest2d::Orientation;
using libnest2d::OrientationType;
using libnest2d::getX;
using libnest2d::getY;
using libnest2d::setX;
using libnest2d::setY;
using Box = libnest2d::_Box<PointImpl>;
using Segment = libnest2d::_Segment<PointImpl>;

}

/**
 * We have to make all the binpack2d geometry types available to boost. The real
 * models of the geometries remain the same if a conforming model for binpack2d
 * was defined by the library client. Boost is used only as an optional
 * implementer of some algorithms that can be implemented by the model itself
 * if a faster alternative exists.
 *
 * However, boost has its own type traits and we have to define the needed
 * specializations to be able to use boost::geometry. This can be done with the
 * already provided model.
 */
namespace boost {
namespace geometry {
namespace traits {

/* ************************************************************************** */
/* Point concept adaptaion ************************************************** */
/* ************************************************************************** */

template<> struct tag<bp2d::PointImpl> {
    using type = point_tag;
};

template<> struct coordinate_type<bp2d::PointImpl> {
    using type = bp2d::Coord;
};

template<> struct coordinate_system<bp2d::PointImpl> {
    using type = cs::cartesian;
};

template<> struct dimension<bp2d::PointImpl>: boost::mpl::int_<2> {};

template<>
struct access<bp2d::PointImpl, 0 > {
    static inline bp2d::Coord get(bp2d::PointImpl const& a) {
        return libnest2d::getX(a);
    }

    static inline void set(bp2d::PointImpl& a,
                           bp2d::Coord const& value) {
        libnest2d::setX(a, value);
    }
};

template<>
struct access<bp2d::PointImpl, 1 > {
    static inline bp2d::Coord get(bp2d::PointImpl const& a) {
        return libnest2d::getY(a);
    }

    static inline void set(bp2d::PointImpl& a,
                           bp2d::Coord const& value) {
        libnest2d::setY(a, value);
    }
};


/* ************************************************************************** */
/* Box concept adaptaion **************************************************** */
/* ************************************************************************** */

template<> struct tag<bp2d::Box> {
    using type = box_tag;
};

template<> struct point_type<bp2d::Box> {
    using type = bp2d::PointImpl;
};

template<> struct indexed_access<bp2d::Box, min_corner, 0> {
    static inline bp2d::Coord get(bp2d::Box const& box) {
        return bp2d::getX(box.minCorner());
    }
    static inline void set(bp2d::Box &box, bp2d::Coord const& coord) {
        bp2d::setX(box.minCorner(), coord);
    }
};

template<> struct indexed_access<bp2d::Box, min_corner, 1> {
    static inline bp2d::Coord get(bp2d::Box const& box) {
        return bp2d::getY(box.minCorner());
    }
    static inline void set(bp2d::Box &box, bp2d::Coord const& coord) {
        bp2d::setY(box.minCorner(), coord);
    }
};

template<> struct indexed_access<bp2d::Box, max_corner, 0> {
    static inline bp2d::Coord get(bp2d::Box const& box) {
        return bp2d::getX(box.maxCorner());
    }
    static inline void set(bp2d::Box &box, bp2d::Coord const& coord) {
        bp2d::setX(box.maxCorner(), coord);
    }
};

template<> struct indexed_access<bp2d::Box, max_corner, 1> {
    static inline bp2d::Coord get(bp2d::Box const& box) {
        return bp2d::getY(box.maxCorner());
    }
    static inline void set(bp2d::Box &box, bp2d::Coord const& coord) {
        bp2d::setY(box.maxCorner(), coord);
    }
};

/* ************************************************************************** */
/* Segement concept adaptaion *********************************************** */
/* ************************************************************************** */

template<> struct tag<bp2d::Segment> {
    using type = segment_tag;
};

template<> struct point_type<bp2d::Segment> {
    using type = bp2d::PointImpl;
};

template<> struct indexed_access<bp2d::Segment, 0, 0> {
    static inline bp2d::Coord get(bp2d::Segment const& seg) {
        return bp2d::getX(seg.first());
    }
    static inline void set(bp2d::Segment &seg, bp2d::Coord const& coord) {
        bp2d::setX(seg.first(), coord);
    }
};

template<> struct indexed_access<bp2d::Segment, 0, 1> {
    static inline bp2d::Coord get(bp2d::Segment const& seg) {
        return bp2d::getY(seg.first());
    }
    static inline void set(bp2d::Segment &seg, bp2d::Coord const& coord) {
        bp2d::setY(seg.first(), coord);
    }
};

template<> struct indexed_access<bp2d::Segment, 1, 0> {
    static inline bp2d::Coord get(bp2d::Segment const& seg) {
        return bp2d::getX(seg.second());
    }
    static inline void set(bp2d::Segment &seg, bp2d::Coord const& coord) {
        bp2d::setX(seg.second(), coord);
    }
};

template<> struct indexed_access<bp2d::Segment, 1, 1> {
    static inline bp2d::Coord get(bp2d::Segment const& seg) {
        return bp2d::getY(seg.second());
    }
    static inline void set(bp2d::Segment &seg, bp2d::Coord const& coord) {
        bp2d::setY(seg.second(), coord);
    }
};


/* ************************************************************************** */
/* Polygon concept adaptaion ************************************************ */
/* ************************************************************************** */

// Connversion between binpack2d::Orientation and order_selector ///////////////

template<bp2d::Orientation> struct ToBoostOrienation {};

template<>
struct ToBoostOrienation<bp2d::Orientation::CLOCKWISE> {
    static const order_selector Value = clockwise;
};

template<>
struct ToBoostOrienation<bp2d::Orientation::COUNTER_CLOCKWISE> {
    static const order_selector Value = counterclockwise;
};

static const bp2d::Orientation RealOrientation =
        bp2d::OrientationType<bp2d::PolygonImpl>::Value;

// Ring implementation /////////////////////////////////////////////////////////

// Boost would refer to ClipperLib::Path (alias bp2d::PolygonImpl) as a ring
template<> struct tag<bp2d::PathImpl> {
    using type = ring_tag;
};

template<> struct point_order<bp2d::PathImpl> {
    static const order_selector value =
            ToBoostOrienation<RealOrientation>::Value;
};

// All our Paths should be closed for the bin packing application
template<> struct closure<bp2d::PathImpl> {
    static const closure_selector value = closed;
};

// Polygon implementation //////////////////////////////////////////////////////

template<> struct tag<bp2d::PolygonImpl> {
    using type = polygon_tag;
};

template<> struct exterior_ring<bp2d::PolygonImpl> {
    static inline bp2d::PathImpl& get(bp2d::PolygonImpl& p) {
        return libnest2d::ShapeLike::getContour(p);
    }

    static inline bp2d::PathImpl const& get(bp2d::PolygonImpl const& p) {
        return libnest2d::ShapeLike::getContour(p);
    }
};

template<> struct ring_const_type<bp2d::PolygonImpl> {
    using type = const bp2d::PathImpl&;
};

template<> struct ring_mutable_type<bp2d::PolygonImpl> {
    using type = bp2d::PathImpl&;
};

template<> struct interior_const_type<bp2d::PolygonImpl> {
   using type = const libnest2d::THolesContainer<bp2d::PolygonImpl>&;
};

template<> struct interior_mutable_type<bp2d::PolygonImpl> {
   using type = libnest2d::THolesContainer<bp2d::PolygonImpl>&;
};

template<>
struct interior_rings<bp2d::PolygonImpl> {

    static inline libnest2d::THolesContainer<bp2d::PolygonImpl>& get(
            bp2d::PolygonImpl& p)
    {
        return libnest2d::ShapeLike::holes(p);
    }

    static inline const libnest2d::THolesContainer<bp2d::PolygonImpl>& get(
            bp2d::PolygonImpl const& p)
    {
        return libnest2d::ShapeLike::holes(p);
    }
};

}   // traits
}   // geometry

// This is an addition to the ring implementation
template<>
struct range_value<bp2d::PathImpl> {
    using type = bp2d::PointImpl;
};

}   // boost

namespace libnest2d { // Now the algorithms that boost can provide...

template<>
inline double PointLike::distance(const PointImpl& p1,
                                  const PointImpl& p2 )
{
    return boost::geometry::distance(p1, p2);
}

template<>
inline double PointLike::distance(const PointImpl& p,
                                  const bp2d::Segment& seg )
{
    return boost::geometry::distance(p, seg);
}

// Tell binpack2d how to make string out of a ClipperPolygon object
template<>
inline bool ShapeLike::intersects(const PathImpl& sh1,
                                  const PathImpl& sh2)
{
    return boost::geometry::intersects(sh1, sh2);
}

// Tell binpack2d how to make string out of a ClipperPolygon object
template<>
inline bool ShapeLike::intersects(const PolygonImpl& sh1,
                                  const PolygonImpl& sh2) {
    return boost::geometry::intersects(sh1, sh2);
}

// Tell binpack2d how to make string out of a ClipperPolygon object
template<>
inline bool ShapeLike::intersects(const bp2d::Segment& s1,
                                  const bp2d::Segment& s2) {
    return boost::geometry::intersects(s1, s2);
}

#ifndef DISABLE_BOOST_AREA
template<>
inline double ShapeLike::area(const PolygonImpl& shape) {
    return boost::geometry::area(shape);
}
#endif

template<>
inline bool ShapeLike::isInside(const PointImpl& point,
                                const PolygonImpl& shape)
{
    return boost::geometry::within(point, shape);
}

template<>
inline bool ShapeLike::isInside(const PolygonImpl& sh1,
                                const PolygonImpl& sh2)
{
    return boost::geometry::within(sh1, sh2);
}

template<>
inline bool ShapeLike::touches( const PolygonImpl& sh1,
                                const PolygonImpl& sh2)
{
    return boost::geometry::touches(sh1, sh2);
}

template<>
inline bp2d::Box ShapeLike::boundingBox(const PolygonImpl& sh) {
    bp2d::Box b;
    boost::geometry::envelope(sh, b);
    return b;
}

template<>
inline void ShapeLike::rotate(PolygonImpl& sh, const Radians& rads) {
    namespace trans = boost::geometry::strategy::transform;

    PolygonImpl cpy = sh;

    trans::rotate_transformer<boost::geometry::radian, Radians, 2, 2>
            rotate(rads);
    boost::geometry::transform(cpy, sh, rotate);
}

template<>
inline void ShapeLike::translate(PolygonImpl& sh, const PointImpl& offs) {
    namespace trans = boost::geometry::strategy::transform;

    PolygonImpl cpy = sh;
    trans::translate_transformer<bp2d::Coord, 2, 2> translate(
                bp2d::getX(offs), bp2d::getY(offs));

    boost::geometry::transform(cpy, sh, translate);
}

#ifndef DISABLE_BOOST_OFFSET
template<>
inline void ShapeLike::offset(PolygonImpl& sh, bp2d::Coord distance) {
    PolygonImpl cpy = sh;
    boost::geometry::buffer(cpy, sh, distance);
}
#endif

#ifndef DISABLE_BOOST_MINKOWSKI_ADD
template<>
inline PolygonImpl& Nfp::minkowskiAdd(PolygonImpl& sh,
                                      const PolygonImpl& /*other*/) {
    return sh;
}
#endif

#ifndef DISABLE_BOOST_SERIALIZE
template<>
inline std::string ShapeLike::serialize<libnest2d::Formats::SVG>(
        const PolygonImpl& sh)
{

    std::stringstream ss;
    std::string style = "fill: orange; stroke: black; stroke-width: 1px;";
    auto svg_data = boost::geometry::svg(sh, style);

    ss << svg_data << std::endl;

    return ss.str();
}
#endif

#ifndef DISABLE_BOOST_UNSERIALIZE
template<>
inline void ShapeLike::unserialize<libnest2d::Formats::SVG>(
        PolygonImpl& sh,
        const std::string& str)
{
}
#endif

template<> inline std::pair<bool, std::string>
ShapeLike::isValid(const PolygonImpl& sh) {
    std::string message;
    bool ret = boost::geometry::is_valid(sh, message);

    return {ret, message};
}

}



#endif // BOOST_ALG_HPP
