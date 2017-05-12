-- MySQL dump 10.13  Distrib 5.6.11, for Linux (x86_64)
--
-- Host: 192.168.1.201    Database: sgsrecharge
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
-- Table structure for table `activated_device1`
--

DROP TABLE IF EXISTS `activated_device1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `activated_device1` (
  `dev_idfa` varchar(41) NOT NULL COMMENT '设备号IDFA',
  `dev_type` varchar(32) DEFAULT NULL COMMENT '机型',
  `system` varchar(16) DEFAULT NULL COMMENT '系统',
  `createtts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '时间',
  `dev_ip` varchar(16) NOT NULL COMMENT '设备IP',
  `region` varchar(40) DEFAULT NULL COMMENT '区域',
  PRIMARY KEY (`dev_idfa`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `activated_device2`
--

DROP TABLE IF EXISTS `activated_device2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `activated_device2` (
  `dev_idfa` varchar(41) NOT NULL COMMENT '设备IDFA',
  `dev_type` varchar(32) DEFAULT NULL COMMENT '机型',
  `system` varchar(16) DEFAULT NULL COMMENT '系统',
  `createtts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '时间',
  `dev_ip` varchar(16) NOT NULL COMMENT '设备IP',
  `region` varchar(40) DEFAULT NULL COMMENT '区域',
  PRIMARY KEY (`dev_idfa`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `activated_device3`
--

DROP TABLE IF EXISTS `activated_device3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `activated_device3` (
  `dev_idfa` varchar(41) NOT NULL COMMENT '设备IDFA',
  `dev_type` varchar(32) DEFAULT NULL COMMENT '机型',
  `system` varchar(16) DEFAULT NULL COMMENT '系统',
  `createtts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '时间',
  `dev_ip` varchar(16) NOT NULL COMMENT '设备IP',
  `region` varchar(40) DEFAULT NULL COMMENT '区域',
  PRIMARY KEY (`dev_idfa`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `activated_device4`
--

DROP TABLE IF EXISTS `activated_device4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `activated_device4` (
  `dev_idfa` varchar(41) NOT NULL COMMENT '设备IDFA',
  `dev_type` varchar(32) DEFAULT NULL COMMENT '机型',
  `system` varchar(16) DEFAULT NULL COMMENT '系统',
  `createtts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '时间',
  `dev_ip` varchar(16) NOT NULL COMMENT '设备IP',
  `region` varchar(40) DEFAULT NULL COMMENT '区域',
  PRIMARY KEY (`dev_idfa`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `activated_device5`
--

DROP TABLE IF EXISTS `activated_device5`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `activated_device5` (
  `dev_idfa` varchar(41) NOT NULL COMMENT '设备IDFA',
  `dev_type` varchar(32) DEFAULT NULL COMMENT '机型',
  `system` varchar(16) DEFAULT NULL COMMENT '系统',
  `createtts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '时间',
  `dev_ip` varchar(16) NOT NULL COMMENT '设备IP',
  `region` varchar(40) DEFAULT NULL COMMENT '区域',
  PRIMARY KEY (`dev_idfa`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_cash_1`
--

DROP TABLE IF EXISTS `player_cash_1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_cash_2`
--

DROP TABLE IF EXISTS `player_cash_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_cash_3`
--

DROP TABLE IF EXISTS `player_cash_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_cash_4`
--

DROP TABLE IF EXISTS `player_cash_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_cash_5`
--

DROP TABLE IF EXISTS `player_cash_5`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-11-28 15:02:53
