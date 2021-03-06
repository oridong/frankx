#pragma once

#include <franka/duration.h>
#include <franka/robot_state.h>

#include <movex/otg/parameter.hpp>
#include <movex/robot/motion_data.hpp>
#include <movex/robot/robot_state.hpp>
#include <movex/waypoint.hpp>


namespace frankx {
    using namespace movex;

struct MotionGenerator {
    static inline franka::CartesianPose CartesianPose(const Vector7d& vector, bool include_elbow = true) {
        auto affine = Affine(vector);
        if (include_elbow) {
            return franka::CartesianPose(affine.array(), {vector(6), -1});
        }
        return franka::CartesianPose(affine.array());
    }

    template <class T = double>
    static inline std::array<T, 7> VectorCartRotElbow(T cart, T rot, T elbow) {
        return {cart, cart, cart, rot, rot, rot, elbow};
    }

    static inline void setCartRotElbowVector(Vector7d& vector, double cart, double rot, double elbow) {
        const std::array<double, 7> data = VectorCartRotElbow(cart, rot, elbow);
        vector = Eigen::Map<const Vector7d>(data.data(), data.size());
    }

    static inline movex::RobotState<7> convertState(const franka::RobotState& franka) {
        movex::RobotState<7> movex;
        movex.q = franka.q;
        movex.q_d = franka.q_d;
        movex.dq = franka.dq;

        movex.O_T_EE = franka.O_T_EE;
        movex.O_T_EE_c = franka.O_T_EE_c;
        movex.O_dP_EE_c = franka.O_dP_EE_c;

        movex.elbow = franka.elbow;
        movex.elbow_c = franka.elbow_c;
        movex.elbow_d = franka.elbow_d;

        movex.O_F_ext_hat_K = franka.O_F_ext_hat_K;
        return movex;
    }

    template<class RobotType>
    static std::tuple<std::array<double, 7>, std::array<double, 7>, std::array<double, 7>> getInputLimits(RobotType* robot, const MotionData& data) {
        return getInputLimits(robot, Waypoint(), data);
    }

    template<class RobotType>
    static std::tuple<std::array<double, 7>, std::array<double, 7>, std::array<double, 7>> getInputLimits(RobotType* robot, const Waypoint& waypoint, const MotionData& data) {
        constexpr double translation_factor {0.5};
        constexpr double elbow_factor {0.32};
        constexpr double derivative_factor {0.4};

        if (waypoint.max_dynamics || data.max_dynamics) {
            auto max_velocity = MotionGenerator::VectorCartRotElbow(
                0.8 * translation_factor * robot->max_translation_velocity,
                robot->max_rotation_velocity,
                0.5 * elbow_factor * robot->max_elbow_velocity
            );
            auto max_acceleration = MotionGenerator::VectorCartRotElbow(
                translation_factor * derivative_factor * robot->max_translation_acceleration,
                derivative_factor * robot->max_rotation_acceleration,
                elbow_factor * derivative_factor * robot->max_elbow_acceleration
            );
            auto max_jerk = MotionGenerator::VectorCartRotElbow(
                translation_factor * derivative_factor * robot->max_translation_jerk,
                derivative_factor * robot->max_rotation_jerk,
                elbow_factor * derivative_factor * robot->max_elbow_jerk
            );
            return {max_velocity, max_acceleration, max_jerk};
        }

        auto max_velocity = MotionGenerator::VectorCartRotElbow(
            translation_factor * waypoint.velocity_rel * data.velocity_rel * robot->velocity_rel * robot->max_translation_velocity,
            waypoint.velocity_rel * data.velocity_rel * robot->velocity_rel * robot->max_rotation_velocity,
            elbow_factor * waypoint.velocity_rel * data.velocity_rel * robot->velocity_rel * robot->max_elbow_velocity
        );
        auto max_acceleration = MotionGenerator::VectorCartRotElbow(
            translation_factor * derivative_factor * data.acceleration_rel * robot->acceleration_rel * robot->max_translation_acceleration,
            derivative_factor * data.acceleration_rel * robot->acceleration_rel * robot->max_rotation_acceleration,
            elbow_factor * derivative_factor * data.acceleration_rel * robot->acceleration_rel * robot->max_elbow_acceleration
        );
        auto max_jerk = MotionGenerator::VectorCartRotElbow(
            translation_factor * std::pow(derivative_factor, 2) * data.jerk_rel * robot->jerk_rel * robot->max_translation_jerk,
            std::pow(derivative_factor, 2) * data.jerk_rel * robot->jerk_rel * robot->max_rotation_jerk,
            elbow_factor * std::pow(derivative_factor, 2) * data.jerk_rel * robot->jerk_rel * robot->max_elbow_jerk
        );
        return {max_velocity, max_acceleration, max_jerk};
    }

    template<class RobotType>
    static void setInputLimits(InputParameter<7>& input_parameters, RobotType* robot, const MotionData& data) {
        setInputLimits(input_parameters, robot, Waypoint(), data);
    }

    template<class RobotType>
    static void setInputLimits(InputParameter<7>& input_parameters, RobotType* robot, const Waypoint& waypoint, const MotionData& data) {
        const auto [max_velocity, max_acceleration, max_jerk] = getInputLimits(robot, data);
        input_parameters.max_velocity = Eigen::Map<const Vector7d>(max_velocity.data(), max_velocity.size());
        input_parameters.max_acceleration = Eigen::Map<const Vector7d>(max_acceleration.data(), max_acceleration.size());
        input_parameters.max_jerk = Eigen::Map<const Vector7d>(max_jerk.data(), max_jerk.size());

        if (!(waypoint.max_dynamics || data.max_dynamics) && waypoint.minimum_time.has_value()) {
            input_parameters.minimum_duration = waypoint.minimum_time.value();
        }
    }
};

} // namespace frankx
