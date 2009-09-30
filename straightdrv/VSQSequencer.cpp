#include "VSQSequencer.h"

VSQSequencer::VSQSequencer()
{
	//�e�m�[�g�ɑΉ�������g�������炩���ߌv�Z
	for(int i=0;i<128;i++)
		dNoteFrequency[i]=A4_PITCH*pow(2.0,(i-A4_NOTE)/12.0);
	//�������p
	dNoteFrequency[128]=0.0;

	//�R���g���[���g���b�N�̒ǉ�
	for(int i=0;i<CONTROL_TRACK_NUM;i++){
		EventManager* pTarget=new EventManager;

		//Tick=0�Ƀf�t�H���g�̒l����������
		pTarget->SetValue(0,iControlDefaultValue[i]);

		EventMap.insert(make_pair(sControlNames[i],pTarget));
		ManagerList.push_back(pTarget);
	}

	NoteManager* pTarget = new NoteManager;
	EventMap.insert(make_pair(sNoteTrackName,pTarget));
	ManagerList.push_back(pTarget);

	dTempo=0.0;
}

VSQSequencer::~VSQSequencer()
{
	UtauManager.Uninitialize();
	for(list<EventManager*>::iterator i=ManagerList.begin();i!=ManagerList.end();i++){
		SAFE_DELETE((*i));
	}
}

