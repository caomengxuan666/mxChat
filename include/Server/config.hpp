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
        _yamlDoc.reset(); // 清除缓存
    }

    void setRootPath(const std::string& rootPath) {
        _configRoot = rootPath;
        spdlog::info("Set config root path: {}", _configRoot);
        _yamlDoc.reset(); // 清除缓存
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

    YAML::Node operator[](const std::string& key) const {
        if (!_yamlDoc) {
            try {
                spdlog::info("Loading YAML file: {}", _configName);
                _yamlDoc = YAML::LoadFile(_configName);
            } catch (const YAML::Exception& e) {
                spdlog::error("Failed to load YAML file: {}", e.what());
                throw;
            }
        }
        return (*_yamlDoc)[key];
    }

private:
    Config_Manager() {
        char* configRootEnv = std::getenv("CONFIG_ROOT");
        if (configRootEnv) {
            _configRoot = configRootEnv;
        } else {
            std::filesystem::path source_dir = std::filesystem::current_path();
            // current_path/ + config
            _configRoot = source_dir / "config";
        }
        _configRoot = "/home/cmx/QtProjects/mxChat/config/";
        spdlog::info("Config root path: {}", _configRoot);
    }

    mutable std::optional<YAML::Node> _yamlDoc;
    std::string _configRoot;
    std::string _configName;
};

#endif // CONFIG_MANAGER_HPP