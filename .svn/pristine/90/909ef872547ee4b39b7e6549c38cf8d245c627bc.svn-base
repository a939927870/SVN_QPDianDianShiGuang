-- --------------------------------------------------------
-- 主机:                           127.0.0.1
-- 服务器版本:                        5.5.20 - MySQL Community Server (GPL)
-- 服务器操作系统:                      Win64
-- HeidiSQL 版本:                  8.2.0.4675
-- --------------------------------------------------------

alter table user modify Gold INT(11) default 0;
alter table user modify NewGolduser INT(11) default 0;
alter table user modify RemainingGifts INT(11) default 0;
alter table user modify LastGiftTime INT(11) default 0;
update user set Gold=0 where 1;
update user set NewGolduser=0 where 1;
update user set RemainingGifts=0 where 1;
update user set LastGiftTime=0 where 1;
