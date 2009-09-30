#include "VSQEventManager.h"

EventManager::~EventManager()
{
	for(list<ControlEvent*>::iterator i=EventList.begin();i!=EventList.end();i++){
		if((*i)!=NULL)
			delete (*i);
	}
}
int		EventManager::GetValue(long nTick)
{
	if(nTick>10890){
		int a=0;
		a++;
	}
	if(EventList.size()==0)
		return -1;
	//���`�ɒl��T���B
	list<ControlEvent*>::iterator i=EventList.begin();

	//�ŏ��̃C�x���g���O
	if(nTick<(*i)->nTick)
		return -1;

	for(i=EventList.begin();i!=EventList.end();i++){
		if((*i)->nTick>nTick){
			if(i!=EventList.begin() && (*i)->nTick!=nTick)
				i--;
			return (*i)->iValue;
		}
	}
	if(i!=EventList.begin())
		i--;

	//������Ȃ���΍Ō�̗v�f�̒l��Ԃ�
	return (*i)->iValue;
}


bool	EventManager::SetValue(long nTick,int iValue)
{
	//�}���������f�[�^���\���̂Ɋi�[����B
	ControlEvent* pEvent = new ControlEvent;
	pEvent->nTick=nTick;
	pEvent->iValue=iValue;

	//�������s��
	if(NULL==pEvent)
		return false;

	//���X�g����Ȃ�擪�ɑ}������B
	if(EventList.size()==0){
		EventList.push_front(pEvent);
		return true;
	}

	//��������v������l��ύX���邾��
	for(list<ControlEvent*>::iterator i=EventList.begin();i!=EventList.end();i++){
		if((*i)->nTick==nTick){
			delete pEvent;				//�]���Ɋm�ۂ����̂ŊJ��
			(*i)->iValue=iValue;
			return true;
		}else if((*i)->nTick>nTick){

			//��������v���Ȃ���Βl��ݒ肷��
			EventList.insert(i,pEvent);
			return true;
		}
	}

	//�����ꏊ���ǂ��ɂ��Ȃ���Έ�ԍŌ�ɓ����
	EventList.push_back(pEvent);

	return true;
}

bool	EventManager::SetValue(string sFront,string sBack)
{
	return SetValue(atol(sFront.c_str()),atoi(sBack.c_str()));
}