bool	VSQSequencer::LoadFile(string sFileName, double dFrameShift)
{
	FILE* ifs;
    const int BUF_LEN = 4096;
	char cBuf[BUF_LEN];
	string sBuf;
	string sCurrentLoading="";
	MAP_TYPE<string,EventManager*>::iterator h_i;
	bool bResult=true;
	int iCurrentLine=0;
	bool bOtoFlag=false;

	this->dFrameShift=dFrameShift;

#ifdef __GNUC__
    ifs = fopen( sFileName.c_str(), "r" );
#else
    fopen_s( &ifs, sFileName.c_str(), "r" );
#endif

	if( NULL == ifs ){
		cout << "error; io error on file:" << sFileName.c_str() << endl;
		return false;
	}

	//�V�[�P���X�t�@�C���̉���
	while( NULL != fgets( cBuf, BUF_LEN, ifs ) ){
        char *p = strchr( cBuf, '\n' );
        if( NULL != p ) *p = '\0';
		sBuf = cBuf;
        iCurrentLine++;

		//��������P�F�R�����g�s�y�ы�s�͓ǂݔ�΂��B
		if(sBuf.size()==0 || sBuf.find("//")==0)	//�ǂݔ�΂�����
			continue;

		//��������Q�Foto.ini�̐ݒ�̎��͏���ɂ��B
		else if(sBuf.compare("[oto.ini]")==0){
            if( NULL == fgets( cBuf, BUF_LEN, ifs ) ){
                bOtoFlag = false;
                break;
            }
            p = strchr( cBuf, '\n' );
            if( NULL != p ) *p = '\0';
			sBuf=cBuf;
			iCurrentLine++;

			bOtoFlag=UtauManager.Initialize(sBuf);

			if(!bOtoFlag){
				cout << "error; parser error for oto.ini; line=" << sBuf.c_str() << endl;
			}else
				sFilePath=sBuf.substr(0,sBuf.find("oto.ini"));

		//��������R�F�e���|�w��̏ꍇ������ɂ��B
		}else if(sBuf.compare("[Tempo]")==0){
            if( NULL == fgets( cBuf, BUF_LEN, ifs ) ){
                dTempo = 0.0;
                break;
            }
            p = strchr( cBuf, '\n' );
            if( NULL != p ) *p = '\0';
			iCurrentLine++;
			dTempo=atof(cBuf);

		//��������S�FEOS��������I�[Tick���m�ۂ���B
		}else if(sBuf.find("EOS")!=string::npos){
			nEndTick=atol(sBuf.substr(0,sBuf.find("=")).c_str());

		//��������T�F[ID#xxxx]�̎���[h#xxxx]��EventList�̓ǂݍ��݈���
		}else if(sBuf.find("[ID#")!=string::npos || sBuf.find("[h#")!=string::npos){

			//���ꂩ��ǂݍ���ID��ݒ肵�Ă����B
			h_i=EventMap.find(sNoteTrackName);
			if(h_i!=EventMap.end()){
				NoteManager* pNoteManager;
				pNoteManager=dynamic_cast<NoteManager*>(h_i->second);

				//�������ނׂ��m�[�g�Ǘ��N���X���Ȃ���΃G���[
				if(pNoteManager!=NULL){
					if(sBuf.find("[ID#")!=string::npos)
						pNoteManager->SetEditNoteID(atoi(sBuf.substr(4,sBuf.find("]")).c_str()));
					else if(sBuf.find("[h#")!=string::npos)
						pNoteManager->SetHandleID(atoi(sBuf.substr(3,sBuf.find("]")).c_str()));
				}
				else
					bResult=false;
			}

		//�ʏ폈���P�F[xxxxxxxx]�̏ꍇxxxxxxx�����ݓǂݍ���ł��郊�X�g���ɐݒ�B
		}else if(sBuf.find("[")==0){
			sCurrentLoading=sBuf;

		//�ʏ폈���Q�F�l��ݒ肵�Ă���s�̏ꍇ�͌��݂̃��X�g�ɕ������n���B
		}else{
			h_i=EventMap.find(sCurrentLoading);
			if(h_i!=EventMap.end())
				bResult=h_i->second->SetValue(sBuf.substr(0,sBuf.find("=")),sBuf.substr(sBuf.find("=")+1));
			else{
				cout << "error; file: " << sFileName.c_str() << "; unexpected token: " << sCurrentLoading.c_str() <<endl;
//				bResult=false;
			}
		}

		//�G���[����
		if(!bResult){
			cout << "error; line#: " << iCurrentLine << endl;
            if( NULL != ifs ){
                fclose( ifs );
            }
			return false;
		}
	}

    if( NULL != ifs ){
        fclose( ifs );
    }


	//�f�t�H���g�̐ݒ��ǂݍ���
	ifstream ifsDef;
	ifsDef.open("defaultSetting.ini");

	if(!ifsDef.fail()){
		ifsDef.getline(cBuf,sizeof(cBuf));
		sBuf=cBuf;

		if(!bOtoFlag)
			if((bOtoFlag=UtauManager.Initialize(sBuf)))
				sFilePath=sBuf.substr(0,sBuf.find("oto.ini"));

		ifsDef.getline(cBuf,sizeof(cBuf));

		if(dTempo==0.0)
			dTempo=atof(cBuf);
	}

	//�f�t�H���g�ݒ��ǂݍ���ł��Ȃ����߂ȂƂ�
	if(!bOtoFlag){
        return false;
	}


	//�e���|�ݒ肪inputFile���ɑ��݂��Ȃ��ꍇ
	if(dTempo==0.0){
        return false;
	}

	h_i=EventMap.find(sNoteTrackName);
	if(h_i!=EventMap.end()){
		NoteManager* pNoteManager;
		pNoteManager=dynamic_cast<NoteManager*>(h_i->second);

		//�������ނׂ��m�[�g�Ǘ��N���X���Ȃ���΃G���[
		if(pNoteManager!=NULL){
			bResult=pNoteManager->CheckContinuity(dTempo,&UtauManager,dFrameShift);
			if(!bResult)
				cout << "error; cannot find note. synthesize will be omitted." << endl;
			nEndNoteID=pNoteManager->GetEndID();
			nEndFrame=pNoteManager->GetEndFrame();
			nBeginFrame=pNoteManager->GetBeginFrame();
		}else{
			cout << "error; cannot find note manager" << endl;
			bResult=false;
		}
	}
	
	return bResult;
}


NoteEvent* VSQSequencer::GetNoteEvent(long nFrame)
{
	NoteEvent* pResult=NULL;
	NoteManager* pNoteManager=NULL;

	MAP_TYPE<string,EventManager*>::iterator h_i;

	h_i=EventMap.find(sNoteTrackName);

	if(h_i!=EventMap.end()){
		pNoteManager=dynamic_cast<NoteManager*>(h_i->second);

		//�������ނׂ��m�[�g�Ǘ��N���X���Ȃ���΃G���[
		if(pNoteManager!=NULL){
			pResult=pNoteManager->GetNoteEventByFrame(nFrame);
		}
	}

	return pResult;
}

