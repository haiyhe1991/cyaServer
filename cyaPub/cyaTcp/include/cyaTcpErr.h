#ifndef __CYA_TCP_ERR_H__
#define __CYA_TCP_ERR_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif


#define		CYATCP_OK						0		//�����ɹ�
#define		CYATCP_ERROR					-1		//δ֪����
#define		CYATCP_INVALID_PARAM			-10001	//�������Ϸ�
#define		CYATCP_ERROR_USE				-10002	//�÷�����
#define		CYATCP_SESSION_CLOSING			-10003	//�Ự���ڹر���
#define		CYATCP_INNER_ERROR				-10004	//�ڲ�����
#define		CYATCP_INVALID_SESSION_SIZE		-10005	//�����session��С
#define		CYATCP_SYSTEM_ERROR				-10006	//ϵͳ����
#define		CYATCP_SERVE_STARTED			-10007	//������������
#define		CYATCP_SESSION_USING			-10008	//�Ự����ʹ����
#define		CYATCP_SESSION_BROKEN			-10009	//�Ự����
#define		CYATCP_INVALID_READ_SIZE		-10010	//Ҫ��ȡ�����ݳ��ȷǷ�
#define		CYATCP_ALLOC_MEM_FAILED			-10011	//�����ڴ�ʧ��
#define		CYATCP_SENDBUF_IS_FULL			-10012	//�Ự���ͻ�������
#define		CYATCP_PEER_CLOSED				-10013	//�Զ˹ر�����
#define		CYATCP_SELF_ACTIVE_CLOSED		-10014	//�Լ������ر�����
#define		CYATCP_KEEPALIVE_EXPIRED		-10015	//���ʱ�����,�ر�����

#endif