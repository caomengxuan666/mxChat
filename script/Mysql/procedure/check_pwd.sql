DROP PROCEDURE check_pwd;
CREATE DEFINER=`cmx`@`localhost` PROCEDURE `check_pwd`(
    IN `input_name` VARCHAR(255), 
    IN `input_pwd` VARCHAR(255), 
    OUT `result` INT)
BEGIN
    DECLARE user_count INT;

    -- 如果在执行过程中遇到任何错误，则回滚事务
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        -- 回滚事务
        ROLLBACK;
        -- 设置返回值为-1，表示错误
        SET result = -1;
    END;
    
    -- 开始事务
    START TRANSACTION;

    -- 检查用户名和密码是否匹配
    SELECT COUNT(*) INTO user_count 
    FROM `user` 
    WHERE `name` = input_name AND `pwd` = input_pwd;

    IF user_count = 1 THEN
        SET result = 1; -- 用户名和密码匹配
    ELSE
        SET result = 0; -- 用户名或密码不匹配
    END IF;

    COMMIT;
END