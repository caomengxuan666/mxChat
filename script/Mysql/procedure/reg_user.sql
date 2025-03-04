DROP PROCEDURE reg_user;
CREATE DEFINER=`cmx`@`localhost` PROCEDURE `reg_user`(
    IN `new_name` VARCHAR(255), 
    IN `new_email` VARCHAR(255), 
    IN `new_pwd` VARCHAR(255), 
    OUT `result` INT)
BEGIN
    DECLARE new_id INT;

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

    -- 检查用户名是否已存在
    IF EXISTS (SELECT 1 FROM `user` WHERE `name` = new_name) THEN
        SET result = 0; -- 用户名已存在
        COMMIT;
    ELSE
        -- 用户名不存在，检查email是否已存在
        IF EXISTS (SELECT 1 FROM `user` WHERE `email` = new_email) THEN
            SET result = 0; -- email已存在
            COMMIT;
        ELSE
            -- 更新 user_id 表，获取新 ID
            UPDATE `user_id` SET `id` = `id` + 1;
            SELECT `id` INTO new_id FROM `user_id` LIMIT 1; -- 确保只获取一行数据

            -- 在 user 表中插入新记录
            INSERT INTO `user` (`uid`, `name`, `email`, `pwd`) VALUES (new_id, new_name, new_email, new_pwd);
            
            -- 设置 result 为新插入的 uid
            SET result = new_id; -- 插入成功，返回新的 uid
            COMMIT;
        END IF;
    END IF;
END