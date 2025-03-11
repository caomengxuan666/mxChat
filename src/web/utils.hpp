/**
 * @FilePath     : /mxChat/src/web/utils.hpp
 * @Description  :  常用的工具函数
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-03-05 21:25:10
 * @Copyright    : PERSONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#pragma once
#include <crow.h>
#include <filesystem>
#include <spdlog/spdlog.h>

namespace utilFunc {
    /**
     * @author       : caomengxuan666
     * @brief        : 提供一个文件流式下载功能
     * @param         {string} &file_path:
     * @return        {crow::response}:response,包含大文件本身
    **/    
    inline static crow::response downloadFile(const std::string &file_path) noexcept {
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            spdlog::error("Failed to open file: {}", file_path);
            return crow::response(500, "Failed to open file");
        }

        // 使用流式传输
        crow::response res;
        res.set_static_file_info(file_path);
        res.add_header("Content-Type", "application/octet-stream");

        // Extract file name from file_path
        std::string file_name = std::filesystem::path(file_path).filename().string();
        res.add_header("Content-Disposition", "attachment; filename=" + file_name);

        spdlog::info("Returning file: {}", file_path);
        return res;
    }
    consteval inline static auto pageNotFound() {
        return R"(
            <!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>404 Not Found</title>
                <style>
                    body {
                        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
                        background-color: #f3f2f1;
                        color: #323130;
                        display: flex;
                        justify-content: center;
                        align-items: center;
                        height: 100vh;
                        margin: 0;
                    }
                    .container {
                        text-align: center;
                        padding: 20px;
                        background-color: #ffffff;
                        border-radius: 12px;
                        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
                        width: 100%;
                        max-width: 500px;
                    }
                    h1 {
                        font-size: 2.5rem;
                        margin-bottom: 20px;
                        color: #0078d4;
                    }
                    p {
                        font-size: 1.2rem;
                        margin-bottom: 30px;
                        color: #5d5a58;
                    }
                    a {
                        color: #0078d4;
                        text-decoration: none;
                    }
                    a:hover {
                        text-decoration: underline;
                    }
                </style>
            </head>
            <body>
                <div class="container">
                    <h1>404 Not Found</h1>
                    <p>The page you are looking for does not exist.</p>
                    <p><a href="/">Return to Home</a></p>
                </div>
            </body>
            </html>
            )";
    }
}// namespace utilFunc