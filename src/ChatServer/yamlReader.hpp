/**
 * @FilePath     : /mxChat/src/ChatServer/yamlReader.hpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-10 20:13:46
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once

#include <optional>
#include <spdlog/spdlog.h>
#include <string>
#include <yaml-cpp/yaml.h>

class YamlReader {
public:
    static std::optional<YAML::Node>  readYaml(const std::string &filePath);
};

inline std::optional<YAML::Node> YamlReader::readYaml(const std::string &filePath) {
    // 构造完整的文件路径
    std::string fullPath = "/home/cmx/QtProjects/mxChat/config/" + filePath;
    spdlog::info("Reading YAML file: {}", fullPath);

    try {
        // 直接加载并解析 YAML 文件
        YAML::Node config = YAML::LoadFile(fullPath);
        return config; // 返回解析后的 YAML 节点
    } catch (const YAML::Exception &e) {
        spdlog::error("YAML parsing error: {} at line {}, column {}", e.what(), e.mark.line, e.mark.column);
        return std::nullopt;
    } catch (const std::exception &e) {
        spdlog::error("Error reading YAML file: {}", e.what());
        return std::nullopt;
    }
}