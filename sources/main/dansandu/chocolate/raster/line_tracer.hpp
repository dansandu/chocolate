#pragma once

#include "dansandu/chocolate/common.hpp"

namespace dansandu::chocolate::raster::line_tracer
{

class LineTracer
{
public:
    LineTracer(const ConstantPoint2View a, const ConstantPoint2View b);

    bool step();

    Point2 position() const;

private:
    int width_;
    int height_;
    int error_;
    int x_;
    int y_;
    int dx_;
    int dy_;
    int end_;
};

}
