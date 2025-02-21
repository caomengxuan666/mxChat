let code_prefix = "code_";
let reset_prefix= "code_reset_"
//所有的验证码，规定都要以code_开头,用于注册的是code_，如果是找回直接往后加后缀，防止重复

const Errors = {
    Success : 0,
    RedisErr : 1,
    Exception : 2,
};


module.exports = {code_prefix,reset_prefix,Errors}