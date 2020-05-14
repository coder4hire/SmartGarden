#include "HWInterface.h"
#include "wiringPi.h"
#include "PinsDefinitions.h"

#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <fcntl.h>

#define SOUND_CFG_PATH "/etc/DispKeyb/sound.cfg"

int CHWInterface::OutNum2Pin[] = { 0,PIN_OUT1,PIN_OUT2,PIN_OUT3,PIN_OUT4 };

CHWInterface CHWInterface::Inst;

CHWInterface::CHWInterface()
{
}

CHWInterface::~CHWInterface()
{
}

bool CHWInterface::SetOutValue(unsigned int port, bool enable)
{
	if (port < 1 || port > 4)
	{
		return false;
	}
	digitalWrite(OutNum2Pin[port], enable);
	return true;}

bool CHWInterface::GetOutStatus(unsigned int port)
{
	if (port < 1 || port > 4)
	{
		return false;
	}
	return digitalRead(OutNum2Pin[port]);
}

int CHWInterface::ALSAVolume(long& vol, bool isReading)
{
    snd_mixer_t* handle;
    snd_mixer_elem_t* elem;
    snd_mixer_selem_id_t* sid;

    static const char* mix_name = "DAC";
    static const char* card = "default";
    static int mix_index = 0;

    long pmin, pmax;
    long get_vol, set_vol;
    float f_multi;

    snd_mixer_selem_id_alloca(&sid);

    //sets simple-mixer index and name
    snd_mixer_selem_id_set_index(sid, mix_index);
    snd_mixer_selem_id_set_name(sid, mix_name);

    if ((snd_mixer_open(&handle, 0)) < 0)
        return -1;
    if ((snd_mixer_attach(handle, card)) < 0) {
        snd_mixer_close(handle);
        return -2;
    }
    if ((snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
        snd_mixer_close(handle);
        return -3;
    }
    int ret = snd_mixer_load(handle);
    if (ret < 0) {
        snd_mixer_close(handle);
        return -4;
    }
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem) {
        snd_mixer_close(handle);
        return -5;
    }

    long minv, maxv;

    snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv);
    //fprintf(stderr, "Volume range <%i,%i>\n", minv, maxv);

    if (isReading) {
        if (snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &vol) < 0) {
            snd_mixer_close(handle);
            return -6;
        }

        //fprintf(stderr, "Get volume %i with status %i\n", vol, ret);
        /* make the value bound to 100 */
        vol -= minv;
        maxv -= minv;
        minv = 0;
        vol = 100 * vol / maxv; // make the value bound from 0 to 100
        vol = (vol + 4) / 10 * 10; // rounding to the step of 10
    }
    else {
        if (vol < 0 || vol > 100) // out of bounds
            return -7;
        vol = (vol * (maxv - minv) / (100 - 1)) + minv;

        if (snd_mixer_selem_set_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, vol) < 0) {
            snd_mixer_close(handle);
            return -8;
        }
        if (snd_mixer_selem_set_playback_volume(elem, SND_MIXER_SCHN_FRONT_RIGHT, vol) < 0) {
            snd_mixer_close(handle);
            return -9;
        }
        //fprintf(stderr, "Set volume %i with status %i\n", vol, ret);
    }

    snd_mixer_close(handle);
    return 0;
}

void CHWInterface::SaveVolume()
{
    long vol;
    if (!ALSAVolume(vol, true))
    {
        if (FILE* fp = fopen(SOUND_CFG_PATH,"wb"))
        {
            fwrite(&vol, 1, sizeof(vol), fp);
            fclose(fp);
        }
        else
        {
            fprintf(stderr, "Error while storing volume data\n");
        }
    }
}

void CHWInterface::RestoreVolume()
{
    long vol;
    if (FILE* fp = fopen(SOUND_CFG_PATH,"rb"))
    {
        fread(&vol, 1, sizeof(vol), fp);
        fclose(fp);
        if (vol >= 0 && vol <= 100)
        {
            ALSAVolume(vol, false);
        }
    }
    else
    {
        fprintf(stderr, "Error while reading volume data\n");
    }

}
