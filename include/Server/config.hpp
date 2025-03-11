/**
 * @FilePath     : /mxChat/include/Server/config.hpp
 * @Description  :  YAML配置管理类
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-11 20:35:02
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include <cassert>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <string>
#include <yaml-cpp/yaml.h>

#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

/**
 * @author       : caomengxuan666
 * @brief        : YAML配置的管理单例类,header only
**/
class Config_Manager {
public:
    //Config_Manager(const Config_Manager&) = delete;
    Config_Manager& operator=(const Config_Manager&) = delete;

    /**
     * @author       : caomengxuan666
     * @brief        : 返回YAML配置管理单例
     * @return        {Config_Manager} 返回的管理器单例对象
    **/    
    static Config_Manager& getInstance() {
        static Config_Manager instance;
        return instance;
    }

    /**
     * @author       : caomengxuan666
     * @brief        : 设置YAML配置文件名称
     * @param         {string&} yamlName:
     * @return        {*}
    **/    
    void setYamlPath(const std::string& yamlName) {
        _configName = _configRoot + yamlName;
        spdlog::info("Set YAML path: {}", _configName);
        _yamlDoc.reset(); // 清除缓存
    }

    
    /**
     * @author       : caomengxuan666
     * @brief        : 设置YAML配置文件根目录
     * @param         {string&} rootPath:
     * @return        {*}
    **/    
    void setRootPath(const std::string& rootPath) {
        _configRoot = rootPath;
        spdlog::info("Set config root path: {}", _configRoot);
        _yamlDoc.reset(); // 清除缓存
    }

    /**
     * @author       : caomengxuan666
     * @brief        : 返回YAML配置文件名称
     * @return        {*}
    **/    
    std::string getYamlPath() const {
        return _configName;
    }

    /**
     * @author       : caomengxuan666
     * @brief        : 返回整个YAML配置文件内容
     * @return        {YAML::Node} 查询到的YAML::Node文件Doc对象
    **/    
    YAML::Node loadYamlDoc() const {
        try {
            spdlog::info("Loading YAML file: {}", _configName);
            return YAML::LoadFile(_configName);
        } catch (const YAML::Exception& e) {
            spdlog::error("YAML parsing error: {} at line {}, column {}", e.what(), e.mark.line, e.mark.column);
            throw;
        }
    }

    /**
     * @author       : caomengxuan666
     * @brief        : 根据key返回YAML配置文件内容
     * @return        {YAML::Node} 查询到的YAML::Node
    **/    
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
    /**
     * @author       : caomengxuan666
     * @brief        : 构造函数，初始化root路径文件夹
     * @return        {*}
    **/
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