#pragma once

// ���庯��ָ��
//typedef int(*handleFunc)(void* arg);
using handleFunc = int(*)(void* arg);

// �����ļ��������Ķ�д�¼�
enum class FDEvent
{
    TimeOut = 0x01,
    ReadEvent = 0x02,
    WriteEvent = 0x04
};

class Channel
{
public:
    Channel(int fd, int events, handleFunc readFunc, handleFunc writeFunc, handleFunc destroyFunc, void* arg);
    // �ص�����
    handleFunc readCallback;
    handleFunc writeCallback;
    handleFunc destroyCallback;
    // �޸�fd��д�¼�(��� or �����)
    void writeEventEnable(bool flag);
    // �ж��Ƿ���Ҫ����ļ���������д�¼�
    bool isWriteEventEnable();
    //ȡ��˽�г�Ա��ֵ��������
    inline int getEvent()
    {
        return m_events;
    }
    inline int getSocket()
    {
        return m_fd;
    }
    inline const void* arg()
    {
        return m_arg;
    }
private:
     // �ļ�������
    int m_fd;
    // �¼�
    int m_events;
    // �ص������Ĳ���
    void* m_arg;
};

