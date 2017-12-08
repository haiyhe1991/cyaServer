/*
Navicat MySQL Data Transfer

Source Server         : root
Source Server Version : 50515
Source Host           : localhost:3306
Source Database       : sgs

Target Server Type    : MYSQL
Target Server Version : 50515
File Encoding         : 65001

Date: 2017-12-08 11:18:13
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for achievement
-- ----------------------------
DROP TABLE IF EXISTS `achievement`;
CREATE TABLE `achievement` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `point` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '成就点数',
  `userdata` varbinary(1024) DEFAULT NULL COMMENT '用户信息',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `achievement_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `achievement_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of achievement
-- ----------------------------

-- ----------------------------
-- Table structure for achievement_total
-- ----------------------------
DROP TABLE IF EXISTS `achievement_total`;
CREATE TABLE `achievement_total` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `common` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '击杀小怪数量',
  `cream` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '击杀精英怪数量',
  `boss` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '击杀boss怪数量',
  `boxsum` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '打开宝箱数量',
  `continuous` int(11) NOT NULL DEFAULT '0' COMMENT '连击次数',
  `speed_evaluation` int(11) NOT NULL DEFAULT '0' COMMENT '达成速度评价副本数量',
  `hurt_evaluation` int(11) NOT NULL DEFAULT '0' COMMENT '达成伤害评价副本数量',
  `star_evaluation` int(11) NOT NULL DEFAULT '0' COMMENT '达成三星评价副本数量',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `killmonster_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `achievement_total_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of achievement_total
-- ----------------------------
INSERT INTO `achievement_total` VALUES ('1762', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `achievement_total` VALUES ('1880', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `achievement_total` VALUES ('30849', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `achievement_total` VALUES ('74189477', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `achievement_total` VALUES ('111991989', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `achievement_total` VALUES ('116932180', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `achievement_total` VALUES ('126732690', '0', '0', '0', '0', '0', '0', '0', '0');

-- ----------------------------
-- Table structure for activeinst
-- ----------------------------
DROP TABLE IF EXISTS `activeinst`;
CREATE TABLE `activeinst` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `actorid` int(10) unsigned DEFAULT '0' COMMENT '角色id',
  `instid` smallint(5) unsigned DEFAULT '0' COMMENT '活动副本id',
  `times` tinyint(3) unsigned DEFAULT '0' COMMENT '当天挑战次数',
  `tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '当天最后一次挑战时间',
  PRIMARY KEY (`id`),
  KEY `activeinst_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `active_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of activeinst
-- ----------------------------

-- ----------------------------
-- Table structure for activity
-- ----------------------------
DROP TABLE IF EXISTS `activity`;
CREATE TABLE `activity` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `actorid` int(10) unsigned DEFAULT '0' COMMENT '角色id',
  `activityid` int(10) unsigned DEFAULT '0' COMMENT '角色参加活动id',
  `desc` varchar(256) DEFAULT '' COMMENT '活动描述',
  `tts` timestamp NULL DEFAULT NULL COMMENT '参加活动时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `activity_idx_1` (`actorid`,`activityid`) USING BTREE,
  CONSTRAINT `activity_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of activity
-- ----------------------------

-- ----------------------------
-- Table structure for actor
-- ----------------------------
DROP TABLE IF EXISTS `actor`;
CREATE TABLE `actor` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id,自动递增(角色id)',
  `createtts` timestamp NULL DEFAULT NULL COMMENT '角色创建时间',
  `accountid` int(11) unsigned NOT NULL COMMENT '所属账户id',
  `nick` char(33) NOT NULL COMMENT '昵称',
  `sex` enum('0','1') DEFAULT NULL COMMENT '性别(0,1)',
  `gold` bigint(20) NOT NULL COMMENT '游戏币',
  `level` tinyint(4) unsigned NOT NULL COMMENT '角色等级',
  `occu` smallint(6) unsigned DEFAULT NULL COMMENT '职业称号',
  `exp` bigint(20) NOT NULL COMMENT '经验',
  `cash` int(11) unsigned DEFAULT NULL COMMENT '现金',
  `cashcount` int(11) unsigned DEFAULT NULL COMMENT '充值现金总量',
  `actortype` smallint(6) unsigned DEFAULT NULL COMMENT '角色类型',
  `hp` int(11) unsigned DEFAULT NULL COMMENT '基础生命值',
  `mp` int(11) unsigned DEFAULT NULL COMMENT '基础魔力值',
  `attack` int(11) unsigned DEFAULT '0' COMMENT '基础攻击值',
  `def` int(11) unsigned DEFAULT '0' COMMENT '基础防御值',
  `vit` int(11) unsigned DEFAULT NULL COMMENT '体力',
  `cp` int(11) unsigned DEFAULT NULL COMMENT '战力',
  `ap` int(11) unsigned DEFAULT NULL COMMENT '剩余属性点',
  `packsize` tinyint(4) unsigned DEFAULT NULL COMMENT '背包大小',
  `waresize` tinyint(4) unsigned DEFAULT NULL COMMENT '仓库大小',
  `guild` smallint(6) unsigned DEFAULT NULL COMMENT '所属公会',
  `dressid` smallint(6) unsigned DEFAULT '0' COMMENT '时装id',
  `duplicate` smallint(6) unsigned DEFAULT NULL COMMENT '所在副本',
  `coords` binary(16) DEFAULT NULL COMMENT '坐标',
  `faction` tinyint(4) DEFAULT NULL COMMENT '所在阵营id',
  `vist` int(11) unsigned DEFAULT NULL COMMENT '访问次数',
  `online` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '在线时间(秒)',
  `lvtts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '最后访问时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `actor_idx_2` (`nick`) USING BTREE,
  UNIQUE KEY `actor_idx_3` (`id`) USING BTREE,
  KEY `actor_idx_1` (`accountid`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=126732691 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of actor
-- ----------------------------
INSERT INTO `actor` VALUES ('1762', '2017-11-18 16:00:50', '54880134', 'gr', '0', '0', '1', '0', '0', '0', '0', '1', '300', '30', '25', '15', '200', '222', '0', '40', '0', '0', '0', '0', 0x00000000000000000000000000000000, '0', '5', '4882', '2017-11-22 16:57:04');
INSERT INTO `actor` VALUES ('1880', '2017-10-26 10:04:23', '27377', 'oh', '1', '0', '1', '0', '0', '0', '0', '1', '300', '30', '25', '15', '200', '222', '0', '40', '0', '0', '0', '0', 0x00000000000000000000000000000000, '0', '71', '42860', '2017-12-05 09:39:06');
INSERT INTO `actor` VALUES ('30849', '2017-12-05 10:08:30', '1857', 'qqq', '0', '0', '1', '0', '0', '0', '0', '1', '300', '30', '25', '15', '200', '222', '0', '40', '0', '0', '0', '0', 0x00000000000000000000000000000000, '0', '1', '32', '2017-12-05 10:08:32');
INSERT INTO `actor` VALUES ('74189477', '2017-09-01 16:36:26', '27377', 'iampeople', '0', '0', '1', '0', '0', '0', '0', '2', '300', '30', '25', '15', '200', '222', '0', '40', '0', '0', '0', '0', 0x00000000000000000000000000000000, '0', '0', '0', '2017-09-01 16:36:26');
INSERT INTO `actor` VALUES ('111991989', '2017-09-14 11:36:05', '28409', 'dewfee', '0', '0', '1', '0', '0', '0', '0', '2', '300', '30', '25', '15', '200', '222', '0', '40', '0', '0', '0', '0', 0x00000000000000000000000000000000, '0', '38', '20544', '2017-09-23 15:17:44');
INSERT INTO `actor` VALUES ('116932180', '2017-09-01 16:33:43', '28409', 'iamgod', '1', '0', '1', '0', '0', '0', '0', '1', '300', '30', '25', '15', '200', '222', '0', '40', '0', '0', '0', '0', 0x00000000000000000000000000000000, '0', '359', '181115', '2017-12-05 09:38:30');
INSERT INTO `actor` VALUES ('126732690', '2017-11-18 16:03:45', '30933', 'qwerrr', '0', '0', '1', '0', '0', '0', '0', '1', '300', '30', '25', '15', '200', '222', '0', '40', '0', '0', '0', '0', 0x00000000000000000000000000000000, '0', '21', '2044', '2017-12-04 15:43:11');

-- ----------------------------
-- Table structure for actorlock
-- ----------------------------
DROP TABLE IF EXISTS `actorlock`;
CREATE TABLE `actorlock` (
  `actorid` int(10) unsigned NOT NULL DEFAULT '0',
  `stages` varbinary(4096) DEFAULT NULL COMMENT '解锁关卡',
  `skills` varbinary(1024) DEFAULT NULL COMMENT '解锁技能',
  `levels` varbinary(512) DEFAULT NULL COMMENT '解锁等级',
  `stages_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后一次解锁关卡时间',
  `skills_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后一次解锁技能时间',
  `levels_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后一次解锁等级时间',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `actorlock_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `actorlock_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of actorlock
-- ----------------------------

-- ----------------------------
-- Table structure for cash
-- ----------------------------
DROP TABLE IF EXISTS `cash`;
CREATE TABLE `cash` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `actornick` char(33) NOT NULL COMMENT '角色昵称',
  `cashid` smallint(6) unsigned NOT NULL COMMENT '充值id',
  `rmb` int(11) unsigned DEFAULT '0' COMMENT '人民币',
  `token` int(11) unsigned DEFAULT '0' COMMENT '代币',
  `give` int(11) unsigned DEFAULT '0' COMMENT '首充赠送',
  `tts` timestamp NULL DEFAULT NULL COMMENT '充值时间',
  `accountid` int(10) unsigned DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';

-- ----------------------------
-- Records of cash
-- ----------------------------

-- ----------------------------
-- Table structure for cashcode
-- ----------------------------
DROP TABLE IF EXISTS `cashcode`;
CREATE TABLE `cashcode` (
  `id` char(33) NOT NULL DEFAULT '' COMMENT '兑换码',
  `rewardid` smallint(5) unsigned DEFAULT '0' COMMENT '奖励id',
  `tts` timestamp NULL DEFAULT NULL COMMENT '有效期',
  `receiveid` int(10) unsigned DEFAULT '0' COMMENT '领取角色id',
  `status` tinyint(3) unsigned DEFAULT '0' COMMENT '状态(0-未兑换, 1-已兑换, 2-过期)',
  `cashtts` timestamp NULL DEFAULT NULL COMMENT '兑换日期',
  PRIMARY KEY (`id`),
  UNIQUE KEY `cashcode_idx_1` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of cashcode
-- ----------------------------

-- ----------------------------
-- Table structure for competitive
-- ----------------------------
DROP TABLE IF EXISTS `competitive`;
CREATE TABLE `competitive` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `win` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '胜利次数',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `competitive_idx` (`actorid`) USING BTREE,
  CONSTRAINT `competitive_ibfk` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of competitive
-- ----------------------------

-- ----------------------------
-- Table structure for cp_reward
-- ----------------------------
DROP TABLE IF EXISTS `cp_reward`;
CREATE TABLE `cp_reward` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `tts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '领取时间',
  `actorid` int(11) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `cprewardid` smallint(6) NOT NULL DEFAULT '0' COMMENT '奖励领取',
  `cp` int(11) DEFAULT NULL COMMENT '获取奖励时玩家的战力',
  PRIMARY KEY (`id`),
  KEY `actorid_idx` (`actorid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of cp_reward
-- ----------------------------

-- ----------------------------
-- Table structure for dailysign
-- ----------------------------
DROP TABLE IF EXISTS `dailysign`;
CREATE TABLE `dailysign` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `userdata` varbinary(1024) NOT NULL COMMENT '用户数据',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `sign_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `dailysign_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of dailysign
-- ----------------------------

-- ----------------------------
-- Table structure for dailytask
-- ----------------------------
DROP TABLE IF EXISTS `dailytask`;
CREATE TABLE `dailytask` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id',
  `actorid` int(11) unsigned DEFAULT NULL COMMENT '角色id',
  `taskid` smallint(6) unsigned DEFAULT NULL COMMENT '任务id',
  `tts` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '完成时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `dailytask_idx_1` (`actorid`,`taskid`) USING BTREE,
  CONSTRAINT `dailytask_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of dailytask
-- ----------------------------

-- ----------------------------
-- Table structure for data
-- ----------------------------
DROP TABLE IF EXISTS `data`;
CREATE TABLE `data` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `whpass` char(33) DEFAULT NULL COMMENT '仓库密码',
  `buffer` binary(17) DEFAULT NULL,
  `backpack` varbinary(4096) DEFAULT NULL COMMENT '背包数据',
  `warehouse` varbinary(4096) DEFAULT NULL COMMENT '仓库数据',
  `wearequipment` varbinary(128) DEFAULT NULL COMMENT '已穿戴装备',
  `taskaccepted` varbinary(5870) DEFAULT NULL COMMENT '已接任务',
  `ability` varbinary(1300) DEFAULT NULL COMMENT '已学技能',
  `help` varbinary(512) DEFAULT NULL COMMENT '新手引导信息',
  `dress` varbinary(2048) DEFAULT NULL COMMENT '角色所拥有时装',
  `fragment` varbinary(1024) DEFAULT NULL COMMENT '装备碎片',
  `equipsock` varbinary(70) DEFAULT NULL COMMENT '装备位属性',
  `lh_fragment` varbinary(1024) DEFAULT NULL COMMENT '炼魂碎片',
  `lh_pos` varbinary(1024) DEFAULT NULL COMMENT '炼魂部件',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `data_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `data_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of data
-- ----------------------------
INSERT INTO `data` VALUES ('1762', '123456', 0x0000000000000000000000000000000000, 0x00, 0x00, 0x060100000080270100000100010202000000812701000001000103030000008427010000010001060400000083270100000100010505000000822701000001000104060000007F2701000001000101, 0x00, 0x1DFD53050161540600C5540700295508005D2B00005E2B00005F2B0000602B0000612B0000622B0000632B0000642B0000652B0000662B0000C12B0000C22B0000C32B0000C42B0000C52B0000C62B0000C72B0000252C0000262C0000457A0000467A0000A97A0000AA7A0000AB7A0000AC7A0000, 0x00, 0x0000, 0x00, 0x06010000000000000000000002000000000000000000000300000000000000000000040000000000000000000005000000000000000000000600000000000000000000, 0x00, 0x06010002000300040005000600);
INSERT INTO `data` VALUES ('1880', '123456', 0x0000000000000000000000000000000000, 0x00, 0x00, 0x060100000080270100000100010202000000812701000001000103030000008427010000010001060400000083270100000100010505000000822701000001000104060000007F2701000001000101, 0x00, 0x1DFD53050161540600C5540700295508005D2B00005E2B00005F2B0000602B0000612B0000622B0000632B0000642B0000652B0000662B0000C12B0000C22B0000C32B0000C42B0000C52B0000C62B0000C72B0000252C0000262C0000457A0000467A0000A97A0000AA7A0000AB7A0000AC7A0000, 0x00, 0x0000, 0x00, 0x06010000000000000000000002000000000000000000000300000000000000000000040000000000000000000005000000000000000000000600000000000000000000, 0x00, 0x06010002000300040005000600);
INSERT INTO `data` VALUES ('30849', '123456', 0x0000000000000000000000000000000000, 0x00, 0x00, 0x060100000080270100000100010202000000812701000001000103030000008427010000010001060400000083270100000100010505000000822701000001000104060000007F2701000001000101, 0x00, 0x1DFD53050161540600C5540700295508005D2B00005E2B00005F2B0000602B0000612B0000622B0000632B0000642B0000652B0000662B0000C12B0000C22B0000C32B0000C42B0000C52B0000C62B0000C72B0000252C0000262C0000457A0000467A0000A97A0000AA7A0000AB7A0000AC7A0000, 0x00, 0x0000, 0x00, 0x06010000000000000000000002000000000000000000000300000000000000000000040000000000000000000005000000000000000000000600000000000000000000, 0x00, 0x06010002000300040005000600);
INSERT INTO `data` VALUES ('74189477', '123456', 0x0000000000000000000000000000000000, 0x00, 0x00, 0x060100000080270100000100010202000000812701000001000103030000008427010000010001060400000083270100000100010505000000822701000001000104060000007F2701000001000101, 0x00, 0x1DE557050149580600AD58070011590800452F0000462F0000472F0000482F0000492F00004A2F00004B2F00004C2F00004D2F00004E2F0000A92F0000AA2F0000AB2F0000AC2F0000AD2F0000AE2F0000AF2F00000D3000000E3000002D7E00002E7E0000917E0000927E0000937E0000947E0000, 0x00, 0x0000, 0x00, 0x06010000000000000000000002000000000000000000000300000000000000000000040000000000000000000005000000000000000000000600000000000000000000, 0x00, 0x06010002000300040005000600);
INSERT INTO `data` VALUES ('111991989', '123456', 0x0000000000000000000000000000000000, 0x00, 0x00, 0x060100000080270100000100010202000000812701000001000103030000008427010000010001060400000083270100000100010505000000822701000001000104060000007F2701000001000101, 0x00, 0x1DE557050149580600AD58070011590800452F0000462F0000472F0000482F0000492F00004A2F00004B2F00004C2F00004D2F00004E2F0000A92F0000AA2F0000AB2F0000AC2F0000AD2F0000AE2F0000AF2F00000D3000000E3000002D7E00002E7E0000917E0000927E0000937E0000947E0000, 0x00, 0x0000, 0x00, 0x06010000000000000000000002000000000000000000000300000000000000000000040000000000000000000005000000000000000000000600000000000000000000, 0x00, 0x06010002000300040005000600);
INSERT INTO `data` VALUES ('116932180', '123456', 0x0000000000000000000000000000000000, 0x00, 0x00, 0x060100000080270100000100010202000000812701000001000103030000008427010000010001060400000083270100000100010505000000822701000001000104060000007F2701000001000101, 0x00, 0x1DFD53050161540600C5540700295508005D2B00005E2B00005F2B0000602B0000612B0000622B0000632B0000642B0000652B0000662B0000C12B0000C22B0000C32B0000C42B0000C52B0000C62B0000C72B0000252C0000262C0000457A0000467A0000A97A0000AA7A0000AB7A0000AC7A0000, 0x00, 0x0000, 0x00, 0x06010000000000000000000002000000000000000000000300000000000000000000040000000000000000000005000000000000000000000600000000000000000000, 0x00, 0x06010002000300040005000600);
INSERT INTO `data` VALUES ('126732690', '123456', 0x0000000000000000000000000000000000, 0x00, 0x00, 0x060100000080270100000100010202000000812701000001000103030000008427010000010001060400000083270100000100010505000000822701000001000104060000007F2701000001000101, 0x00, 0x1DFD53050161540600C5540700295508005D2B00005E2B00005F2B0000602B0000612B0000622B0000632B0000642B0000652B0000662B0000C12B0000C22B0000C32B0000C42B0000C52B0000C62B0000C72B0000252C0000262C0000457A0000467A0000A97A0000AA7A0000AB7A0000AC7A0000, 0x00, 0x0000, 0x00, 0x06010000000000000000000002000000000000000000000300000000000000000000040000000000000000000005000000000000000000000600000000000000000000, 0x00, 0x06010002000300040005000600);

-- ----------------------------
-- Table structure for email
-- ----------------------------
DROP TABLE IF EXISTS `email`;
CREATE TABLE `email` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id,自动增长(邮件id)',
  `actorid` int(11) unsigned NOT NULL COMMENT '所属角色',
  `type` tinyint(4) NOT NULL COMMENT '邮件类型',
  `senderid` int(11) unsigned DEFAULT NULL COMMENT '发送者id',
  `sendernick` char(33) DEFAULT NULL COMMENT '发送者昵称',
  `caption` char(65) DEFAULT NULL COMMENT '邮件标题',
  `body` varchar(512) DEFAULT NULL COMMENT '邮件内容',
  `gold` bigint(20) DEFAULT NULL COMMENT '邮件附件金币',
  `cash` int(11) unsigned DEFAULT NULL COMMENT '邮件附件现金',
  `exp` bigint(20) unsigned DEFAULT '0' COMMENT '经验',
  `vit` int(10) unsigned DEFAULT '0' COMMENT '体力',
  `attachment` varbinary(1024) DEFAULT NULL COMMENT '邮件附件',
  `getattachment` tinyint(4) DEFAULT NULL COMMENT '是否领取附件',
  `state` tinyint(4) DEFAULT NULL COMMENT '邮件状态 0.未读 1.已读',
  `tts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '邮件发送时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `email_idx_2` (`id`) USING BTREE,
  KEY `email_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `email_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of email
-- ----------------------------
INSERT INTO `email` VALUES ('1', '30849', '8', '0', '系统', '欢迎来到刀魂，加入官方群有惊喜', '刀魂官方QQ群311740622', '0', '0', '0', '0', 0x00, '1', '0', '2017-12-05 10:08:31');

-- ----------------------------
-- Table structure for friend
-- ----------------------------
DROP TABLE IF EXISTS `friend`;
CREATE TABLE `friend` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id,自动增长',
  `actorid` int(11) unsigned NOT NULL COMMENT '所属角色id',
  `friendid` int(11) unsigned NOT NULL COMMENT '好友id',
  `status` tinyint(4) DEFAULT '0' COMMENT '状态(0-未确认,1-确认)',
  `tts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '添加时间',
  PRIMARY KEY (`id`),
  KEY `friend_idx_1` (`actorid`) USING BTREE,
  KEY `friend_idx_2` (`friendid`) USING BTREE,
  CONSTRAINT `friend_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE,
  CONSTRAINT `friend_ibfk_2` FOREIGN KEY (`friendid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of friend
-- ----------------------------

-- ----------------------------
-- Table structure for goldexchange
-- ----------------------------
DROP TABLE IF EXISTS `goldexchange`;
CREATE TABLE `goldexchange` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `exchangetimes` smallint(6) unsigned DEFAULT NULL COMMENT '当天兑换次数',
  `tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后一次兑换时间',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `goldexchange_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `goldexchange_ifbk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of goldexchange
-- ----------------------------

-- ----------------------------
-- Table structure for guild
-- ----------------------------
DROP TABLE IF EXISTS `guild`;
CREATE TABLE `guild` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '公会id,自动增长',
  `name` char(33) NOT NULL COMMENT '公会名字',
  `notice` varchar(255) DEFAULT NULL COMMENT '公会公告',
  `level` tinyint(4) unsigned DEFAULT NULL COMMENT '公会等级',
  `exp` bigint(20) DEFAULT NULL COMMENT '公会经验',
  `founder` char(33) DEFAULT NULL COMMENT '公会创世人昵称',
  `tts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '创会时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of guild
-- ----------------------------

-- ----------------------------
-- Table structure for guildmember
-- ----------------------------
DROP TABLE IF EXISTS `guildmember`;
CREATE TABLE `guildmember` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id,自动增长',
  `guildid` int(11) unsigned NOT NULL COMMENT '所属公会id',
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `sex` enum('0','1') DEFAULT NULL COMMENT '角色性别(0,1)',
  `actornick` char(33) DEFAULT NULL COMMENT '角色昵称',
  `position` int(11) unsigned DEFAULT NULL COMMENT '公会所属职位',
  `credit` int(11) unsigned DEFAULT NULL COMMENT '角色对公会的贡献值',
  `jointts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '加入公会时间',
  PRIMARY KEY (`id`),
  KEY `guildmember_idx_1` (`guildid`) USING BTREE,
  KEY `guildmember_idx_2` (`actorid`) USING BTREE,
  CONSTRAINT `guildmember_ibfk_1` FOREIGN KEY (`guildid`) REFERENCES `guild` (`id`) ON DELETE CASCADE,
  CONSTRAINT `guildmember_ibfk_2` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of guildmember
-- ----------------------------

-- ----------------------------
-- Table structure for guildposition
-- ----------------------------
DROP TABLE IF EXISTS `guildposition`;
CREATE TABLE `guildposition` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id,自动增长',
  `guildid` int(11) unsigned NOT NULL COMMENT '所属公会id',
  `name` char(33) DEFAULT NULL COMMENT '职位名称',
  `chairman` tinyint(4) DEFAULT NULL COMMENT '是否是会长',
  `privilege` varchar(255) DEFAULT NULL COMMENT '职位权限',
  PRIMARY KEY (`id`),
  KEY `guildmember_idx_1` (`guildid`) USING BTREE,
  CONSTRAINT `guildposition_ibfk_1` FOREIGN KEY (`guildid`) REFERENCES `guild` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of guildposition
-- ----------------------------

-- ----------------------------
-- Table structure for helper
-- ----------------------------
DROP TABLE IF EXISTS `helper`;
CREATE TABLE `helper` (
  `actorid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '角色id',
  `helps` varbinary(1024) DEFAULT NULL COMMENT '已完成引导',
  `tts` timestamp NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最后一次完成引导时间',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `helper_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `helper_ifbk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of helper
-- ----------------------------

-- ----------------------------
-- Table structure for heyf_t10
-- ----------------------------
DROP TABLE IF EXISTS `heyf_t10`;
CREATE TABLE `heyf_t10` (
  `empid` int(11) DEFAULT NULL,
  `deptid` int(11) DEFAULT NULL,
  `salary` decimal(10,2) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of heyf_t10
-- ----------------------------

-- ----------------------------
-- Table structure for instancehistory
-- ----------------------------
DROP TABLE IF EXISTS `instancehistory`;
CREATE TABLE `instancehistory` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id,自动增长',
  `actorid` int(11) unsigned NOT NULL COMMENT '所属角色id',
  `instanceid` smallint(6) unsigned DEFAULT NULL COMMENT '副本id',
  `chapterid` smallint(5) unsigned DEFAULT '0' COMMENT '章节id',
  `star` tinyint(4) unsigned DEFAULT NULL COMMENT '通关评星',
  `startts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '通关评星时间',
  `complatednum` smallint(6) unsigned DEFAULT NULL COMMENT '挑战次数',
  `lasttts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后一次通关时间',
  `daynum` int(6) DEFAULT '0' COMMENT '该副本该角色每天挑战完成的次数',
  PRIMARY KEY (`id`),
  KEY `instancehistory_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `instancehistory_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of instancehistory
-- ----------------------------

-- ----------------------------
-- Table structure for joblock
-- ----------------------------
DROP TABLE IF EXISTS `joblock`;
CREATE TABLE `joblock` (
  `accountid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '账号id',
  `jobs` varbinary(512) DEFAULT NULL COMMENT '解锁职业',
  `tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后一次解锁时间',
  PRIMARY KEY (`accountid`),
  UNIQUE KEY `lockactor_idx_1` (`accountid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of joblock
-- ----------------------------

-- ----------------------------
-- Table structure for ladder
-- ----------------------------
DROP TABLE IF EXISTS `ladder`;
CREATE TABLE `ladder` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `accountid` int(11) unsigned NOT NULL COMMENT '所属账号id',
  `win_in_row` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '连胜次数',
  `refresh_count` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '刷新次数',
  `eliminate_count` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT 'CD消除次数',
  `challenge_count` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '挑战次数',
  `last_finish_time` timestamp NULL DEFAULT NULL COMMENT '上次完成挑战的时间',
  `user_data` varbinary(64) DEFAULT NULL COMMENT '用户数据',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `ladder_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `ladder_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of ladder
-- ----------------------------

-- ----------------------------
-- Table structure for ladder_rank
-- ----------------------------
DROP TABLE IF EXISTS `ladder_rank`;
CREATE TABLE `ladder_rank` (
  `rank` int(4) unsigned NOT NULL AUTO_INCREMENT COMMENT '排名',
  `actorid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '角色id',
  `robot` smallint(1) unsigned NOT NULL COMMENT '是否是机器人',
  `actortype` smallint(6) unsigned DEFAULT NULL COMMENT '角色类型',
  `cp` int(11) unsigned DEFAULT NULL COMMENT '战力',
  `level` tinyint(4) unsigned DEFAULT NULL COMMENT '角色等级',
  PRIMARY KEY (`rank`,`actorid`),
  UNIQUE KEY `ladder_rank_idx_1` (`rank`) USING BTREE,
  KEY `ladder_rank_idx_2` (`actorid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of ladder_rank
-- ----------------------------

-- ----------------------------
-- Table structure for limitbuy
-- ----------------------------
DROP TABLE IF EXISTS `limitbuy`;
CREATE TABLE `limitbuy` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id',
  `actorid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '角色id',
  `goodsid` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT '货物id',
  `status` tinyint(4) unsigned DEFAULT '0' COMMENT '当天是否购买(0-否，1-是)',
  `tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后一次购买时间',
  PRIMARY KEY (`id`),
  KEY `limitbuy_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `limitbuy_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of limitbuy
-- ----------------------------

-- ----------------------------
-- Table structure for messagetips
-- ----------------------------
DROP TABLE IF EXISTS `messagetips`;
CREATE TABLE `messagetips` (
  `actorid` int(11) NOT NULL,
  `type` int(11) DEFAULT NULL,
  `title` varchar(255) DEFAULT NULL,
  `content` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`actorid`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of messagetips
-- ----------------------------

-- ----------------------------
-- Table structure for online_reward
-- ----------------------------
DROP TABLE IF EXISTS `online_reward`;
CREATE TABLE `online_reward` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '唯一ID',
  `actorid` int(11) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `date` date NOT NULL COMMENT '日期',
  `online` int(11) NOT NULL DEFAULT '0' COMMENT '在线时间',
  `info` varbinary(255) DEFAULT NULL COMMENT '领取信息，记录领取过的ID',
  `lasttts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后进入游戏时间，如果晚上0点整在线，也会有一条记录',
  PRIMARY KEY (`id`),
  KEY `actorid_idx` (`actorid`) USING BTREE,
  KEY `date_idx` (`date`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=63 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of online_reward
-- ----------------------------
INSERT INTO `online_reward` VALUES ('1', '1', '2017-08-24', '3954', null, '2017-08-24 14:57:53');
INSERT INTO `online_reward` VALUES ('2', '1', '2017-08-25', '37176', null, '2017-08-25 17:09:08');
INSERT INTO `online_reward` VALUES ('3', '1', '2017-08-26', '26', null, '2017-08-26 17:15:30');
INSERT INTO `online_reward` VALUES ('4', '1', '2017-08-28', '18676', null, '2017-08-28 14:06:45');
INSERT INTO `online_reward` VALUES ('5', '1', '2017-08-29', '6158', null, '2017-08-29 16:30:32');
INSERT INTO `online_reward` VALUES ('6', '1', '2017-08-31', '70', null, '2017-08-31 17:35:25');
INSERT INTO `online_reward` VALUES ('7', '1', '2017-09-01', '68', null, '2017-09-01 11:46:47');
INSERT INTO `online_reward` VALUES ('8', '116932180', '2017-09-13', '0', null, '2017-09-13 11:54:53');
INSERT INTO `online_reward` VALUES ('9', '111991989', '2017-09-14', '110', null, '2017-09-14 17:23:14');
INSERT INTO `online_reward` VALUES ('10', '111991989', '2017-09-15', '378', null, '2017-09-15 17:32:44');
INSERT INTO `online_reward` VALUES ('11', '116932180', '2017-09-15', '28', null, '2017-09-15 11:42:25');
INSERT INTO `online_reward` VALUES ('12', '116932180', '2017-09-18', '54', null, '2017-09-18 10:45:31');
INSERT INTO `online_reward` VALUES ('13', '111991989', '2017-09-20', '580', null, '2017-09-20 16:31:32');
INSERT INTO `online_reward` VALUES ('14', '111991989', '2017-09-23', '19476', null, '2017-09-23 15:19:04');
INSERT INTO `online_reward` VALUES ('15', '116932180', '2017-09-23', '346', null, '2017-09-23 17:31:44');
INSERT INTO `online_reward` VALUES ('16', '116932180', '2017-09-25', '390', null, '2017-09-25 16:30:26');
INSERT INTO `online_reward` VALUES ('17', '116932180', '2017-09-26', '330', null, '2017-09-26 17:10:25');
INSERT INTO `online_reward` VALUES ('18', '116932180', '2017-09-27', '910', null, '2017-09-27 17:34:30');
INSERT INTO `online_reward` VALUES ('19', '116932180', '2017-09-28', '3962', null, '2017-09-28 17:35:24');
INSERT INTO `online_reward` VALUES ('20', '116932180', '2017-09-29', '12036', null, '2017-09-29 17:25:37');
INSERT INTO `online_reward` VALUES ('21', '116932180', '2017-09-30', '30550', null, '2017-09-30 16:07:50');
INSERT INTO `online_reward` VALUES ('22', '116932180', '2017-10-09', '8828', null, '2017-10-09 17:33:43');
INSERT INTO `online_reward` VALUES ('23', '116932180', '2017-10-10', '2930', null, '2017-10-10 17:02:55');
INSERT INTO `online_reward` VALUES ('24', '116932180', '2017-10-11', '2649', null, '2017-10-11 17:38:36');
INSERT INTO `online_reward` VALUES ('25', '116932180', '2017-10-12', '9144', null, '2017-10-12 17:42:03');
INSERT INTO `online_reward` VALUES ('26', '116932180', '2017-10-13', '3390', null, '2017-10-13 17:36:38');
INSERT INTO `online_reward` VALUES ('27', '116932180', '2017-10-16', '2322', null, '2017-10-16 15:50:05');
INSERT INTO `online_reward` VALUES ('28', '116932180', '2017-10-18', '1936', null, '2017-10-18 19:55:09');
INSERT INTO `online_reward` VALUES ('29', '116932180', '2017-10-19', '8219', null, '2017-10-19 16:10:29');
INSERT INTO `online_reward` VALUES ('30', '116932180', '2017-10-20', '36748', null, '2017-10-20 22:27:26');
INSERT INTO `online_reward` VALUES ('31', '116932180', '2017-10-21', '3736', null, '2017-10-21 17:31:59');
INSERT INTO `online_reward` VALUES ('32', '116932180', '2017-10-23', '3290', null, '2017-10-23 14:15:56');
INSERT INTO `online_reward` VALUES ('33', '116932180', '2017-10-24', '7414', null, '2017-10-24 17:25:40');
INSERT INTO `online_reward` VALUES ('34', '116932180', '2017-10-26', '186', null, '2017-10-26 10:04:45');
INSERT INTO `online_reward` VALUES ('35', '1880', '2017-10-26', '1494', null, '2017-10-26 14:13:51');
INSERT INTO `online_reward` VALUES ('36', '1880', '2017-11-02', '5380', null, '2017-11-02 17:35:31');
INSERT INTO `online_reward` VALUES ('37', '1880', '2017-11-03', '998', null, '2017-11-03 10:53:31');
INSERT INTO `online_reward` VALUES ('38', '116932180', '2017-11-03', '606', null, '2017-11-03 17:32:59');
INSERT INTO `online_reward` VALUES ('39', '116932180', '2017-11-07', '486', null, '2017-11-07 16:18:26');
INSERT INTO `online_reward` VALUES ('40', '116932180', '2017-11-08', '454', null, '2017-11-08 14:42:45');
INSERT INTO `online_reward` VALUES ('41', '116932180', '2017-11-15', '18729', null, '2017-11-15 14:20:30');
INSERT INTO `online_reward` VALUES ('42', '1880', '2017-11-15', '18520', null, '2017-11-15 14:20:27');
INSERT INTO `online_reward` VALUES ('43', '1880', '2017-11-17', '1036', null, '2017-11-17 18:37:22');
INSERT INTO `online_reward` VALUES ('44', '116932180', '2017-11-17', '1296', null, '2017-11-17 18:37:24');
INSERT INTO `online_reward` VALUES ('45', '116932180', '2017-11-18', '7466', null, '2017-11-18 16:06:12');
INSERT INTO `online_reward` VALUES ('46', '1880', '2017-11-18', '7262', null, '2017-11-18 16:13:44');
INSERT INTO `online_reward` VALUES ('47', '1762', '2017-11-18', '886', null, '2017-11-18 16:13:44');
INSERT INTO `online_reward` VALUES ('48', '126732690', '2017-11-18', '254', null, '2017-11-18 16:06:11');
INSERT INTO `online_reward` VALUES ('49', '1880', '2017-11-21', '6826', null, '2017-11-21 10:33:07');
INSERT INTO `online_reward` VALUES ('50', '116932180', '2017-11-21', '6558', null, '2017-11-21 10:33:07');
INSERT INTO `online_reward` VALUES ('51', '1762', '2017-11-21', '3872', null, '2017-11-21 10:33:07');
INSERT INTO `online_reward` VALUES ('52', '126732690', '2017-11-21', '168', null, '2017-11-21 10:33:07');
INSERT INTO `online_reward` VALUES ('53', '126732690', '2017-11-22', '282', null, '2017-11-22 16:57:27');
INSERT INTO `online_reward` VALUES ('54', '1762', '2017-11-22', '124', null, '2017-11-22 16:57:26');
INSERT INTO `online_reward` VALUES ('55', '126732690', '2017-11-28', '236', null, '2017-11-28 11:16:18');
INSERT INTO `online_reward` VALUES ('56', '1880', '2017-11-28', '438', null, '2017-11-28 11:14:03');
INSERT INTO `online_reward` VALUES ('57', '126732690', '2017-12-04', '1104', null, '2017-12-04 15:43:34');
INSERT INTO `online_reward` VALUES ('58', '116932180', '2017-12-04', '5626', null, '2017-12-04 15:56:09');
INSERT INTO `online_reward` VALUES ('59', '1880', '2017-12-04', '492', null, '2017-12-04 15:56:03');
INSERT INTO `online_reward` VALUES ('60', '116932180', '2017-12-05', '496', null, '2017-12-05 09:42:38');
INSERT INTO `online_reward` VALUES ('61', '1880', '2017-12-05', '414', null, '2017-12-05 09:42:33');
INSERT INTO `online_reward` VALUES ('62', '30849', '2017-12-05', '32', null, '2017-12-05 10:08:48');

-- ----------------------------
-- Table structure for permanent
-- ----------------------------
DROP TABLE IF EXISTS `permanent`;
CREATE TABLE `permanent` (
  `actorid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '角色id',
  `helps` varbinary(512) DEFAULT NULL COMMENT '新手引导',
  `magics` varbinary(512) DEFAULT NULL COMMENT '魔导器',
  `help_tts` timestamp NULL DEFAULT NULL COMMENT '最后更新新手引导时间',
  `magic_tts` timestamp NULL DEFAULT NULL COMMENT '最后更新魔导器时间',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `permanent_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `permanent_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of permanent
-- ----------------------------
INSERT INTO `permanent` VALUES ('1762', 0x00, 0x00, '2017-11-18 16:00:50', '2017-11-18 16:00:50');
INSERT INTO `permanent` VALUES ('1880', 0x00, 0x00, '2017-10-26 10:04:23', '2017-10-26 10:04:23');
INSERT INTO `permanent` VALUES ('30849', 0x00, 0x00, '2017-12-05 10:08:30', '2017-12-05 10:08:30');
INSERT INTO `permanent` VALUES ('74189477', 0x00, 0x00, '2017-09-01 16:36:26', '2017-09-01 16:36:26');
INSERT INTO `permanent` VALUES ('111991989', 0x00, 0x00, '2017-09-14 11:36:05', '2017-09-14 11:36:05');
INSERT INTO `permanent` VALUES ('116932180', 0x00, 0x00, '2017-09-01 16:33:43', '2017-09-01 16:33:43');
INSERT INTO `permanent` VALUES ('126732690', 0x00, 0x00, '2017-11-18 16:03:45', '2017-11-18 16:03:45');

-- ----------------------------
-- Table structure for personaltimertask
-- ----------------------------
DROP TABLE IF EXISTS `personaltimertask`;
CREATE TABLE `personaltimertask` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '定时任务记录id',
  `taskid` smallint(6) unsigned NOT NULL COMMENT '任务编号',
  `actorid` int(11) unsigned NOT NULL COMMENT '所属角色id',
  `starttime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '开始时间',
  PRIMARY KEY (`id`),
  KEY `personaltimertask_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `personaltimertask_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of personaltimertask
-- ----------------------------

-- ----------------------------
-- Table structure for randactivity
-- ----------------------------
DROP TABLE IF EXISTS `randactivity`;
CREATE TABLE `randactivity` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `activity` varbinary(64) DEFAULT NULL COMMENT '拥有的活动副本信息',
  `last_send_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '上次发放时间',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `randactivity_idx` (`actorid`) USING BTREE,
  CONSTRAINT `randactivity_ibfk` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of randactivity
-- ----------------------------

-- ----------------------------
-- Table structure for rankcomp
-- ----------------------------
DROP TABLE IF EXISTS `rankcomp`;
CREATE TABLE `rankcomp` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `rank` int(11) NOT NULL DEFAULT '0' COMMENT '排名',
  `ranktts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '排名时间',
  `accountid` int(11) NOT NULL DEFAULT '0' COMMENT '帐号ID',
  `actorid` int(11) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `nick` varchar(33) NOT NULL DEFAULT '' COMMENT '角色名',
  `competitive` int(11) NOT NULL DEFAULT '0' COMMENT '竞技模式',
  `award` smallint(6) DEFAULT '0' COMMENT '奖励领取',
  `msign` smallint(6) DEFAULT '0' COMMENT '主排行标记',
  `actortype` smallint(6) DEFAULT NULL COMMENT '职业类型',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of rankcomp
-- ----------------------------

-- ----------------------------
-- Table structure for rankcp
-- ----------------------------
DROP TABLE IF EXISTS `rankcp`;
CREATE TABLE `rankcp` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `rank` int(11) NOT NULL DEFAULT '0' COMMENT '排名',
  `ranktts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '排名时间',
  `actorid` int(11) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `nick` varchar(33) NOT NULL DEFAULT '' COMMENT '角色名',
  `cp` int(11) NOT NULL DEFAULT '0' COMMENT '战斗力',
  `award` smallint(6) DEFAULT '0' COMMENT '奖励领取',
  `msign` smallint(6) DEFAULT '0' COMMENT '主排行标记',
  `actortype` smallint(6) DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of rankcp
-- ----------------------------

-- ----------------------------
-- Table structure for rankinst
-- ----------------------------
DROP TABLE IF EXISTS `rankinst`;
CREATE TABLE `rankinst` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `rank` int(11) NOT NULL DEFAULT '0' COMMENT '排名',
  `ranktts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '排名时间',
  `actorid` int(11) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `nick` varchar(33) NOT NULL DEFAULT '' COMMENT '角色名',
  `instid` int(11) NOT NULL DEFAULT '0' COMMENT '角色副本',
  `award` smallint(6) DEFAULT '0' COMMENT '奖励领取',
  `msign` smallint(6) DEFAULT '0' COMMENT '主排行标记',
  `actortype` smallint(6) DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of rankinst
-- ----------------------------

-- ----------------------------
-- Table structure for rankinstance
-- ----------------------------
DROP TABLE IF EXISTS `rankinstance`;
CREATE TABLE `rankinstance` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '纪录id',
  `instid` smallint(6) unsigned DEFAULT NULL COMMENT '完成副本id',
  `actorid` int(11) unsigned DEFAULT NULL COMMENT '角色id',
  `accountid` int(11) unsigned DEFAULT '0' COMMENT '账号id',
  `nick` char(33) DEFAULT NULL COMMENT '角色昵称',
  `score` int(11) unsigned DEFAULT NULL COMMENT '评分',
  `gamemode` tinyint(4) unsigned DEFAULT NULL COMMENT '游戏模式(1-挑战,2-探索,3...)',
  `tts` timestamp NULL DEFAULT NULL COMMENT '完成副本时间',
  PRIMARY KEY (`id`),
  KEY `rankinstance_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `rankinstance_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of rankinstance
-- ----------------------------

-- ----------------------------
-- Table structure for rankinstnum
-- ----------------------------
DROP TABLE IF EXISTS `rankinstnum`;
CREATE TABLE `rankinstnum` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `rank` int(11) NOT NULL DEFAULT '0' COMMENT '排名',
  `ranktts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '排名时间',
  `actorid` int(11) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `nick` varchar(33) NOT NULL DEFAULT '' COMMENT '角色名',
  `instnum` int(11) NOT NULL DEFAULT '0' COMMENT '角色副本',
  `award` smallint(6) DEFAULT '0' COMMENT '奖励领取',
  `msign` smallint(6) NOT NULL DEFAULT '0' COMMENT '主排行标记',
  `actortype` tinyint(4) DEFAULT NULL COMMENT '职业类型',
  PRIMARY KEY (`id`),
  KEY `rank_idx` (`rank`) USING BTREE,
  KEY `ranktts_idx` (`ranktts`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of rankinstnum
-- ----------------------------

-- ----------------------------
-- Table structure for ranklevel
-- ----------------------------
DROP TABLE IF EXISTS `ranklevel`;
CREATE TABLE `ranklevel` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `rank` int(11) NOT NULL DEFAULT '0' COMMENT '排名',
  `ranktts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '排名时间',
  `actorid` int(11) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `nick` varchar(33) NOT NULL DEFAULT '' COMMENT '角色名',
  `level` tinyint(4) NOT NULL DEFAULT '0' COMMENT '等级',
  `award` smallint(6) DEFAULT '0' COMMENT '奖励领取',
  `msign` smallint(6) DEFAULT '0' COMMENT '主排行标记',
  `actortype` smallint(6) DEFAULT '0' COMMENT '职业类型',
  PRIMARY KEY (`id`),
  KEY `msign_idx` (`msign`) USING BTREE,
  KEY `ranktts_idx` (`ranktts`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of ranklevel
-- ----------------------------

-- ----------------------------
-- Table structure for rankpoint
-- ----------------------------
DROP TABLE IF EXISTS `rankpoint`;
CREATE TABLE `rankpoint` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `rank` int(11) NOT NULL DEFAULT '0' COMMENT '排名',
  `ranktts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '排名时间',
  `actorid` int(11) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `nick` varchar(33) NOT NULL DEFAULT '' COMMENT '角色名',
  `point` int(11) NOT NULL DEFAULT '0' COMMENT '成就点数',
  `award` smallint(6) DEFAULT '0' COMMENT '奖励领取',
  `msign` smallint(6) DEFAULT '0' COMMENT '主排行标记',
  `actortype` smallint(6) DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of rankpoint
-- ----------------------------

-- ----------------------------
-- Table structure for recharge
-- ----------------------------
DROP TABLE IF EXISTS `recharge`;
CREATE TABLE `recharge` (
  `orderid` varchar(65) NOT NULL,
  `partid` int(11) NOT NULL DEFAULT '0',
  `channelid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '平台id',
  `accountid` int(11) NOT NULL DEFAULT '0',
  `actorid` int(11) NOT NULL DEFAULT '0',
  `rechargeid` int(11) NOT NULL DEFAULT '0',
  `rmb` int(11) NOT NULL DEFAULT '0',
  `token` int(11) NOT NULL DEFAULT '0',
  `give` int(11) NOT NULL DEFAULT '0',
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `ordertts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `isgettoken` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '是否领取代币(0-未领取， 1-已经领取)',
  `oktts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`orderid`),
  UNIQUE KEY `recharge_idx_1` (`orderid`) USING BTREE,
  KEY `recharge_idx_2` (`actorid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of recharge
-- ----------------------------

-- ----------------------------
-- Table structure for roulette
-- ----------------------------
DROP TABLE IF EXISTS `roulette`;
CREATE TABLE `roulette` (
  `actorid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '角色id',
  `times` smallint(5) unsigned DEFAULT '0' COMMENT '抽取次数',
  `props` varbinary(1024) DEFAULT NULL COMMENT '轮盘奖品',
  `tts` timestamp NULL DEFAULT NULL COMMENT '最后一次抽取时间',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `roulette_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `roulette_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of roulette
-- ----------------------------

-- ----------------------------
-- Table structure for sellhistory
-- ----------------------------
DROP TABLE IF EXISTS `sellhistory`;
CREATE TABLE `sellhistory` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id,自动增长',
  `actorid` int(11) unsigned NOT NULL COMMENT '所属角色id',
  `sells` varbinary(10301) DEFAULT NULL COMMENT '出售物品',
  PRIMARY KEY (`id`),
  KEY `sellhistory_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `sellhostory_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of sellhistory
-- ----------------------------

-- ----------------------------
-- Table structure for signin
-- ----------------------------
DROP TABLE IF EXISTS `signin`;
CREATE TABLE `signin` (
  `actorid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '角色id',
  `signs` varbinary(1024) DEFAULT NULL COMMENT '签到记录',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `signin_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `signin_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of signin
-- ----------------------------

-- ----------------------------
-- Table structure for snarehistory
-- ----------------------------
DROP TABLE IF EXISTS `snarehistory`;
CREATE TABLE `snarehistory` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id,自动增长',
  `actorid` int(11) unsigned NOT NULL COMMENT '所属角色id',
  `snareid` smallint(6) unsigned NOT NULL COMMENT '机关id',
  `state` tinyint(4) DEFAULT NULL COMMENT '机关状态',
  `lasttts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '机关最后一次修改时间',
  PRIMARY KEY (`id`),
  KEY `snarehistory_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `snarehistory_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of snarehistory
-- ----------------------------

-- ----------------------------
-- Table structure for steprank
-- ----------------------------
DROP TABLE IF EXISTS `steprank`;
CREATE TABLE `steprank` (
  `rank` int(11) unsigned NOT NULL COMMENT '名次',
  `actorid` int(11) unsigned NOT NULL COMMENT '所属角色id',
  `sex` enum('0','1') DEFAULT NULL COMMENT '角色性别(0,1)',
  `actornick` char(33) DEFAULT NULL COMMENT '角色昵称',
  `oldrank` binary(17) DEFAULT NULL COMMENT '前4次的名次信息',
  `todaynum` smallint(6) unsigned DEFAULT NULL COMMENT '今日挑战次数',
  `joinrank` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '加入天梯时间',
  PRIMARY KEY (`rank`),
  KEY `steprank_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `steprank_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of steprank
-- ----------------------------

-- ----------------------------
-- Table structure for taskhistory
-- ----------------------------
DROP TABLE IF EXISTS `taskhistory`;
CREATE TABLE `taskhistory` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id,自动增长',
  `actorid` int(11) unsigned NOT NULL COMMENT '所属角色id',
  `taskid` smallint(6) unsigned NOT NULL COMMENT '任务id',
  `type` tinyint(4) unsigned NOT NULL COMMENT '任务类型',
  `chapterid` smallint(6) unsigned NOT NULL COMMENT '所属章节id',
  `tts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '完成任务时间',
  PRIMARY KEY (`id`),
  KEY `taskhistory_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `taskhistory_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of taskhistory
-- ----------------------------

-- ----------------------------
-- Table structure for vip
-- ----------------------------
DROP TABLE IF EXISTS `vip`;
CREATE TABLE `vip` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id',
  `actorid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '角色id',
  `vip` tinyint(4) DEFAULT '0' COMMENT 'VIP等级',
  `reward` tinyint(4) DEFAULT '0' COMMENT 'VIP等级对应奖励是否领取(0-未领取, 1-领取)',
  `tts` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '领取奖励时间',
  PRIMARY KEY (`id`),
  KEY `vip_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `vip_ifbk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of vip
-- ----------------------------

-- ----------------------------
-- Table structure for vitexchange
-- ----------------------------
DROP TABLE IF EXISTS `vitexchange`;
CREATE TABLE `vitexchange` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `exchangetimes` smallint(6) unsigned DEFAULT NULL COMMENT '当天兑换次数',
  `tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后一次兑换时间',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `vitexchange_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `vitexchange_ifbk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of vitexchange
-- ----------------------------
