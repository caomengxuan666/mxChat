#!/bin/bash

# 定义源目录
SOURCE_DIR="install/Bin"

# 检查源目录是否存在
if [ ! -d "$SOURCE_DIR" ]; then
  echo "源目录 $SOURCE_DIR 不存在"
  exit 1
fi

# 获取源目录名作为压缩包名
DIRNAME=$(basename "$SOURCE_DIR")

# 创建同名的 ZIP 文件
zip -r "${DIRNAME}.zip" "$SOURCE_DIR"

echo "已创建压缩包: ${DIRNAME}.zip"