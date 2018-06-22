/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 50528
Source Host           : localhost:3306
Source Database       : LogicDB_Doudizhu

Target Server Type    : MYSQL
Target Server Version : 50528
File Encoding         : 65001

Date: 2017-10-16 16:38:52
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `activegiftstock`
-- ----------------------------
DROP TABLE IF EXISTS `activegiftstock`;
CREATE TABLE `activegiftstock` (
  `ActiveId` int(11) NOT NULL,
  `Gift1` int(11) DEFAULT '0',
  `Gift2` int(11) DEFAULT '0',
  `Gift3` int(11) DEFAULT '0',
  `Gift4` int(11) DEFAULT '0',
  `Gift5` int(11) DEFAULT '0',
  `Gift6` int(11) DEFAULT '0',
  `Gift7` int(11) DEFAULT '0',
  `Gift8` int(11) DEFAULT '0',
  `Gift9` int(11) DEFAULT '0',
  `Gift10` int(11) DEFAULT '0',
  `Gift11` int(11) DEFAULT '0',
  `Gift12` int(11) DEFAULT '0',
  PRIMARY KEY (`ActiveId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of activegiftstock
-- ----------------------------

-- ----------------------------
-- Table structure for `activelog`
-- ----------------------------
DROP TABLE IF EXISTS `activelog`;
CREATE TABLE `activelog` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `activeID` int(11) NOT NULL,
  `userID` int(11) NOT NULL,
  `phone` varchar(20) NOT NULL DEFAULT '',
  `reward` int(11) NOT NULL,
  `date` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of activelog
-- ----------------------------

-- ----------------------------
-- Table structure for `bindingrelationships`
-- ----------------------------
DROP TABLE IF EXISTS `bindingrelationships`;
CREATE TABLE `bindingrelationships` (
  `Invitee` int(11) NOT NULL,
  `Inviter` int(11) NOT NULL,
  `BindingTime` int(11) DEFAULT NULL,
  `TaskFinished` int(11) DEFAULT NULL,
  PRIMARY KEY (`Invitee`,`Inviter`),
  KEY `Inviter` (`Inviter`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of bindingrelationships
-- ----------------------------

-- ----------------------------
-- Table structure for `log`
-- ----------------------------
DROP TABLE IF EXISTS `log`;
CREATE TABLE `log` (
  `Id` varchar(16) NOT NULL,
  `Time` int(11) DEFAULT '0',
  `Pos1` int(11) DEFAULT '0',
  `Pos2` int(11) DEFAULT '0',
  `Pos3` int(11) DEFAULT '0',
  `Pos4` int(11) DEFAULT '0',
  `Flag` int(11) DEFAULT '0',
  `DeskId` int(11) DEFAULT '0',
  `MaxCircle` int(11) DEFAULT '0',
  `CurCircle` int(11) DEFAULT '0',
  `Pass` varchar(50) DEFAULT '',
  `Score1` int(11) DEFAULT '0',
  `Score2` int(11) DEFAULT '0',
  `Score3` int(11) DEFAULT '0',
  `Score4` int(11) DEFAULT '0',
  `Reset` int(11) DEFAULT '0',
  `Data` varchar(2048) DEFAULT '',
  `PlayType` varchar(64) DEFAULT '',
  PRIMARY KEY (`Id`),
  KEY `Pos1` (`Pos1`) USING BTREE,
  KEY `Pos2` (`Pos2`) USING BTREE,
  KEY `Time` (`Time`) USING BTREE,
  KEY `Pos3` (`Pos3`) USING BTREE,
  KEY `Pos4` (`Pos4`) USING BTREE,
  KEY `MaxCircle` (`MaxCircle`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log
-- ----------------------------

-- ----------------------------
-- Table structure for `luckydraw`
-- ----------------------------
DROP TABLE IF EXISTS `luckydraw`;
CREATE TABLE `luckydraw` (
  `UserId` int(11) NOT NULL,
  `LuckyDrawNum` int(11) DEFAULT '0',
  PRIMARY KEY (`UserId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of luckydraw
-- ----------------------------

-- ----------------------------
-- Table structure for `sharevideo`
-- ----------------------------
DROP TABLE IF EXISTS `sharevideo`;
CREATE TABLE `sharevideo` (
  `VideoId` varchar(16) NOT NULL,
  `ShareId` int(11) NOT NULL DEFAULT '0',
  `Time` int(11) NOT NULL DEFAULT '0',
  `UserId1` int(11) NOT NULL DEFAULT '0',
  `UserId2` int(11) NOT NULL DEFAULT '0',
  `UserId3` int(11) NOT NULL DEFAULT '0',
  `UserId4` int(11) NOT NULL DEFAULT '0',
  `Score1` int(11) NOT NULL DEFAULT '0',
  `Score2` int(11) NOT NULL DEFAULT '0',
  `Score3` int(11) NOT NULL DEFAULT '0',
  `Score4` int(11) NOT NULL DEFAULT '0',
  `Zhuang` int(11) NOT NULL DEFAULT '0',
  `Data` varchar(4096) NOT NULL,
  `PlayType` varchar(64) NOT NULL,
  `DeskId` int(11) NOT NULL,
  `Flag` int(11) NOT NULL DEFAULT '0',
  `MaxCircle` int(11) NOT NULL DEFAULT '0',
  `CurCircle` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`VideoId`),
  KEY `ShareId` (`ShareId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of sharevideo
-- ----------------------------

-- ----------------------------
-- Table structure for `sportdailystat`
-- ----------------------------
DROP TABLE IF EXISTS `sportdailystat`;
CREATE TABLE `sportdailystat` (
  `SportId` int(11) NOT NULL COMMENT '比赛id',
  `StatTime` date NOT NULL COMMENT '统计的日期',
  `UpdTime` datetime NOT NULL COMMENT '更新的时间',
  `ReptSign` int(11) DEFAULT NULL COMMENT '重复报名人数',
  `Sign` int(11) DEFAULT NULL COMMENT '报名人数',
  `ReptJoin` int(11) DEFAULT NULL COMMENT '重复实际加入比赛的人数',
  `Join` int(11) DEFAULT NULL COMMENT '不重复加入比赛的人数',
  `LoopNum` int(11) DEFAULT NULL COMMENT '比赛的场次',
  `Custom1` int(11) DEFAULT NULL COMMENT '自定义参赛',
  `Custom2` int(11) DEFAULT NULL COMMENT '自定义参赛',
  `Custom3` int(11) DEFAULT NULL COMMENT '自定义参赛',
  `Custom4` int(11) DEFAULT NULL COMMENT '自定义参赛',
  `Custom5` int(11) DEFAULT NULL COMMENT '自定义参赛',
  PRIMARY KEY (`SportId`,`StatTime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of sportdailystat
-- ----------------------------

-- ----------------------------
-- Table structure for `sportitemrecord`
-- ----------------------------
DROP TABLE IF EXISTS `sportitemrecord`;
CREATE TABLE `sportitemrecord` (
  `Id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT '递增id',
  `SportId` int(11) NOT NULL DEFAULT '0' COMMENT '比赛场配置id',
  `QueueId` int(11) DEFAULT NULL COMMENT '队列id',
  `RandId` int(11) DEFAULT NULL COMMENT '随机id',
  `SignTime` int(11) DEFAULT NULL COMMENT '报名时间',
  `StartTime` int(11) DEFAULT NULL COMMENT '比赛开始时间',
  `MatchTime` int(11) DEFAULT NULL COMMENT '比赛时间',
  `SignNum` int(11) DEFAULT NULL COMMENT '报名人数',
  `MatchNum` int(11) DEFAULT NULL COMMENT '比赛人数',
  PRIMARY KEY (`Id`),
  KEY `SportId` (`SportId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of sportitemrecord
-- ----------------------------

-- ----------------------------
-- Table structure for `sportitemstat`
-- ----------------------------
DROP TABLE IF EXISTS `sportitemstat`;
CREATE TABLE `sportitemstat` (
  `Id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '递增id',
  `SportId` int(11) NOT NULL DEFAULT '0' COMMENT '比赛场配置id',
  `QueueId` int(11) NOT NULL COMMENT '队列id',
  `RandId` int(11) NOT NULL COMMENT '随机id',
  `SignTime` datetime DEFAULT NULL COMMENT '报名时间',
  `StartTime` datetime DEFAULT NULL COMMENT '比赛开始时间',
  `SignStartDiff` int(11) DEFAULT NULL COMMENT '报名到开赛的时间差',
  `MatchTime` int(11) DEFAULT NULL COMMENT '比赛时间-比赛消耗时间',
  `SignNum` int(11) DEFAULT NULL COMMENT '报名人数',
  `MatchNum` int(11) DEFAULT NULL COMMENT '比赛人数',
  `RewardUserNum` int(11) DEFAULT NULL COMMENT '奖励人数',
  `CustomNum1` int(11) DEFAULT NULL COMMENT '消耗数值1',
  `CustomNum2` int(11) DEFAULT NULL COMMENT '消耗数值2',
  `CustomNum3` int(11) DEFAULT NULL COMMENT '消耗数值3',
  `CustomNum4` int(11) DEFAULT NULL COMMENT '消耗数值4',
  `CustomNum5` int(11) DEFAULT NULL COMMENT '消耗数值5',
  `RewardNum1` int(11) DEFAULT NULL COMMENT '奖励数值1',
  `RewardNum2` int(11) DEFAULT NULL COMMENT '奖励数值2',
  `RewardNum3` int(11) DEFAULT NULL COMMENT '奖励数值3',
  `RewardNum4` int(11) DEFAULT NULL COMMENT '奖励数值4',
  `RewardNum5` int(11) DEFAULT NULL COMMENT '奖励数值5',
  PRIMARY KEY (`Id`),
  KEY `SportId` (`SportId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of sportitemstat
-- ----------------------------

-- ----------------------------
-- Table structure for `sportrecordinfo`
-- ----------------------------
DROP TABLE IF EXISTS `sportrecordinfo`;
CREATE TABLE `sportrecordinfo` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT '唯一id',
  `uid` int(11) NOT NULL COMMENT '用户id',
  `sportid` int(11) DEFAULT NULL COMMENT '比赛场id',
  `time` int(11) DEFAULT NULL COMMENT '保存时间',
  `tipsId` int(11) DEFAULT NULL COMMENT '记录内容提示id',
  `rewardid` int(11) DEFAULT NULL COMMENT '赛场奖励 的id',
  `read` int(11) DEFAULT NULL COMMENT '读取标识 0 尚未读取 1 已读',
  `draw` int(11) DEFAULT '0' COMMENT '领取标识 如果m_rewardid 不为0 0 尚未领取 1 已经领取',
  `rank` int(11) DEFAULT NULL COMMENT '排名',
  `share` int(11) DEFAULT NULL COMMENT '是否已分享 0未分享 1已分享',
  `randid` int(11) DEFAULT '0' COMMENT '随机id，判断是某个竞技场',
  `groupid` int(11) DEFAULT '0' COMMENT '组id号',
  PRIMARY KEY (`id`,`uid`),
  KEY `uid` (`uid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of sportrecordinfo
-- ----------------------------

-- ----------------------------
-- Table structure for `sportsigninfo`
-- ----------------------------
DROP TABLE IF EXISTS `sportsigninfo`;
CREATE TABLE `sportsigninfo` (
  `UserId` int(11) NOT NULL COMMENT '用户的id',
  `Nike` varchar(126) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL COMMENT '昵称',
  `SportId` int(11) NOT NULL COMMENT '比赛场id',
  `RandId` int(11) NOT NULL COMMENT '比赛场随机id',
  `SignTime` datetime DEFAULT NULL COMMENT '报名时间',
  `Custom` varchar(11) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL COMMENT '配置表消耗字符串',
  PRIMARY KEY (`UserId`,`SportId`,`RandId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of sportsigninfo
-- ----------------------------

-- ----------------------------
-- Table structure for `sportuserinfo`
-- ----------------------------
DROP TABLE IF EXISTS `sportuserinfo`;
CREATE TABLE `sportuserinfo` (
  `UserId` int(11) NOT NULL COMMENT '用户id',
  `Nike` varchar(255) NOT NULL COMMENT '账户名',
  `Sex` int(255) NOT NULL COMMENT '性别',
  `HeadImageUrl` varchar(255) NOT NULL COMMENT '头像连接',
  `Sportid` int(11) NOT NULL DEFAULT '0' COMMENT '比赛场id',
  `Rank` int(11) NOT NULL COMMENT '预留排名等级',
  `Join` int(11) NOT NULL COMMENT '参赛次数',
  `MaxRank` int(11) NOT NULL COMMENT '最好的名次',
  `Super` int(11) NOT NULL COMMENT '冠军次数',
  `Final` int(11) NOT NULL COMMENT '决赛次数',
  `Reward` int(11) NOT NULL COMMENT '获得奖励次数',
  `TotalGift` int(11) NOT NULL COMMENT '总礼券数',
  `TotalCard` int(11) NOT NULL COMMENT '总房卡数',
  `TotalReal` int(11) NOT NULL COMMENT '总实物数',
  `InNum` int(11) NOT NULL COMMENT '加入比赛次数',
  `WinNum` int(11) NOT NULL COMMENT '胜利次数',
  `MaxScore` int(11) NOT NULL COMMENT '获得的最大积分',
  `JoinTimesByDay` int(11) DEFAULT '0' COMMENT '添加每日报名次数字段',
  PRIMARY KEY (`UserId`,`Sportid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of sportuserinfo
-- ----------------------------

-- ----------------------------
-- Table structure for `taskprogress`
-- ----------------------------
DROP TABLE IF EXISTS `taskprogress`;
CREATE TABLE `taskprogress` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `TaskId` int(11) NOT NULL,
  `StartTime` int(11) DEFAULT NULL,
  `ExpiredTime` int(11) DEFAULT NULL,
  `Total` int(11) DEFAULT NULL,
  `Progress` int(11) DEFAULT NULL,
  `UserId` int(11) DEFAULT NULL,
  `RewardRecved` int(11) DEFAULT NULL,
  PRIMARY KEY (`Id`),
  KEY `TaskId` (`TaskId`,`UserId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of taskprogress
-- ----------------------------

-- ----------------------------
-- Table structure for `video`
-- ----------------------------
DROP TABLE IF EXISTS `video`;
CREATE TABLE `video` (
  `Id` varchar(16) NOT NULL,
  `UserId1` int(11) DEFAULT NULL,
  `Time` int(11) DEFAULT '0',
  `UserId2` int(11) DEFAULT NULL,
  `UserId3` int(11) DEFAULT NULL,
  `UserId4` int(11) DEFAULT NULL,
  `Data` varchar(4096) DEFAULT NULL,
  `DeskId` int(11) DEFAULT '0',
  `Flag` int(11) DEFAULT '0',
  `CurCircle` int(11) DEFAULT '0',
  `MaxCircle` int(11) DEFAULT '0',
  `Score1` int(11) DEFAULT '0',
  `Score2` int(11) DEFAULT '0',
  `Score3` int(11) DEFAULT '0',
  `Score4` int(11) DEFAULT '0',
  `Zhuang` int(11) DEFAULT '0',
  `PlayType` varchar(64) DEFAULT '',
  PRIMARY KEY (`Id`),
  KEY `Time` (`Time`) USING BTREE,
  KEY `UserId1` (`UserId1`) USING BTREE,
  KEY `UserId2` (`UserId2`) USING BTREE,
  KEY `UserId3` (`UserId3`) USING BTREE,
  KEY `UserId4` (`UserId4`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of video
-- ----------------------------
