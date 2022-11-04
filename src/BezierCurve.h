#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include "Eigen/Dense"
#include <functional>
#include <vector>

class BezierCurve
{
private:
    unsigned int piecewise_size;
    Eigen::Vector3d start;
    Eigen::Vector3d end;
    Eigen::Vector3d start_control;
    Eigen::Vector3d end_control;
    std::vector<Eigen::Vector3d> curve_points;
    bool generated;
    void generate();
    bool enabled;
public:

    BezierCurve(unsigned int size):piecewise_size(size < 2 ? 2 : size), generated(false), enabled(false){
        start = Eigen::Vector3d(0, 0, 0);
        end = Eigen::Vector3d(0, 0, 0);
        start_control = Eigen::Vector3d(0, 0, 0);
        end_control = Eigen::Vector3d(0, 0, 0);
    }

    Eigen::Vector3d getPointAt(unsigned int index) {
        if (!generated) {
            generate();
        }
        return index < piecewise_size ? curve_points[index] : Eigen::Vector3d(0, 0, 0);
    }

    Eigen::Vector3d getLast() {
        if (!generated) {
            generate();
        }
        return curve_points[piecewise_size - 1];
    }

    void setPieceSize(unsigned int size) {
        piecewise_size = size;
        generated = false;
    }

    void setStart(double x, double y, double z) {
        start.x() = x;
        start.y() = y;
        start.z() = z;
        generated = false;
    }
    void setEnd(double x, double y, double z) {
        end.x() = x;
        end.y() = y;
        end.z() = z;
        generated = false;
    }
    void setStartControl(double x, double y, double z) {
        start_control.x() = x;
        start_control.y() = y;
        start_control.z() = z;
        generated = false;
    }
    void setEndControl(double x, double y, double z) {
        end_control.x() = x;
        end_control.y() = y;
        end_control.z() = z;
        generated = false;
    }

    void reduce(std::function<void(Eigen::Vector3d prev, Eigen::Vector3d cur)> callback)
    {
        if (!generated) {
            generate();
        }
        for (unsigned int i = 1; i < piecewise_size; i ++) {
            callback(curve_points[i - 1], curve_points[i]);
        }
    }

    void forEach(std::function<void(Eigen::Vector3d prev, unsigned int i, bool &stop)> callback)
    {
        if (!generated) {
            generate();
        }
        bool stop = false;
        for (unsigned int i = 0; i < piecewise_size; i ++) {
            callback(curve_points[i], i, stop);
            if (stop) {
                return;
            }
        }
    }

    void setEnabled(bool value)
    {
        enabled = value;
        if (!enabled) {
            setStart(0, 0, 0);
            setEnd(0, 0, 0);
            setStartControl(0, 0, 0);
            setEndControl(0, 0, 0);
        }
    }

    bool getEnabled() {return enabled;};
};

#endif // BEZIERCURVE_H
