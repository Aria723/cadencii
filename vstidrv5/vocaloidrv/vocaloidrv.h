#include "../vstidrv.h"

void print_help();

class vocaloidrv : public vstidrv
{
public:
    vocaloidrv( string path ) : vstidrv( path ){
        mIsRendering = false;
        mFile = NULL;
    };

    ~vocaloidrv();

    bool open( int block_size, int sample_rate );

    bool vocaloidrv::sendEvent( unsigned char *midi_data, int *clock_data, int num_data, int targetTrack );

    /// <summary>
    /// 
    /// </summary>
    /// <param name="total_samples"></param>
    /// <param name="mode_infinite"></param>
    /// <param name="sample_rate"></param>
    /// <param name="runner">���̃h���C�o���쓮���Ă���RenderingRunner�̃I�u�W�F�N�g</param>
    /// <returns></returns>
    int startRendering( long total_samples, bool mode_infinite, int sample_rate );

    bool isRendering()
    {
        return mIsRendering;
    };

    void abortRendering()
    {
        mIsCancelRequired = true;
    };

private:
    /// <summary>
    /// �w�肵���^�C���R�[�h�ɂ�����C�ȓ����瑪�������Ԃ𒲂ׂ�
    /// </summary>
    double msec_from_clock( int timeCode );

    // �g�`�̏o�͏������s���D�߂�l��true�̏ꍇ�C�g�`�������ɒ��f�v�����s��ꂽ���Ƃ�\��
    bool waveIncoming( double *left, double *right, int length );

    static void merge_events( vector<MidiEvent *> &x0, vector<MidiEvent *> &y0, vector<MidiEvent *> &dst );

private:
    static const int TIME_FORMAT = 480;
    static const int DEF_TEMPO = 500000;           // �f�t�H���g�̃e���|�D

    vector<MidiEvent *> mEvents0;
    vector<MidiEvent *> mEvents1;
    vector<TempoInfo *> mTempoList;
    bool mIsCancelRequired;
    /// <summary>
    /// StartRendering���\�b�h������Ă���Œ���true
    /// </summary>
    bool mIsRendering;
    FILE *mFile;

};
