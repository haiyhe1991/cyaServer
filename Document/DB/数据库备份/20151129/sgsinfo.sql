-- MySQL dump 10.13  Distrib 5.6.11, for Linux (x86_64)
--
-- Host: 192.168.1.201    Database: sgsinfo
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
-- Table structure for table `account_info`
--

DROP TABLE IF EXISTS `account_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `account_info` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `accountid` int(11) unsigned DEFAULT '0' COMMENT '账号id',
  `user` char(33) DEFAULT '' COMMENT '用户名',
  `pwd` char(65) DEFAULT '' COMMENT '密码',
  `ip` char(16) DEFAULT '' COMMENT '用户注册ip',
  `regtts` timestamp NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '注册时间',
  `platsource` char(17) DEFAULT '' COMMENT '注册平台',
  `os` char(17) DEFAULT '' COMMENT '用户注册使用操作系统',
  `model` char(17) DEFAULT '' COMMENT '机型',
  `regdev` char(65) DEFAULT '' COMMENT '注册设备号',
  `ver` char(17) DEFAULT '' COMMENT '当前包版本',
  `visit` int(11) unsigned DEFAULT '0' COMMENT '访问次数',
  `lastdev` char(65) DEFAULT '' COMMENT '最后一次登录设备号',
  `lasttts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后一次登录时间',
  `partid` smallint(5) unsigned DEFAULT '0' COMMENT '所在分区id',
  `status` tinyint(4) unsigned DEFAULT '0' COMMENT '账号状态',
  PRIMARY KEY (`id`),
  UNIQUE KEY `account_info_idx_1` (`id`) USING BTREE,
  KEY `account_info_idx_2` (`user`) USING BTREE,
  KEY `account_info_idx_3` (`regtts`) USING BTREE,
  KEY `account_info_idx_4` (`lasttts`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=161067758 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `account_leavekeep`
--

DROP TABLE IF EXISTS `account_leavekeep`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `account_leavekeep` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'id',
  `platformid` int(10) unsigned DEFAULT '0' COMMENT '平台ID',
  `partid` smallint(5) unsigned DEFAULT '0' COMMENT '分区ID',
  `regcount` int(11) unsigned DEFAULT '0' COMMENT '注册人数',
  `lgcount` int(11) unsigned DEFAULT '0' COMMENT '次X登录人数',
  `rate` float DEFAULT '0' COMMENT '留存率',
  `tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '留存时间',
  `type` tinyint(4) DEFAULT '1' COMMENT '留存类型',
  `uptts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新留存时间',
  `uplgcount` int(11) DEFAULT '0' COMMENT '更新时当日登录总数',
  PRIMARY KEY (`id`),
  UNIQUE KEY `leavekeep_idx_1` (`id`) USING BTREE,
  KEY `leavekeep_idx_2` (`tts`) USING BTREE,
  KEY `leavekeep_idx_3` (`uptts`) USING BTREE,
  KEY `leavekeep_idx_4` (`platformid`) USING BTREE,
  KEY `leavekeep_idx_5` (`partid`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=972 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `active_device`
--

DROP TABLE IF EXISTS `active_device`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `active_device` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `platformid` int(10) unsigned DEFAULT '0' COMMENT '平台ID',
  `partid` smallint(5) unsigned DEFAULT '0' COMMENT '分区ID',
  `activetts` timestamp NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '注册时间',
  `count` int(11) unsigned DEFAULT '0' COMMENT '注册人数',
  `type` tinyint(4) unsigned DEFAULT '1' COMMENT '创建类型1:当日新增(不包含未创角色)，2:当日新增总的帐号数(含未创角色),3:截止当前时间增加总的设备数',
  PRIMARY KEY (`id`),
  UNIQUE KEY `regist_account_idx_1` (`id`) USING BTREE,
  KEY `regist_account_idx_2` (`activetts`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=821 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `actor_info`
--

DROP TABLE IF EXISTS `actor_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `actor_info` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `actorid` int(11) unsigned NOT NULL COMMENT '记录id,自动递增(角色id)',
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
  `faction` tinyint(4) DEFAULT NULL COMMENT '所在阵营id',
  `vist` int(11) unsigned DEFAULT NULL COMMENT '访问次数',
  `online` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '在线时间(秒)',
  `lvtts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '最后访问时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `actor_info_idx_1` (`id`) USING BTREE,
  KEY `actor_info_idx_2` (`nick`) USING BTREE,
  KEY `actor_info_idx_3` (`createtts`) USING BTREE,
  KEY `actor_info_idx_4` (`lvtts`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=267163429 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `actor_leavekeep`
--

DROP TABLE IF EXISTS `actor_leavekeep`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `actor_leavekeep` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'id',
  `platformid` int(10) unsigned DEFAULT '0' COMMENT '平台ID',
  `partid` smallint(5) unsigned DEFAULT '0' COMMENT '分区ID',
  `regcount` int(11) unsigned DEFAULT '0' COMMENT '注册人数',
  `lgcount` int(11) unsigned DEFAULT '0' COMMENT '登录人数',
  `rate` float unsigned DEFAULT '0' COMMENT '留存率',
  `tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '留存时间',
  `type` tinyint(4) unsigned DEFAULT '1' COMMENT '留存类型',
  `uptts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新留存时间',
  `uplgcount` int(11) unsigned DEFAULT '0' COMMENT '更新时当日登录总数',
  PRIMARY KEY (`id`),
  UNIQUE KEY `leavekeep_idx_1` (`id`) USING BTREE,
  KEY `leavekeep_idx_2` (`tts`) USING BTREE,
  KEY `leavekeep_idx_3` (`uptts`) USING BTREE,
  KEY `leavekeep_idx_4` (`platformid`) USING BTREE,
  KEY `leavekeep_idx_` (`partid`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=945 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cash`
--

DROP TABLE IF EXISTS `cash`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cash` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `platformid` int(10) unsigned DEFAULT '0' COMMENT '平台ID',
  `partid` smallint(5) unsigned DEFAULT '0' COMMENT '分区ID',
  `actorcount` int(11) unsigned DEFAULT '0' COMMENT '当日付费角色数',
  `paymentcount` int(11) unsigned DEFAULT '0' COMMENT '当日付费总次数',
  `firstrechargecount` int(10) unsigned DEFAULT '0' COMMENT '当日首充用户数',
  `totalcount` int(10) unsigned DEFAULT '0' COMMENT '累计新增付费用户数',
  `adduprmb` int(11) unsigned DEFAULT '0' COMMENT '当日新增付费用户的付费金额',
  `rmb` int(10) unsigned DEFAULT '0' COMMENT '当日累计付费金额',
  `totalrmb` int(11) unsigned DEFAULT '0' COMMENT '累计付费用户（开服起）',
  `tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '刷新时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `cash_idx_1` (`id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=57 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `create_actor`
--

DROP TABLE IF EXISTS `create_actor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `create_actor` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `platformid` int(10) unsigned DEFAULT '0' COMMENT '平台ID',
  `partid` smallint(5) unsigned DEFAULT '0' COMMENT '分区ID',
  `createtts` timestamp NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '创建时间',
  `count` int(11) unsigned DEFAULT '0' COMMENT '数量',
  `type` tinyint(4) unsigned DEFAULT '0' COMMENT '创建类型1:每日新增，2:截止当前时间增加总的角色数',
  PRIMARY KEY (`id`),
  UNIQUE KEY `create_actor_idx_1` (`id`) USING BTREE,
  KEY `create_actor_idx_2` (`createtts`) USING BTREE,
  KEY `create_actor_idx_3` (`platformid`) USING BTREE,
  KEY `create_actor_idx_4` (`partid`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=549 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `device_leavekeep`
--

DROP TABLE IF EXISTS `device_leavekeep`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `device_leavekeep` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'id',
  `platformid` int(10) unsigned DEFAULT '0' COMMENT '平台ID',
  `partid` smallint(5) unsigned DEFAULT '0' COMMENT '分区ID',
  `regcount` int(11) unsigned DEFAULT '0' COMMENT '注册人数',
  `lgcount` int(11) unsigned DEFAULT '0' COMMENT '登录人数',
  `rate` float unsigned DEFAULT '0' COMMENT '留存率',
  `tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '留存时间',
  `type` tinyint(4) unsigned DEFAULT '1' COMMENT '留存类型',
  `uptts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新留存时间',
  `uplgcount` int(11) unsigned DEFAULT '0' COMMENT '更新时当日登录总数',
  PRIMARY KEY (`id`),
  UNIQUE KEY `leavekeep_idx_1` (`id`) USING BTREE,
  KEY `leavekeep_idx_2` (`tts`) USING BTREE,
  KEY `leavekeep_idx_3` (`uptts`) USING BTREE,
  KEY `leavekeep_idx_4` (`platformid`) USING BTREE,
  KEY `leavekeep_idx_5` (`partid`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=970 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gm_account`
--

DROP TABLE IF EXISTS `gm_account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `gm_account` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` char(33) NOT NULL,
  `passwd` char(33) NOT NULL,
  `type` enum('1','0') NOT NULL DEFAULT '0' COMMENT '是否为管理员[1：管理员， 0: 普通用户]',
  PRIMARY KEY (`id`),
  UNIQUE KEY `acount_idx_1` (`id`) USING BTREE,
  KEY `account_idx_2` (`name`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `online`
--

DROP TABLE IF EXISTS `online`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `online` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `platformid` int(10) unsigned DEFAULT '0' COMMENT '平台ID',
  `partid` tinyint(4) unsigned DEFAULT '0' COMMENT '当前分区',
  `count` int(11) unsigned DEFAULT '0' COMMENT '在线人数',
  `tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '在线时间',
  `type` tinyint(4) unsigned DEFAULT '0' COMMENT '在线类型[1: 普通， 2: 最大在线， 3:平均在线]',
  PRIMARY KEY (`id`),
  UNIQUE KEY `online_idx_1` (`id`) USING BTREE,
  KEY `online_idx_2` (`tts`) USING BTREE,
  KEY `online_idx_3` (`platformid`) USING BTREE,
  KEY `online_idx_4` (`partid`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=14959 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `person`
--

DROP TABLE IF EXISTS `person`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `person` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(40) NOT NULL,
  `city` varchar(20) DEFAULT NULL,
  `salary` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `regist_account`
--

DROP TABLE IF EXISTS `regist_account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `regist_account` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `platformid` int(10) unsigned DEFAULT '0' COMMENT '平台ID',
  `partid` smallint(5) unsigned DEFAULT '0' COMMENT '分区ID',
  `regtts` timestamp NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '注册时间',
  `count` int(11) unsigned DEFAULT '0' COMMENT '注册人数',
  `type` tinyint(4) unsigned DEFAULT '0' COMMENT '创建类型1:当日新增(不包含未创角色)，2:当日新增总的帐号数(含未创角色),3:截止当前时间增加总的角色数',
  PRIMARY KEY (`id`),
  UNIQUE KEY `regist_account_idx_1` (`id`) USING BTREE,
  KEY `regist_account_idx_2` (`regtts`) USING BTREE,
  KEY `regist_account_idx_3` (`platformid`) USING BTREE,
  KEY `regist_account_idx_4` (`partid`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=126 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-11-28 15:02:57
