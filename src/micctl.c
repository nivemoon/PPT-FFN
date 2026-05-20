#define COBJMACROS
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

/* Свои GUID, чтобы не конфликтовать с DEFINE_GUID в заголовках */

static const CLSID MY_CLSID_MMDeviceEnumerator =
{ 0xBCDE0395, 0xE52F, 0x467C,{ 0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E } };

static const IID MY_IID_IMMDeviceEnumerator =
{ 0xA95664D2, 0x9614, 0x4F35,{ 0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6 } };

static const IID MY_IID_IAudioEndpointVolume =
{ 0x5CDF2C82, 0x841E, 0x4546,{ 0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A } };

int set_mic_mute(int mute) {
    HRESULT hr;
    IMMDeviceEnumerator *pEnum = NULL;
    IMMDevice *pDevice = NULL;
    IAudioEndpointVolume *pEndpoint = NULL;

    hr = CoInitialize(NULL);
    if (FAILED(hr)) return 1;

    hr = CoCreateInstance(
        &MY_CLSID_MMDeviceEnumerator,
        NULL,
        CLSCTX_ALL,
        &MY_IID_IMMDeviceEnumerator,
        (void**)&pEnum
    );
    if (FAILED(hr) || !pEnum) goto cleanup;

    hr = IMMDeviceEnumerator_GetDefaultAudioEndpoint(
        pEnum,
        eCapture,        /* устройство записи */
        eCommunications, /* default communications device (см. "устройство связи по умолчанию") */
        &pDevice
    );
    if (FAILED(hr) || !pDevice) goto cleanup;

    hr = IMMDevice_Activate(
        pDevice,
        &MY_IID_IAudioEndpointVolume,
        CLSCTX_ALL,
        NULL,
        (void**)&pEndpoint
    );
    if (FAILED(hr) || !pEndpoint) goto cleanup;

    hr = IAudioEndpointVolume_SetMute(pEndpoint, mute, NULL);

cleanup:
    if (pEndpoint) IAudioEndpointVolume_Release(pEndpoint);
    if (pDevice)   IMMDevice_Release(pDevice);
    if (pEnum)     IMMDeviceEnumerator_Release(pEnum);
    CoUninitialize();
    return FAILED(hr) ? 1 : 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        /* 0 = unmute, 1 = mute */
        return 1;
    }

    int mute = (argv[1][0] == '1') ? 1 : 0;
    return set_mic_mute(mute);
}
