-- MySQL dump 10.13  Distrib 5.6.11, for Linux (x86_64)
--
-- Host: 192.168.1.201    Database: sgs
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
-- Table structure for table `achievement`
--

DROP TABLE IF EXISTS `achievement`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `achievement` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `point` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '成就点数',
  `userdata` varbinary(1024) DEFAULT NULL COMMENT '用户信息',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `achievement_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `achievement_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `achievement_total`
--

DROP TABLE IF EXISTS `achievement_total`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `activeinst`
--

DROP TABLE IF EXISTS `activeinst`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `activity`
--

DROP TABLE IF EXISTS `activity`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `actor`
--

DROP TABLE IF EXISTS `actor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=264608117 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `actorlock`
--

DROP TABLE IF EXISTS `actorlock`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cash`
--

DROP TABLE IF EXISTS `cash`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=463 DEFAULT CHARSET=utf8 COMMENT='兑换表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cashcode`
--

DROP TABLE IF EXISTS `cashcode`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `competitive`
--

DROP TABLE IF EXISTS `competitive`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `competitive` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `win` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '胜利次数',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `competitive_idx` (`actorid`) USING BTREE,
  CONSTRAINT `competitive_ibfk` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cp_reward`
--

DROP TABLE IF EXISTS `cp_reward`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cp_reward` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `tts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '领取时间',
  `actorid` int(11) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `cprewardid` smallint(6) NOT NULL DEFAULT '0' COMMENT '奖励领取',
  `cp` int(11) DEFAULT NULL COMMENT '获取奖励时玩家的战力',
  PRIMARY KEY (`id`),
  KEY `actorid_idx` (`actorid`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=37 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dailysign`
--

DROP TABLE IF EXISTS `dailysign`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dailysign` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `userdata` varbinary(1024) NOT NULL COMMENT '用户数据',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `sign_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `dailysign_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dailytask`
--

DROP TABLE IF EXISTS `dailytask`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dailytask` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id',
  `actorid` int(11) unsigned DEFAULT NULL COMMENT '角色id',
  `taskid` smallint(6) unsigned DEFAULT NULL COMMENT '任务id',
  `tts` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '完成时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `dailytask_idx_1` (`actorid`,`taskid`) USING BTREE,
  CONSTRAINT `dailytask_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `data`
--

DROP TABLE IF EXISTS `data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `email`
--

DROP TABLE IF EXISTS `email`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=560 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `friend`
--

DROP TABLE IF EXISTS `friend`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `goldexchange`
--

DROP TABLE IF EXISTS `goldexchange`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `goldexchange` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `exchangetimes` smallint(6) unsigned DEFAULT NULL COMMENT '当天兑换次数',
  `tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后一次兑换时间',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `goldexchange_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `goldexchange_ifbk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild`
--

DROP TABLE IF EXISTS `guild`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guildmember`
--

DROP TABLE IF EXISTS `guildmember`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guildposition`
--

DROP TABLE IF EXISTS `guildposition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `helper`
--

DROP TABLE IF EXISTS `helper`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `helper` (
  `actorid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '角色id',
  `helps` varbinary(1024) DEFAULT NULL COMMENT '已完成引导',
  `tts` timestamp NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最后一次完成引导时间',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `helper_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `helper_ifbk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `heyf_t10`
--

