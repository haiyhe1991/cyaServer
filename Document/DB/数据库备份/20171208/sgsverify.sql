/*
Navicat MySQL Data Transfer

Source Server         : root
Source Server Version : 50515
Source Host           : localhost:3306
Source Database       : sgsverify

Target Server Type    : MYSQL
Target Server Version : 50515
File Encoding         : 65001

Date: 2017-12-08 11:19:04
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for game_partition
-- ----------------------------
DROP TABLE IF EXISTS `game_partition`;
CREATE TABLE `game_partition` (
  `id` smallint(6) unsigned NOT NULL DEFAULT '0',
  `name` varchar(65) DEFAULT NULL COMMENT '分区名称',
  `status` tinyint(4) DEFAULT NULL COMMENT '状态',
  `isrecomment` tinyint(4) DEFAULT NULL COMMENT '是否推荐分区',
  `actors` int(11) unsigned DEFAULT '0' COMMENT '分区角色数量',
  `accounts` int(11) unsigned DEFAULT '0' COMMENT '分区注册账号数(不分区使用)',
  `tts` timestamp NULL DEFAULT NULL COMMENT '创建时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of game_partition
-- ----------------------------
INSERT INTO `game_partition` VALUES ('1', '国服1区', null, null, '0', '7', null);
INSERT INTO `game_partition` VALUES ('2', '国服2区', null, null, '0', '0', null);
INSERT INTO `game_partition` VALUES ('3', '国服3区', null, null, '0', '0', null);

-- ----------------------------
-- Table structure for player_account_1
-- ----------------------------
DROP TABLE IF EXISTS `player_account_1`;
CREATE TABLE `player_account_1` (
  `id` int(11) unsigned NOT NULL COMMENT '账号id',
  `user` char(33) DEFAULT NULL COMMENT '用户名',
  `pwd` char(65) DEFAULT NULL COMMENT '密码',
  `ip` char(16) DEFAULT NULL COMMENT '用户注册ip',
  `regtts` timestamp NULL DEFAULT NULL COMMENT '注册时间',
  `platsource` char(17) DEFAULT NULL COMMENT '注册平台',
  `os` char(17) DEFAULT NULL COMMENT '用户注册使用操作系统',
  `model` char(17) DEFAULT '' COMMENT '机型',
  `regdev` char(65) DEFAULT NULL COMMENT '注册设备号',
  `ver` char(17) DEFAULT '' COMMENT '当前包版本',
  `visit` int(11) unsigned DEFAULT '0' COMMENT '访问次数',
  `lastdev` char(65) DEFAULT NULL COMMENT '最后一次登录设备号',
  `lasttts` timestamp NULL DEFAULT NULL COMMENT '最后一次登录时间',
  `partid` smallint(5) unsigned DEFAULT '0' COMMENT '所在分区id',
  `status` tinyint(4) unsigned DEFAULT NULL COMMENT '账号状态',
  PRIMARY KEY (`id`),
  UNIQUE KEY `player_account_1_idx_1` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of player_account_1
-- ----------------------------
INSERT INTO `player_account_1` VALUES ('116932180', 'iamgod', 'a4757d7419ff3b48e92e90596f0e7548', '', '2017-06-10 15:08:41', 'win32', 'windows', 'tinkvision', '1212131213121', 'v1.0.0', '4', '1212131213121', '2017-06-13 21:02:32', '1', '0');

-- ----------------------------
-- Table structure for player_account_2
-- ----------------------------
DROP TABLE IF EXISTS `player_account_2`;
CREATE TABLE `player_account_2` (
  `id` int(11) unsigned NOT NULL COMMENT '账号id',
  `user` char(33) DEFAULT NULL COMMENT '用户名',
  `pwd` char(65) DEFAULT NULL COMMENT '密码',
  `ip` char(16) DEFAULT NULL COMMENT '用户注册ip',
  `regtts` timestamp NULL DEFAULT NULL COMMENT '注册时间',
  `platsource` char(17) DEFAULT NULL COMMENT '注册平台',
  `os` char(17) DEFAULT NULL COMMENT '用户注册使用操作系统',
  `model` char(17) DEFAULT '' COMMENT '机型',
  `regdev` char(65) DEFAULT NULL COMMENT '注册设备号',
  `ver` char(17) DEFAULT '' COMMENT '当前包版本',
  `visit` int(11) unsigned DEFAULT '0' COMMENT '访问次数',
  `lastdev` char(65) DEFAULT NULL COMMENT '最后一次登录设备号',
  `lasttts` timestamp NULL DEFAULT NULL COMMENT '最后一次登录时间',
  `partid` smallint(5) unsigned DEFAULT '0' COMMENT '所在分区id',
  `status` tinyint(4) unsigned DEFAULT NULL COMMENT '账号状态',
  PRIMARY KEY (`id`),
  UNIQUE KEY `player_account_2_idx_1` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of player_account_2
-- ----------------------------

-- ----------------------------
-- Table structure for player_account_3
-- ----------------------------
DROP TABLE IF EXISTS `player_account_3`;
CREATE TABLE `player_account_3` (
  `id` int(11) unsigned NOT NULL COMMENT '账号id',
  `user` char(33) DEFAULT NULL COMMENT '用户名',
  `pwd` char(65) DEFAULT NULL COMMENT '密码',
  `ip` char(16) DEFAULT NULL COMMENT '用户注册ip',
  `regtts` timestamp NULL DEFAULT NULL COMMENT '注册时间',
  `platsource` char(17) DEFAULT NULL COMMENT '注册平台',
  `os` char(17) DEFAULT NULL COMMENT '用户注册使用操作系统',
  `model` char(17) DEFAULT '' COMMENT '机型',
  `regdev` char(65) DEFAULT NULL COMMENT '注册设备号',
  `ver` char(17) DEFAULT '' COMMENT '当前包版本',
  `visit` int(11) unsigned DEFAULT '0' COMMENT '访问次数',
  `lastdev` char(65) DEFAULT NULL COMMENT '最后 一次登录设备号',
  `lasttts` timestamp NULL DEFAULT NULL COMMENT '最后一次登录时间',
  `partid` smallint(5) unsigned DEFAULT '0' COMMENT '所在分区id',
  `status` tinyint(4) unsigned DEFAULT NULL COMMENT '账号状态',
  PRIMARY KEY (`id`),
  UNIQUE KEY `player_account_3_idx_1` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of player_account_3
-- ----------------------------
INSERT INTO `player_account_3` VALUES ('1857', 'q1', 'e10adc3949ba59abbe56e057f20f883e', '', '2017-12-05 10:08:16', 'win32', 'windows', 'tinkvision', '12345678', 'v1.0.0', '1', '12345678', '2017-12-05 10:08:22', '1', '0');
INSERT INTO `player_account_3` VALUES ('27377', 'cya', 'e10adc3949ba59abbe56e057f20f883e', '', '2017-06-28 11:48:16', 'win32', 'windows', 'tinkvision', '12345678', 'v1.0.0', '75', '12345678', '2017-12-05 09:39:02', '1', '0');

-- ----------------------------
-- Table structure for player_account_4
-- ----------------------------
DROP TABLE IF EXISTS `player_account_4`;
CREATE TABLE `player_account_4` (
  `id` int(11) unsigned NOT NULL COMMENT '账号id',
  `user` char(33) DEFAULT NULL COMMENT '用户名',
  `pwd` char(65) DEFAULT NULL COMMENT '密码',
  `ip` char(16) DEFAULT NULL COMMENT '用户注册ip',
  `regtts` timestamp NULL DEFAULT NULL COMMENT '注册时间',
  `platsource` char(17) DEFAULT NULL COMMENT '注册平台',
  `os` char(17) DEFAULT NULL COMMENT '用户注册使用操作系统',
  `model` char(17) DEFAULT '' COMMENT '机型',
  `regdev` char(65) DEFAULT NULL COMMENT '注册设备号',
  `ver` char(17) DEFAULT '' COMMENT '当前包版本',
  `visit` int(11) unsigned DEFAULT '0' COMMENT '访问次数',
  `lastdev` char(65) DEFAULT NULL COMMENT '最后一次登录设备号',
  `lasttts` timestamp NULL DEFAULT NULL COMMENT '最后一次登录时间',
  `partid` smallint(5) unsigned DEFAULT '0' COMMENT '所在分区id',
  `status` tinyint(4) unsigned DEFAULT NULL COMMENT '账号状态',
  PRIMARY KEY (`id`),
  UNIQUE KEY `player_account_4_idx_1` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of player_account_4
-- ----------------------------
INSERT INTO `player_account_4` VALUES ('30933', 'qwe', 'e10adc3949ba59abbe56e057f20f883e', '', '2017-11-18 16:03:15', 'win32', 'windows', 'tinkvision', '12345678', 'v1.0.0', '22', '12345678', '2017-12-04 15:43:07', '1', '0');

-- ----------------------------
-- Table structure for player_account_5
-- ----------------------------
DROP TABLE IF EXISTS `player_account_5`;
CREATE TABLE `player_account_5` (
  `id` int(11) unsigned NOT NULL COMMENT '账号id',
  `user` char(33) DEFAULT NULL COMMENT '用户名',
  `pwd` char(65) DEFAULT NULL COMMENT '密码',
  `ip` char(16) DEFAULT NULL COMMENT '用户注册ip',
  `regtts` timestamp NULL DEFAULT NULL COMMENT '注册时间',
  `platsource` char(17) DEFAULT NULL COMMENT '注册平台',
  `os` char(17) DEFAULT NULL COMMENT '用户注册使用操作系统',
  `model` char(17) DEFAULT '' COMMENT '机型',
  `regdev` char(65) DEFAULT NULL COMMENT '注册设备号',
  `ver` char(17) DEFAULT '' COMMENT '当前包版本',
  `visit` int(11) unsigned DEFAULT '0' COMMENT '访问次数',
  `lastdev` char(65) DEFAULT NULL COMMENT '最后一次登录设备号',
  `lasttts` timestamp NULL DEFAULT NULL COMMENT '最后一次登录时间',
  `partid` smallint(5) unsigned DEFAULT '0' COMMENT '所在分区id',
  `status` tinyint(4) unsigned DEFAULT NULL COMMENT '账号状态',
  PRIMARY KEY (`id`),
  UNIQUE KEY `player_account_5_idx_1` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of player_account_5
-- ----------------------------
INSERT INTO `player_account_5` VALUES ('28409', 'hhy', 'e10adc3949ba59abbe56e057f20f883e', '', '2017-06-28 13:42:27', 'win32', 'windows', 'tinkvision', '12345678', 'v1.0.0', '702', '12345678', '2017-12-05 09:38:25', '1', '0');
INSERT INTO `player_account_5` VALUES ('54880134', '123456', 'e10adc3949ba59abbe56e057f20f883e', '', '2017-06-28 11:30:09', 'win32', 'windows', 'tinkvision', '12345678', 'v1.0.0', '6', '12345678', '2017-11-22 16:57:01', '1', '0');
