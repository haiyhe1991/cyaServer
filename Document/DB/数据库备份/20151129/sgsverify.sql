-- MySQL dump 10.13  Distrib 5.6.11, for Linux (x86_64)
--
-- Host: 192.168.1.201    Database: sgsverify
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
-- Table structure for table `game_partition`
--

DROP TABLE IF EXISTS `game_partition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_account_1`
--

DROP TABLE IF EXISTS `player_account_1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_account_2`
--

DROP TABLE IF EXISTS `player_account_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_account_3`
--

DROP TABLE IF EXISTS `player_account_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_account_4`
--

DROP TABLE IF EXISTS `player_account_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_account_5`
--

DROP TABLE IF EXISTS `player_account_5`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-11-28 15:02:50