double	VSQSequencer::GetF0(long nFrame,double dFrameShift)
{
	double dResult=0.0;
	double dRate;
	double dTemp;

	NoteEvent* pNoteEvent=GetNoteEvent(nFrame);

	if(pNoteEvent==NULL)
		return dResult;

	long nRelativeFrame=nFrame-(long)((double)(pNoteEvent->nTick)/TICK_PER_SEC/dTempo*1000.0/dFrameShift);

	dResult=GetPitchFromNote(pNoteEvent,nFrame,dFrameShift);

	//�㑱��������ꍇ�A�Ō��50ms�͉����̃��[�t�B���O���s��
	if(pNoteEvent->bIsContinuousBack && 50>((pNoteEvent->nEndFrame-nFrame)*dFrameShift)){

		double dBendBeginFrame=(double)(pNoteEvent->nEndFrame)-50.0/dFrameShift;

		dTemp=GetPitchFromNote(pNoteEvent->pNextNote,nFrame,dFrameShift);

		dResult=pow(dResult,((double)(pNoteEvent->nEndFrame)-(double)nFrame)/((double)(pNoteEvent->nEndFrame)-dBendBeginFrame))
				*pow(dTemp,1.0-((double)(pNoteEvent->nEndFrame)-(double)nFrame)/((double)(pNoteEvent->nEndFrame)-dBendBeginFrame));
	}

	dRate=GetPitchBendRate(nFrame,dFrameShift);
	dResult*=dRate;

	return dResult;
}

double	VSQSequencer::GetDynamics(long nFrame,double dFrameShift)
{
	double dResult=1.0;
	double dRate;
	double dTemp;

	NoteEvent* pNoteEvent=GetNoteEvent(nFrame);

	if(pNoteEvent==NULL)
		return dResult;

	long nRelativeFrame=nFrame-(long)((double)(pNoteEvent->nTick)/TICK_PER_SEC/dTempo*1000.0*dFrameShift);

	dResult=GetDynamicsFromNote(pNoteEvent,nFrame,dFrameShift);

	if(pNoteEvent->bIsContinuousBack && 50<((nFrame-pNoteEvent->nEndFrame)*dFrameShift)){
		double dBendBeginFrame=(double)(pNoteEvent->nEndFrame)-50.0/dFrameShift;

		dTemp=GetDynamicsFromNote(pNoteEvent->pNextNote,nFrame,dFrameShift);

		dResult=dResult*(1.0-((double)(pNoteEvent->nEndFrame)-(double)nFrame)/((double)(pNoteEvent->nEndFrame)-dBendBeginFrame))
				+dTemp*((double)(pNoteEvent->nEndFrame)-(double)nFrame)/((double)(pNoteEvent->nEndFrame)-dBendBeginFrame);
	}

	dRate=GetDynamicsRate(nFrame,dFrameShift);
	dResult*=dRate;

	return dResult;
}

double VSQSequencer::GetPitchFromNote(NoteEvent *pNoteEvent,long nFrame,double dFrameShift)
{
	if(pNoteEvent==NULL)
		return 0.0;

	double dRate;
	double dTemp=dNoteFrequency[pNoteEvent->ucNote];

	dRate=pow(2.0,1.0/12.0*GetVibratoRate(pNoteEvent,nFrame,dFrameShift));
	dTemp*=dRate;

	return dTemp;
}

double VSQSequencer::GetPitchBendRate(long nFrame, double dFrameShift)
{
	double dRate=1.0;
	int iPitchBend,iPitchBendSensitivity;

	iPitchBend=GetControlValue(PITCH_BEND,nFrame,dFrameShift);
	iPitchBendSensitivity=GetControlValue(PITCH_SENS,nFrame,dFrameShift);

	dRate=pow(2.0,(double)iPitchBendSensitivity*(double)iPitchBend/98304.0);

	return dRate;
}


