-- MySQL dump 10.13  Distrib 5.6.11, for Linux (x86_64)
--
-- Host: 192.168.1.201    Database: sgscash
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
-- Table structure for table `cash_1`
--

DROP TABLE IF EXISTS `cash_1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cash_1` (
  `accountid` int(11) unsigned NOT NULL COMMENT '账号ID',
  `cashcount` int(11) unsigned DEFAULT '0' COMMENT '充值id',
  `cash` int(11) unsigned DEFAULT '0' COMMENT '充值的代币数量',
  `tts` timestamp NULL DEFAULT NULL COMMENT '代币最后变更时间',
  PRIMARY KEY (`accountid`),
  KEY `accountid_index` (`accountid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cash_2`
--

DROP TABLE IF EXISTS `cash_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cash_2` (
  `accountid` int(11) unsigned NOT NULL COMMENT '账号ID',
  `cashcount` int(11) unsigned DEFAULT '0' COMMENT '充值id',
  `cash` int(11) unsigned DEFAULT '0' COMMENT '充值的代币数量',
  `tts` timestamp NULL DEFAULT NULL COMMENT '代币最后变更时间',
  PRIMARY KEY (`accountid`),
  KEY `accountid_index` (`accountid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cash_3`
--

DROP TABLE IF EXISTS `cash_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cash_3` (
  `accountid` int(11) unsigned NOT NULL COMMENT '账号ID',
  `cashcount` int(11) unsigned DEFAULT '0' COMMENT '充值id',
  `cash` int(11) unsigned DEFAULT '0' COMMENT '充值的代币数量',
  `tts` timestamp NULL DEFAULT NULL COMMENT '代币最后变更时间',
  PRIMARY KEY (`accountid`),
  KEY `accountid_index` (`accountid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cash_4`
--

DROP TABLE IF EXISTS `cash_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cash_4` (
  `accountid` int(11) unsigned NOT NULL COMMENT '账号ID',
  `cashcount` int(11) unsigned DEFAULT '0' COMMENT '充值id',
  `cash` int(11) unsigned DEFAULT '0' COMMENT '充值的代币数量',
  `tts` timestamp NULL DEFAULT NULL COMMENT '代币最后变更时间',
  PRIMARY KEY (`accountid`),
  KEY `accountid_index` (`accountid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cash_5`
--

DROP TABLE IF EXISTS `cash_5`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cash_5` (
  `accountid` int(11) unsigned NOT NULL COMMENT '账号ID',
  `cashcount` int(11) unsigned DEFAULT '0' COMMENT '充值id',
  `cash` int(11) unsigned DEFAULT '0' COMMENT '充值的代币数量',
  `tts` timestamp NULL DEFAULT NULL COMMENT '代币最后变更时间',
  PRIMARY KEY (`accountid`),
  KEY `accountid_index` (`accountid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='兑换表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cashlist_1`
--

DROP TABLE IF EXISTS `cashlist_1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cashlist_2`
--

DROP TABLE IF EXISTS `cashlist_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cashlist_3`
--

DROP TABLE IF EXISTS `cashlist_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cashlist_4`
--

DROP TABLE IF EXISTS `cashlist_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cashlist_5`
--

DROP TABLE IF EXISTS `cashlist_5`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-11-28 15:02:55
