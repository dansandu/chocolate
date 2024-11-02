#include "dansandu/chocolate/raster/line_tracer.hpp"

namespace dansandu::chocolate::raster::line_tracer
{

LineTracer::LineTracer(const ConstantPoint2View a, const ConstantPoint2View b)
{
    x_ = a.x();
    y_ = a.y();

    const auto increasingX = a.x() <= b.x();
    const auto increasingY = a.y() <= b.y();

    width_ = b.x() - a.x();
    width_ = increasingX * width_ - !increasingX * width_;

    height_ = b.y() - a.y();
    height_ = increasingY * height_ - !increasingY * height_;

    dx_ = increasingX - !increasingX;
    dy_ = increasingY - !increasingY;

    const auto alongX = height_ <= width_;

    error_ = height_ - width_ + alongX * height_ - !alongX * width_;

    end_ = alongX * b.x() + !alongX * b.y();
}

bool LineTracer::step()
{
    const auto alongX = width_ >= height_;
    const auto stop = (alongX & (x_ == end_)) | ((!alongX) & (y_ == end_));
    const auto incrementX = (!stop) & (alongX | (error_ < 0));
    const auto incrementY = (!stop) & ((!alongX) | (error_ > 0));

    error_ += 2 * incrementX * height_ - 2 * incrementY * width_;
    x_ += incrementX * dx_;
    y_ += incrementY * dy_;
    return !stop;
}

Point2 LineTracer::position() const
{
    return Point2{{x_, y_}};
}

}
