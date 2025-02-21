#include <cassert>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <string>
#include <yaml-cpp/yaml.h>

#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

class Config_Manager {
public:
    static Config_Manager& getInstance() {
        static Config_Manager instance;
        return instance;
    }

    void setYamlPath(const std::string& yamlName) {
        _configName = _configRoot + yamlName;
        spdlog::info("Set YAML path: {}", _configName);
    }

    void setRootPath(const std::string& rootPath) {
        _configRoot = rootPath;
        spdlog::info("Set config root path: {}", _configRoot);
    }

    std::string getYamlPath() const {
        return _configName;
    }

    YAML::Node loadYamlDoc() const {
        try {
            spdlog::info("Loading YAML file: {}", _configName);
            return YAML::LoadFile(_configName);
        } catch (const YAML::Exception& e) {
            spdlog::error("Failed to load YAML file: {}", e.what());
            throw;
        }
    }

private:
    Config_Manager() {
        char* configRootEnv = std::getenv("CONFIG_ROOT");
        if (configRootEnv) {
            _configRoot = configRootEnv;
        } else {
            std::filesystem::path buildDir = std::filesystem::current_path();
            _configRoot = buildDir.parent_path().parent_path() / "config/";
        }
        spdlog::info("Config root path: {}", _configRoot);
    }

    std::string _configRoot;
    std::string _configName;
};

#endif // CONFIG_MANAGER_HPP