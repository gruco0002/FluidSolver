#pragma once

#include "visualizer/Image.hpp"

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

namespace YAML
{
    template <> struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.SetStyle(YAML::EmitterStyle::Flow);
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
            {
                return false;
            }
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template <> struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.SetStyle(YAML::EmitterStyle::Flow);
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
            {
                return false;
            }
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template <> struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.SetStyle(YAML::EmitterStyle::Flow);
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
            {
                return false;
            }
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template <> struct convert<LibFluid::Image::Color>
    {
        static Node encode(const LibFluid::Image::Color& rhs)
        {
            Node node;
            node.SetStyle(YAML::EmitterStyle::Flow);
            node.push_back(rhs.r);
            node.push_back(rhs.g);
            node.push_back(rhs.b);
            node.push_back(rhs.a);
            return node;
        }

        static bool decode(const Node& node, LibFluid::Image::Color& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
            {
                return false;
            }
            rhs.r = (uint8_t)node[0].as<int>();
            rhs.g = (uint8_t)node[1].as<int>();
            rhs.b = (uint8_t)node[2].as<int>();
            rhs.a = (uint8_t)node[3].as<int>();
            return true;
        }
    };
} // namespace YAML