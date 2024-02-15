#ifndef ERL_CONTROL_VERTICAL_CIRCLE_HPP
#define ERL_CONTROL_VERTICAL_CIRCLE_HPP

#include <ros/ros.h>
#include "command.hpp"
#include "math.h"

#include "../PX4_quadcoptor.hpp"

#include <geometry_msgs/Pose.h>
#include <Eigen/Geometry>
#include <sophus/se3.hpp>

namespace ERLControl{

    class VerticalCircleCommand : public Command {
        public:
        VerticalCircleCommand(Eigen::Vector3d aCenter, double aRadius, double anAngularVelocity, double ayaw, double aTime) :
         center(aCenter), radius(aRadius), angularVelocity(anAngularVelocity), yaw(ayaw), time(aTime) {}

        void initialize() override {
            start_time = ros::Time::now();
            omega = angularVelocity;

            start_pose = quad->getLocalPose(); // used to get target orientation
        }

        void execute() override {
            auto t = (ros::Time::now() - start_time).toSec();
            double wt = omega * t;

            Eigen::Vector3d target_position, target_velocity, target_acceleration, target_jerk;
            double yaw_target = yaw;
            target_position.x() = center.x() + 0.0;;
            target_position.y() = center.y() + radius * sin(wt);
            target_position.z() = center.z() + radius * cos(wt);
            target_velocity.x() = 0.0;
            target_velocity.y() = radius * omega * cos(wt);
            target_velocity.z() = -radius * omega * sin(wt);
            target_acceleration.x() = 0.0;
            target_acceleration.y() = -radius * pow(omega,2) * sin(wt);
            target_acceleration.z() = -radius * pow(omega,2) * cos(wt);
            target_jerk.x() = 0.0; // Neglect
            target_jerk.y() = 0.0; // Neglect
            target_jerk.z() = 0.0; // Neglect
            
            
//            Sophus::SE3d currentTargetPose;
//            currentTargetPose.translation() = target_position;
//            currentTargetPose.setQuaternion(start_pose.unit_quaternion());

            //quad->setTargetPose(currentTargetPose);
            quad->setTargetState(target_position, target_velocity, target_acceleration, target_jerk, yaw_target);
        }

        bool isFinished() override {
            return (ros::Time::now() - start_time) > ros::Duration(time);
        };

        std::string getName() override {
            return "vertical_circle";
        }

        private:
        Eigen::Vector3d         center;
        double                  radius;
        double                  angularVelocity;
        double                  yaw;
        double                  time;

        double                  omega = 0;

        ros::Time               start_time;
        Sophus::SE3d            start_pose;
    };

}

#endif