double VSQSequencer::GetVibratoRate(NoteEvent *pNoteEvent, long nFrame, double dFrameShift)
{
	double dResult,dTheta;
	int iVibratoDepth,iVibratoRate;
	long nRelativeFrame=nFrame-(long)((double)(pNoteEvent->nTick)/TICK_PER_SEC/dTempo*1000.0*dFrameShift);

	iVibratoDepth=pNoteEvent->emVibratoDepth.GetValue((long)((double)(nRelativeFrame)*dFrameShift*dTempo*TICK_PER_SEC/1000.0));
	iVibratoRate=pNoteEvent->emVibratoRate.GetValue((long)((double)(nRelativeFrame)*dFrameShift*dTempo*TICK_PER_SEC/1000.0));

	dTheta=ST_PI*((double)nRelativeFrame-(double)(pNoteEvent->nVibratoDelay)/dTempo/TICK_PER_SEC*1000.0/dFrameShift)*(double)iVibratoRate/128.0/80.0;

	dResult=(double)iVibratoDepth/128.0*sin(dTheta);

	return dResult;

}

double  VSQSequencer::GetDynamicsRate(long nFrame,double dFrameShift)
{
	double dRate;
	int iDynamics;

	iDynamics=GetControlValue(DYNAMICS,nFrame,dFrameShift);

	dRate=(double)iDynamics/64.0;

	return dRate;
}

double	VSQSequencer::GetDynamicsFromNote(NoteEvent *pNoteEvent, long nFrame, double dFrameShift)
{
	double dRate=1.0;

	double dAccentRate=0.25+pow(2.0,((double)(pNoteEvent->ucAccent)-50.0)/50.0);
	double dDecayRate=1.0-(double)(pNoteEvent->ucDecay)/100.0;

	long nAttack=(long)(150.0/dFrameShift);
	long nRelativeFrame=nFrame-pNoteEvent->nBeginFrame;

	if(pNoteEvent->nEndFrame-pNoteEvent->nBeginFrame<nAttack)
		nAttack=pNoteEvent->nEndFrame-pNoteEvent->nBeginFrame;

	//Attack����
	if(nRelativeFrame<=nAttack){
		dRate=1.0*(double)nRelativeFrame/(double)nAttack+dAccentRate*(1.0-(double)nRelativeFrame/(double)nAttack);
		dRate=1.0+dRate*sin(ST_PI/(double)nAttack);
	}else{
		dRate=1.0*(1.0-(double)(nRelativeFrame-nAttack)/(double)(pNoteEvent->nEndFrame-nAttack-pNoteEvent->nBeginFrame))
			+dDecayRate*(double)(nRelativeFrame-nAttack)/(double)(pNoteEvent->nEndFrame-nAttack-pNoteEvent->nBeginFrame);
	}

	dRate+=0.1*GetVibratoRate(pNoteEvent,nFrame,dFrameShift);

	//���̉����������ꍇ��Release����
	if(pNoteEvent->bIsContinuousBack==false){
		if(nFrame>=pNoteEvent->nEndFrame-100.0/dFrameShift){
			long nReleaseBegin=(long)(pNoteEvent->nEndFrame-100.0/dFrameShift);
			if(nReleaseBegin<pNoteEvent->nBeginFrame)
				nReleaseBegin=pNoteEvent->nBeginFrame;

			dRate*=(1.0-(double)(nFrame-nReleaseBegin)/(double)(pNoteEvent->nEndFrame-nReleaseBegin));
		}
	}

	return dRate;
}

int		VSQSequencer::GetControlValue(int iControlNum,long nFrame,double dFrameShift)
{
	if(iControlNum < 0 || iControlNum >=CONTROL_TRACK_NUM)
		return -1;

	MAP_TYPE<string,EventManager*>::iterator h_i;
	int iResult=-1;
	EventManager* pEventManager;
	long nTick=(long)((double)nFrame*dFrameShift*dTempo*TICK_PER_SEC/1000.0);

	h_i=EventMap.find(sControlNames[iControlNum]);

	if(h_i!=EventMap.end()){
		pEventManager=h_i->second;
		if(pEventManager)
			if((iResult=pEventManager->GetValue(nTick))==-1)
				iResult=iControlDefaultValue[iControlNum];
	}

	return iResult;
}
