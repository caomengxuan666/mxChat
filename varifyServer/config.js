const fs = require('fs');

let config = JSON.parse(fs.readFileSync('config.json', 'utf8'));

let email_user = config.email.user;
let email_pass = config.email.pass;

// 提供默认值或检查字段是否存在
let mysql_host = config.mysql?.host || 'localhost'; // 如果 mysql 不存在，使用默认值 'localhost'
let mysql_port = config.mysql?.port || 3306;       // 如果 mysql 不存在，使用默认值 3306
let redis_host = config.redis?.host || 'localhost'; // 如果 redis 不存在，使用默认值 'localhost'
let redis_port = config.redis?.port || 6379;       // 如果 redis 不存在，使用默认值 6379
let redis_passwd = config.redis?.passwd || '';     // 如果 redis 不存在，使用默认值空字符串

let code_prefix = "code_";

module.exports = {
    email_pass,
    email_user,
    mysql_host,
    mysql_port,
    redis_host,
    redis_port,
    redis_passwd,
    code_prefix
};