/*
Navicat MySQL Data Transfer

Source Server         : root
Source Server Version : 50515
Source Host           : localhost:3306
Source Database       : sgscash

Target Server Type    : MYSQL
Target Server Version : 50515
File Encoding         : 65001

Date: 2017-12-08 11:18:41
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for cashlist_1
-- ----------------------------
DROP TABLE IF EXISTS `cashlist_1`;
CREATE TABLE `cashlist_1` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `actornick` char(33) NOT NULL COMMENT '角色昵称',
  `cashid` smallint(6) unsigned NOT NULL COMMENT '充值id',
  `rmb` int(11) unsigned DEFAULT '0' COMMENT '人民币',
  `token` int(11) unsigned DEFAULT '0' COMMENT '代币',
  `give` int(11) unsigned DEFAULT '0' COMMENT '首充赠送',
  `tts` timestamp NULL DEFAULT NULL COMMENT '充值时间',
  `accountid` int(11) NOT NULL COMMENT '账号ID',
  `areaid` smallint(6) NOT NULL DEFAULT '0' COMMENT '角色所在分区的ID',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';

-- ----------------------------
-- Records of cashlist_1
-- ----------------------------

-- ----------------------------
-- Table structure for cashlist_2
-- ----------------------------
DROP TABLE IF EXISTS `cashlist_2`;
CREATE TABLE `cashlist_2` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `actornick` char(33) NOT NULL COMMENT '角色昵称',
  `cashid` smallint(6) unsigned NOT NULL COMMENT '充值id',
  `rmb` int(11) unsigned DEFAULT '0' COMMENT '人民币',
  `token` int(11) unsigned DEFAULT '0' COMMENT '代币',
  `give` int(11) unsigned DEFAULT '0' COMMENT '首充赠送',
  `tts` timestamp NULL DEFAULT NULL COMMENT '充值时间',
  `accountid` int(11) NOT NULL COMMENT '账号ID',
  `areaid` smallint(6) NOT NULL DEFAULT '0' COMMENT '角色所在分区的ID',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';

-- ----------------------------
-- Records of cashlist_2
-- ----------------------------

-- ----------------------------
-- Table structure for cashlist_3
-- ----------------------------
DROP TABLE IF EXISTS `cashlist_3`;
CREATE TABLE `cashlist_3` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `actornick` char(33) NOT NULL COMMENT '角色昵称',
  `cashid` smallint(6) unsigned NOT NULL COMMENT '充值id',
  `rmb` int(11) unsigned DEFAULT '0' COMMENT '人民币',
  `token` int(11) unsigned DEFAULT '0' COMMENT '代币',
  `give` int(11) unsigned DEFAULT '0' COMMENT '首充赠送',
  `tts` timestamp NULL DEFAULT NULL COMMENT '充值时间',
  `accountid` int(11) NOT NULL COMMENT '账号ID',
  `areaid` smallint(6) NOT NULL DEFAULT '0' COMMENT '角色所在分区的ID',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';

-- ----------------------------
-- Records of cashlist_3
-- ----------------------------

-- ----------------------------
-- Table structure for cashlist_4
-- ----------------------------
DROP TABLE IF EXISTS `cashlist_4`;
CREATE TABLE `cashlist_4` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `actornick` char(33) NOT NULL COMMENT '角色昵称',
  `cashid` smallint(6) unsigned NOT NULL COMMENT '充值id',
  `rmb` int(11) unsigned DEFAULT '0' COMMENT '人民币',
  `token` int(11) unsigned DEFAULT '0' COMMENT '代币',
  `give` int(11) unsigned DEFAULT '0' COMMENT '首充赠送',
  `tts` timestamp NULL DEFAULT NULL COMMENT '充值时间',
  `accountid` int(11) NOT NULL COMMENT '账号ID',
  `areaid` smallint(6) NOT NULL DEFAULT '0' COMMENT '角色所在分区的ID',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';

-- ----------------------------
-- Records of cashlist_4
-- ----------------------------

-- ----------------------------
-- Table structure for cashlist_5
-- ----------------------------
DROP TABLE IF EXISTS `cashlist_5`;
CREATE TABLE `cashlist_5` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `actornick` char(33) NOT NULL COMMENT '角色昵称',
  `cashid` smallint(6) unsigned NOT NULL COMMENT '充值id',
  `rmb` int(11) unsigned DEFAULT '0' COMMENT '人民币',
  `token` int(11) unsigned DEFAULT '0' COMMENT '代币',
  `give` int(11) unsigned DEFAULT '0' COMMENT '首充赠送',
  `tts` timestamp NULL DEFAULT NULL COMMENT '充值时间',
  `accountid` int(11) NOT NULL COMMENT '账号ID',
  `areaid` smallint(6) NOT NULL DEFAULT '0' COMMENT '角色所在分区的ID',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';

-- ----------------------------
-- Records of cashlist_5
-- ----------------------------

-- ----------------------------
-- Table structure for cash_1
-- ----------------------------
DROP TABLE IF EXISTS `cash_1`;
CREATE TABLE `cash_1` (
  `accountid` int(11) unsigned NOT NULL COMMENT '账号ID',
  `cashcount` int(11) unsigned DEFAULT '0' COMMENT '充值id',
  `cash` int(11) unsigned DEFAULT '0' COMMENT '充值的代币数量',
  `tts` timestamp NULL DEFAULT NULL COMMENT '代币最后变更时间',
  PRIMARY KEY (`accountid`),
  KEY `accountid_index` (`accountid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';

-- ----------------------------
-- Records of cash_1
-- ----------------------------
INSERT INTO `cash_1` VALUES ('116932180', '0', '0', null);

-- ----------------------------
-- Table structure for cash_2
-- ----------------------------
DROP TABLE IF EXISTS `cash_2`;
CREATE TABLE `cash_2` (
  `accountid` int(11) unsigned NOT NULL COMMENT '账号ID',
  `cashcount` int(11) unsigned DEFAULT '0' COMMENT '充值id',
  `cash` int(11) unsigned DEFAULT '0' COMMENT '充值的代币数量',
  `tts` timestamp NULL DEFAULT NULL COMMENT '代币最后变更时间',
  PRIMARY KEY (`accountid`),
  KEY `accountid_index` (`accountid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';

-- ----------------------------
-- Records of cash_2
-- ----------------------------

-- ----------------------------
-- Table structure for cash_3
-- ----------------------------
DROP TABLE IF EXISTS `cash_3`;
CREATE TABLE `cash_3` (
  `accountid` int(11) unsigned NOT NULL COMMENT '账号ID',
  `cashcount` int(11) unsigned DEFAULT '0' COMMENT '充值id',
  `cash` int(11) unsigned DEFAULT '0' COMMENT '充值的代币数量',
  `tts` timestamp NULL DEFAULT NULL COMMENT '代币最后变更时间',
  PRIMARY KEY (`accountid`),
  KEY `accountid_index` (`accountid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';

-- ----------------------------
-- Records of cash_3
-- ----------------------------
INSERT INTO `cash_3` VALUES ('1857', '0', '0', null);
INSERT INTO `cash_3` VALUES ('27377', '0', '0', null);

-- ----------------------------
-- Table structure for cash_4
-- ----------------------------
DROP TABLE IF EXISTS `cash_4`;
CREATE TABLE `cash_4` (
  `accountid` int(11) unsigned NOT NULL COMMENT '账号ID',
  `cashcount` int(11) unsigned DEFAULT '0' COMMENT '充值id',
  `cash` int(11) unsigned DEFAULT '0' COMMENT '充值的代币数量',
  `tts` timestamp NULL DEFAULT NULL COMMENT '代币最后变更时间',
  PRIMARY KEY (`accountid`),
  KEY `accountid_index` (`accountid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';

-- ----------------------------
-- Records of cash_4
-- ----------------------------
INSERT INTO `cash_4` VALUES ('30933', '0', '0', null);

-- ----------------------------
-- Table structure for cash_5
-- ----------------------------
DROP TABLE IF EXISTS `cash_5`;
CREATE TABLE `cash_5` (
  `accountid` int(11) unsigned NOT NULL COMMENT '账号ID',
  `cashcount` int(11) unsigned DEFAULT '0' COMMENT '充值id',
  `cash` int(11) unsigned DEFAULT '0' COMMENT '充值的代币数量',
  `tts` timestamp NULL DEFAULT NULL COMMENT '代币最后变更时间',
  PRIMARY KEY (`accountid`),
  KEY `accountid_index` (`accountid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';

-- ----------------------------
-- Records of cash_5
-- ----------------------------
INSERT INTO `cash_5` VALUES ('28409', '0', '0', null);
INSERT INTO `cash_5` VALUES ('54880134', '0', '0', null);
