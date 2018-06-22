-- --------------------------------------------------------
-- 主机:                           127.0.0.1
-- 服务器版本:                        5.5.20 - MySQL Community Server (GPL)
-- 服务器操作系统:                      Win64
-- HeidiSQL 版本:                  8.2.0.4675
-- --------------------------------------------------------

alter table user add Gold INT(11) default 0;
alter table user add NewGolduser INT(11) default 0;
alter table user add RemainingGifts INT(11) default 0;
alter table user add LastGiftTime INT(11) default 0;

