/*
Navicat MySQL Data Transfer

Source Server         : root
Source Server Version : 50515
Source Host           : localhost:3306
Source Database       : sgs_action

Target Server Type    : MYSQL
Target Server Version : 50515
File Encoding         : 65001

Date: 2017-12-08 11:18:33
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for consumelog
-- ----------------------------
DROP TABLE IF EXISTS `consumelog`;
CREATE TABLE `consumelog` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id,自动递增',
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `actornick` char(33) NOT NULL COMMENT '角色昵称',
  `action` varchar(128) NOT NULL COMMENT '消费行为',
  `money` int(11) unsigned NOT NULL COMMENT '消费数量',
  `remain` bigint(20) unsigned DEFAULT '0' COMMENT '余额',
  `type` tinyint(3) unsigned DEFAULT '0' COMMENT '消费类型(0-金币， 1-代币)',
  `tts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '消费时间',
  PRIMARY KEY (`id`),
  KEY `consumelog_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `consumelog_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `sgs`.`actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of consumelog
-- ----------------------------

-- ----------------------------
-- Table structure for instenterlog
-- ----------------------------
DROP TABLE IF EXISTS `instenterlog`;
CREATE TABLE `instenterlog` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `actorid` int(11) unsigned DEFAULT '0' COMMENT '角色id',
  `actornick` char(33) DEFAULT '' COMMENT '角色昵称',
  `job` tinyint(3) unsigned DEFAULT '0' COMMENT '玩家职业',
  `level` tinyint(3) unsigned DEFAULT '0' COMMENT '玩家等级',
  `instid` smallint(5) unsigned DEFAULT '0' COMMENT '副本id',
  `instname` varchar(65) DEFAULT '' COMMENT '副本名',
  `enter_result` smallint(5) unsigned DEFAULT '0' COMMENT '进入副本结果(0-成功，其他-失败)',
  `enter_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '进入副本时间',
  `enter_desc` varchar(65) DEFAULT '' COMMENT '进入副本结果描述',
  `fin_result` smallint(5) unsigned DEFAULT '0' COMMENT '完成副本结果(0-成功,其他-失败)',
  `fin_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '完成副本时间',
  `fin_desc` varchar(65) DEFAULT '未提交完成' COMMENT '完成副本描述',
  PRIMARY KEY (`id`),
  UNIQUE KEY `instenterlog_idx_1` (`actorid`,`instid`,`enter_tts`) USING BTREE,
  CONSTRAINT `instenterlog_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `sgs`.`actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of instenterlog
-- ----------------------------

-- ----------------------------
-- Table structure for playdevice
-- ----------------------------
DROP TABLE IF EXISTS `playdevice`;
CREATE TABLE `playdevice` (
  `dev_idfa` varchar(41) NOT NULL COMMENT '设备ID',
  `dev_type` varchar(32) DEFAULT NULL COMMENT '设备型号',
  `system` tinyint(4) DEFAULT NULL COMMENT '系统类型',
  `version` varchar(32) DEFAULT NULL COMMENT '系统版本',
  `dev_ip` varchar(16) DEFAULT NULL COMMENT '设备IP',
  `region` varchar(40) DEFAULT NULL COMMENT '区域',
  `platid` smallint(6) DEFAULT NULL COMMENT '渠道ID',
  `create_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '创建时间',
  `version_check_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '版本校验',
  `play_ready_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '开始游戏',
  PRIMARY KEY (`dev_idfa`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of playdevice
-- ----------------------------

-- ----------------------------
-- Table structure for playrecord
-- ----------------------------
DROP TABLE IF EXISTS `playrecord`;
CREATE TABLE `playrecord` (
  `user` char(33) NOT NULL COMMENT '登录帐号',
  `dev_idfa` varchar(41) NOT NULL COMMENT '设备ID',
  `login_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '登陆账号触发的时间',
  `load_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '读取加载',
  `newbie_guide_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '开始新手引导',
  `pass_stage_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '新手引导通关',
  `begin_cartoon_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '开场动画',
  `play_cartoon_tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '剧情动画',
  `select_roleid` int(11) DEFAULT '0' COMMENT '选择的角色ID',
  PRIMARY KEY (`user`,`dev_idfa`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of playrecord
-- ----------------------------

-- ----------------------------
-- Table structure for producelog
-- ----------------------------
DROP TABLE IF EXISTS `producelog`;
CREATE TABLE `producelog` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id,自动递增',
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `actornick` char(33) NOT NULL COMMENT '角色昵称',
  `action` varchar(128) NOT NULL COMMENT '产出行为',
  `money` int(11) unsigned NOT NULL COMMENT '产出价值',
  `remain` bigint(20) unsigned DEFAULT '0' COMMENT '余额',
  `type` tinyint(3) unsigned DEFAULT '0' COMMENT '产出类型(0-金币， 1-代币)',
  `tts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '产出时间',
  PRIMARY KEY (`id`),
  KEY `producelog_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `producelog_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `sgs`.`actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of producelog
-- ----------------------------
