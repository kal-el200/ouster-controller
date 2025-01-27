// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// Disable warning LNK4006: symbol already defined
#pragma warning(disable : 4006)

#include <Eigen/Dense>
#include <ouster_configuration.hpp>
#include <ouster/point_viz.h>
#include <ouster/types.h>
#include <ouster/sensor_client.h>
#include <ouster/lidar_scan.h>
#include <ouster/sensor_scan_source.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <random>

#endif //PCH_H
