const grpc = require('@grpc/grpc-js');
const emailModule = require('./email.js');
const config_module = require('./config.js'); // 引入 config_module
const const_module = require('./const.js');
const message_proto = require('./proto'); // 导入 proto.js 中定义的 message_proto
const redis_module = require('./redis');

// 生成指定长度的随机数字验证码
function generateShortCode(length = 6) {
    let code = '';
    for (let i = 0; i < length; i++) {
        code += Math.floor(Math.random() * 10); // 随机生成0-9的数字
    }
    return code;
}

async function GetVarifyCode(call, callback) {
    console.log("email is ", call.request.email);
    try {
        let query_res = await redis_module.GetRedis(const_module.code_prefix + call.request.email);
        console.log("query_res is ", query_res);

        let verifyCode;
        if (query_res == null) {
            // 生成6位数字验证码
            verifyCode = generateShortCode(6);
            let bres = await redis_module.SetRedisExpire(const_module.code_prefix + call.request.email, verifyCode, 18000);
            if (!bres) {
                callback(null, {
                    email: call.request.email,
                    error: const_module.Errors.RedisErr
                });
                return;
            }
        } else {
            verifyCode = query_res;
        }

        console.log("verifyCode is ", verifyCode);

        // 构造HTML格式的邮件内容
        const htmlContent = `
            <div style="font-family: Arial, sans-serif; line-height: 1.6; color: #333;">
                <h2 style="color: #007BFF;">您的验证码已生成</h2>
                <p>您好，</p>
                <p>感谢您使用我们的mxChat！以下是您的验证码，请在 <strong>三分钟内</strong> 完成注册：</p>
                <div style="background-color: #f4f4f4; padding: 15px; border-radius: 5px; text-align: center; font-size: 24px; font-weight: bold; color: #007BFF; margin: 20px 0;">
                    ${verifyCode}
                </div>
                <p>请妥善保管此验证码，切勿泄露给他人。</p>
                <p>如有任何疑问，请随时联系我们。</p>
                <p style="color: #888;">祝您生活愉快！</p>
                <hr style="border: none; border-top: 1px solid #ccc; margin: 20px 0;" />
                <p style="font-size: 12px; color: #888;">此邮件为系统自动发送，请勿直接回复。</p>
            </div>
        `;

        // 发送邮件
        let mailOptions = {
            from: config_module.email_user, // 修改为从配置文件中读取的邮箱地址
            to: call.request.email,
            subject: '注册mxChat的验证码',
            html: htmlContent, // 使用 HTML 格式
        };

        let send_res = await emailModule.SendMail(mailOptions);
        console.log("send res is ", send_res);

        // 返回成功响应
        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Success,
            verifyCode: verifyCode // 可选：将验证码返回给调用方（仅用于调试）
        });

    } catch (error) {
        console.log("catch error is ", error);
        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Exception
        });
    }
}

async function SendResetPasswordCode(call, callback) {
    console.log("email is ", call.request.email);
    try {
        // 生成6位数字验证码
        let verifyCode = generateShortCode(6);
        let bres = await redis_module.SetRedisExpire(const_module.reset_code_prefix + call.request.email, verifyCode, 18000);
        if (!bres) {
            callback(null, {
                email: call.request.email,
                error: const_module.Errors.RedisErr
            });
            return;
        }

        console.log("verifyCode is ", verifyCode);

        // 构造HTML格式的邮件内容
        const htmlContent = `
            <div style="font-family: Arial, sans-serif; line-height: 1.6; color: #333;">
                <h2 style="color: #007BFF;">您的重置密码验证码已生成</h2>
                <p>您好，</p>
                <p>感谢您使用我们的mxChat！以下是您的验证码，请在 <strong>三分钟内</strong> 完成密码重置：</p>
                <div style="background-color: #f4f4f4; padding: 15px; border-radius: 5px; text-align: center; font-size: 24px; font-weight: bold; color: #007BFF; margin: 20px 0;">
                    ${verifyCode}
                </div>
                <p>请妥善保管此验证码，切勿泄露给他人。</p>
                <p>如有任何疑问，请随时联系我们。</p>
                <p style="color: #888;">祝您生活愉快！</p>
                <hr style="border: none; border-top: 1px solid #ccc; margin: 20px 0;" />
                <p style="font-size: 12px; color: #888;">此邮件为系统自动发送，请勿直接回复。</p>
            </div>
        `;

        // 发送邮件
        let mailOptions = {
            from: config_module.email_user, // 修改为从配置文件中读取的邮箱地址
            to: call.request.email,
            subject: '重置mxChat密码的验证码',
            html: htmlContent, // 使用 HTML 格式
        };

        let send_res = await emailModule.SendMail(mailOptions);
        console.log("send res is ", send_res);

        // 返回成功响应
        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Success,
            verifyCode: verifyCode // 可选：将验证码返回给调用方（仅用于调试）
        });

    } catch (error) {
        console.log("catch error is ", error);
        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Exception
        });
    }
}

function main() {
    var server = new grpc.Server();
    server.addService(message_proto.VarifyService.service, { 
        GetVarifyCode: GetVarifyCode,
        SendResetPasswordCode: SendResetPasswordCode // 添加新的服务方法
    });
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        server.start();
        console.log('grpc server started');
    });
}

main();