DROP TABLE IF EXISTS `heyf_t10`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `heyf_t10` (
  `empid` int(11) DEFAULT NULL,
  `deptid` int(11) DEFAULT NULL,
  `salary` decimal(10,2) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `instancehistory`
--

DROP TABLE IF EXISTS `instancehistory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=537 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `joblock`
--

DROP TABLE IF EXISTS `joblock`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `joblock` (
  `accountid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '账号id',
  `jobs` varbinary(512) DEFAULT NULL COMMENT '解锁职业',
  `tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后一次解锁时间',
  PRIMARY KEY (`accountid`),
  UNIQUE KEY `lockactor_idx_1` (`accountid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ladder`
--

DROP TABLE IF EXISTS `ladder`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ladder_rank`
--

DROP TABLE IF EXISTS `ladder_rank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=50 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `limitbuy`
--

DROP TABLE IF EXISTS `limitbuy`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `online_reward`
--

DROP TABLE IF EXISTS `online_reward`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=397 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `permanent`
--

DROP TABLE IF EXISTS `permanent`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `personaltimertask`
--

DROP TABLE IF EXISTS `personaltimertask`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `personaltimertask` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '定时任务记录id',
  `taskid` smallint(6) unsigned NOT NULL COMMENT '任务编号',
  `actorid` int(11) unsigned NOT NULL COMMENT '所属角色id',
  `starttime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '开始时间',
  PRIMARY KEY (`id`),
  KEY `personaltimertask_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `personaltimertask_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `randactivity`
--

DROP TABLE IF EXISTS `randactivity`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `randactivity` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `activity` varbinary(64) DEFAULT NULL COMMENT '拥有的活动副本信息',
  `last_send_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '上次发放时间',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `randactivity_idx` (`actorid`) USING BTREE,
  CONSTRAINT `randactivity_ibfk` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `rankcomp`
--

DROP TABLE IF EXISTS `rankcomp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=118 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `rankcp`
--

DROP TABLE IF EXISTS `rankcp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=4560 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `rankinst`
--

DROP TABLE IF EXISTS `rankinst`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=2428 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `rankinstance`
--

DROP TABLE IF EXISTS `rankinstance`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `rankinstnum`
--

DROP TABLE IF EXISTS `rankinstnum`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=89 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ranklevel`
--

DROP TABLE IF EXISTS `ranklevel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=4560 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `rankpoint`
--

DROP TABLE IF EXISTS `rankpoint`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=233 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `recharge`
--

DROP TABLE IF EXISTS `recharge`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `roulette`
--

DROP TABLE IF EXISTS `roulette`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `roulette` (
  `actorid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '角色id',
  `times` smallint(5) unsigned DEFAULT '0' COMMENT '抽取次数',
  `props` varbinary(1024) DEFAULT NULL COMMENT '轮盘奖品',
  `tts` timestamp NULL DEFAULT NULL COMMENT '最后一次抽取时间',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `roulette_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `roulette_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sellhistory`
--

DROP TABLE IF EXISTS `sellhistory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sellhistory` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id,自动增长',
  `actorid` int(11) unsigned NOT NULL COMMENT '所属角色id',
  `sells` varbinary(10301) DEFAULT NULL COMMENT '出售物品',
  PRIMARY KEY (`id`),
  KEY `sellhistory_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `sellhostory_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `signin`
--

DROP TABLE IF EXISTS `signin`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `signin` (
  `actorid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '角色id',
  `signs` varbinary(1024) DEFAULT NULL COMMENT '签到记录',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `signin_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `signin_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `snarehistory`
--

DROP TABLE IF EXISTS `snarehistory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `steprank`
--

DROP TABLE IF EXISTS `steprank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `taskhistory`
--

DROP TABLE IF EXISTS `taskhistory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
) ENGINE=InnoDB AUTO_INCREMENT=32 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `vip`
--

DROP TABLE IF EXISTS `vip`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `vip` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '记录id',
  `actorid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '角色id',
  `vip` tinyint(4) DEFAULT '0' COMMENT 'VIP等级',
  `reward` tinyint(4) DEFAULT '0' COMMENT 'VIP等级对应奖励是否领取(0-未领取, 1-领取)',
  `tts` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '领取奖励时间',
  PRIMARY KEY (`id`),
  KEY `vip_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `vip_ifbk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `vitexchange`
--

DROP TABLE IF EXISTS `vitexchange`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `vitexchange` (
  `actorid` int(11) unsigned NOT NULL COMMENT '角色id',
  `exchangetimes` smallint(6) unsigned DEFAULT NULL COMMENT '当天兑换次数',
  `tts` timestamp NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后一次兑换时间',
  PRIMARY KEY (`actorid`),
  UNIQUE KEY `vitexchange_idx_1` (`actorid`) USING BTREE,
  CONSTRAINT `vitexchange_ifbk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`) ON DELETE CASCADE
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

-- Dump completed on 2015-11-28 15:02:47
