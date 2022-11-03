#include "BezierCurve.h"

void BezierCurve::generate()
{
    curve_points.clear();
    curve_points.push_back(start);
    // get absolute position
    Eigen::Vector3d r_start_control = start + start_control;
    Eigen::Vector3d r_end = start + end;
    Eigen::Vector3d r_end_control = r_end + end_control;
    for (unsigned int i = 1; i < piecewise_size - 1; i ++) {
        double t = static_cast<double>(i) / piecewise_size;
        double t_2 = t * t, t_3 = t_2 * t;
        // t M
        double a = - 1 * t_3 + 3 * t_2 - 3 * t + 1;
        double b =   3 * t_3 - 6 * t_2 + 3 * t;
        double c = - 3 * t_3 + 3 * t_2;
        double d =   1 * t_3;
        // t M B
        double x = a * start.x() + b * r_start_control.x() + c * r_end_control.x() + d * r_end.x();
        double y = a * start.y() + b * r_start_control.y() + c * r_end_control.y() + d * r_end.y();
        double z = a * start.z() + b * r_start_control.z() + c * r_end_control.z() + d * r_end.z();
        curve_points.push_back(Eigen::Vector3d(x, y, z));
    }
    curve_points.push_back(r_end);
    generated = true;
}
