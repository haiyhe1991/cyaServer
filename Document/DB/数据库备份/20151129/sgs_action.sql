-- MySQL dump 10.13  Distrib 5.6.11, for Linux (x86_64)
--
-- Host: 192.168.1.201    Database: sgs_action
-- ------------------------------------------------------
-- Server version	5.6.11

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `consumelog`
--

DROP TABLE IF EXISTS `consumelog`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=5849 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `instenterlog`
--

DROP TABLE IF EXISTS `instenterlog`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=4935 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `playdevice`
--

DROP TABLE IF EXISTS `playdevice`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `playrecord`
--

DROP TABLE IF EXISTS `playrecord`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `producelog`
--

DROP TABLE IF EXISTS `producelog`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=2545 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-11-28 15:02:51
