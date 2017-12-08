/*
Navicat MySQL Data Transfer

Source Server         : root
Source Server Version : 50515
Source Host           : localhost:3306
Source Database       : sgsrecharge

Target Server Type    : MYSQL
Target Server Version : 50515
File Encoding         : 65001

Date: 2017-12-08 11:18:56
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for activated_device1
-- ----------------------------
DROP TABLE IF EXISTS `activated_device1`;
CREATE TABLE `activated_device1` (
  `dev_idfa` varchar(41) NOT NULL COMMENT '设备号IDFA',
  `dev_type` varchar(32) DEFAULT NULL COMMENT '机型',
  `system` varchar(16) DEFAULT NULL COMMENT '系统',
  `createtts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '时间',
  `dev_ip` varchar(16) NOT NULL COMMENT '设备IP',
  `region` varchar(40) DEFAULT NULL COMMENT '区域',
  PRIMARY KEY (`dev_idfa`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of activated_device1
-- ----------------------------

-- ----------------------------
-- Table structure for activated_device2
-- ----------------------------
DROP TABLE IF EXISTS `activated_device2`;
CREATE TABLE `activated_device2` (
  `dev_idfa` varchar(41) NOT NULL COMMENT '设备IDFA',
  `dev_type` varchar(32) DEFAULT NULL COMMENT '机型',
  `system` varchar(16) DEFAULT NULL COMMENT '系统',
  `createtts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '时间',
  `dev_ip` varchar(16) NOT NULL COMMENT '设备IP',
  `region` varchar(40) DEFAULT NULL COMMENT '区域',
  PRIMARY KEY (`dev_idfa`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of activated_device2
-- ----------------------------

-- ----------------------------
-- Table structure for activated_device3
-- ----------------------------
DROP TABLE IF EXISTS `activated_device3`;
CREATE TABLE `activated_device3` (
  `dev_idfa` varchar(41) NOT NULL COMMENT '设备IDFA',
  `dev_type` varchar(32) DEFAULT NULL COMMENT '机型',
  `system` varchar(16) DEFAULT NULL COMMENT '系统',
  `createtts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '时间',
  `dev_ip` varchar(16) NOT NULL COMMENT '设备IP',
  `region` varchar(40) DEFAULT NULL COMMENT '区域',
  PRIMARY KEY (`dev_idfa`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of activated_device3
-- ----------------------------

-- ----------------------------
-- Table structure for activated_device4
-- ----------------------------
DROP TABLE IF EXISTS `activated_device4`;
CREATE TABLE `activated_device4` (
  `dev_idfa` varchar(41) NOT NULL COMMENT '设备IDFA',
  `dev_type` varchar(32) DEFAULT NULL COMMENT '机型',
  `system` varchar(16) DEFAULT NULL COMMENT '系统',
  `createtts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '时间',
  `dev_ip` varchar(16) NOT NULL COMMENT '设备IP',
  `region` varchar(40) DEFAULT NULL COMMENT '区域',
  PRIMARY KEY (`dev_idfa`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of activated_device4
-- ----------------------------

-- ----------------------------
-- Table structure for activated_device5
-- ----------------------------
DROP TABLE IF EXISTS `activated_device5`;
CREATE TABLE `activated_device5` (
  `dev_idfa` varchar(41) NOT NULL COMMENT '设备IDFA',
  `dev_type` varchar(32) DEFAULT NULL COMMENT '机型',
  `system` varchar(16) DEFAULT NULL COMMENT '系统',
  `createtts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '时间',
  `dev_ip` varchar(16) NOT NULL COMMENT '设备IP',
  `region` varchar(40) DEFAULT NULL COMMENT '区域',
  PRIMARY KEY (`dev_idfa`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of activated_device5
-- ----------------------------

-- ----------------------------
-- Table structure for player_cash_1
-- ----------------------------
DROP TABLE IF EXISTS `player_cash_1`;
CREATE TABLE `player_cash_1` (
  `orderid` char(65) NOT NULL DEFAULT '' COMMENT '原始订单号',
  `accountid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帐号ID',
  `actorid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '角色ID',
  `platformid` varchar(65) NOT NULL DEFAULT '0' COMMENT '平台ID',
  `createtts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '生成订单号时间',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '当前订单状态',
  `rechargeid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '充值ID',
  `rmb` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '额度',
  `confirmtts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '确认时间',
  `partid` smallint(6) unsigned NOT NULL DEFAULT '0',
  `platformorderid` char(65) DEFAULT NULL COMMENT '平台订单ID',
  PRIMARY KEY (`orderid`),
  UNIQUE KEY `player_cash_4_idx_1` (`orderid`) USING BTREE,
  KEY `player_cash_4_idx_2` (`platformid`) USING BTREE,
  KEY `player_cash_4_idx_3` (`partid`) USING BTREE,
  KEY `player_cash_4_idx_4` (`confirmtts`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of player_cash_1
-- ----------------------------

-- ----------------------------
-- Table structure for player_cash_2
-- ----------------------------
DROP TABLE IF EXISTS `player_cash_2`;
CREATE TABLE `player_cash_2` (
  `orderid` char(65) NOT NULL DEFAULT '' COMMENT '原始订单号',
  `accountid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帐号ID',
  `actorid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '角色ID',
  `platformid` varchar(65) NOT NULL DEFAULT '0' COMMENT '平台ID',
  `createtts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '生成订单号时间',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '当前订单状态',
  `rechargeid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '充值ID',
  `rmb` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '额度',
  `confirmtts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '确认时间',
  `partid` smallint(6) unsigned NOT NULL DEFAULT '0',
  `platformorderid` char(65) DEFAULT NULL COMMENT '平台订单ID',
  PRIMARY KEY (`orderid`),
  UNIQUE KEY `player_cash_4_idx_1` (`orderid`) USING BTREE,
  KEY `player_cash_4_idx_2` (`platformid`) USING BTREE,
  KEY `player_cash_4_idx_3` (`partid`) USING BTREE,
  KEY `player_cash_4_idx_4` (`confirmtts`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of player_cash_2
-- ----------------------------

-- ----------------------------
-- Table structure for player_cash_3
-- ----------------------------
DROP TABLE IF EXISTS `player_cash_3`;
CREATE TABLE `player_cash_3` (
  `orderid` char(65) NOT NULL DEFAULT '' COMMENT '原始订单号',
  `accountid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帐号ID',
  `actorid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '角色ID',
  `platformid` varchar(65) NOT NULL DEFAULT '0' COMMENT '平台ID',
  `createtts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '生成订单号时间',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '当前订单状态',
  `rechargeid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '充值ID',
  `rmb` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '额度',
  `confirmtts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '确认时间',
  `partid` smallint(6) unsigned NOT NULL DEFAULT '0',
  `platformorderid` char(65) DEFAULT NULL COMMENT '平台订单ID',
  PRIMARY KEY (`orderid`),
  UNIQUE KEY `player_cash_4_idx_1` (`orderid`) USING BTREE,
  KEY `player_cash_4_idx_2` (`platformid`) USING BTREE,
  KEY `player_cash_4_idx_3` (`partid`) USING BTREE,
  KEY `player_cash_4_idx_4` (`confirmtts`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of player_cash_3
-- ----------------------------

-- ----------------------------
-- Table structure for player_cash_4
-- ----------------------------
DROP TABLE IF EXISTS `player_cash_4`;
CREATE TABLE `player_cash_4` (
  `orderid` char(65) NOT NULL DEFAULT '' COMMENT '原始订单号',
  `accountid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帐号ID',
  `actorid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '角色ID',
  `platformid` varchar(65) NOT NULL DEFAULT '0' COMMENT '平台ID',
  `createtts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '生成订单号时间',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '当前订单状态',
  `rechargeid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '充值ID',
  `rmb` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '额度',
  `confirmtts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '确认时间',
  `partid` smallint(6) unsigned NOT NULL DEFAULT '0',
  `platformorderid` char(65) DEFAULT NULL COMMENT '平台订单ID',
  PRIMARY KEY (`orderid`),
  UNIQUE KEY `player_cash_4_idx_1` (`orderid`) USING BTREE,
  KEY `player_cash_4_idx_2` (`platformid`) USING BTREE,
  KEY `player_cash_4_idx_3` (`partid`) USING BTREE,
  KEY `player_cash_4_idx_4` (`confirmtts`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of player_cash_4
-- ----------------------------

-- ----------------------------
-- Table structure for player_cash_5
-- ----------------------------
DROP TABLE IF EXISTS `player_cash_5`;
CREATE TABLE `player_cash_5` (
  `orderid` char(65) NOT NULL DEFAULT '' COMMENT '原始订单号',
  `accountid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帐号ID',
  `actorid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '角色ID',
  `platformid` varchar(65) NOT NULL DEFAULT '0' COMMENT '平台ID',
  `createtts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '生成订单号时间',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '当前订单状态',
  `rechargeid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '充值ID',
  `rmb` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '额度',
  `confirmtts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '确认时间',
  `partid` smallint(6) unsigned NOT NULL DEFAULT '0',
  `platformorderid` char(65) DEFAULT NULL COMMENT '平台订单ID',
  PRIMARY KEY (`orderid`),
  UNIQUE KEY `player_cash_4_idx_1` (`orderid`) USING BTREE,
  KEY `player_cash_4_idx_2` (`platformid`) USING BTREE,
  KEY `player_cash_4_idx_3` (`partid`) USING BTREE,
  KEY `player_cash_4_idx_4` (`confirmtts`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of player_cash_5
-- ----------------------------
