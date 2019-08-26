#include <iostream>
#include <windows.h>
#include <mmsystem.h>

#pragma comment( lib, "Winmm.lib" )

typedef struct {
	char RIFF[4];
	DWORD bytes;
	char WAVE[4];
	char fmt[4];
	int siz_wf;
	WORD wFormatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD wBitsPerSample;
	char data[4];
	DWORD pcmbytes;
} WAVHEADER;

int main()
{
	int micCount = waveInGetNumDevs();
	if (micCount < 1) {
		std::cout << "Mic not exist. Need add mic in computer" << std::endl;
		exit(1);
	}
	std::cout << micCount << std::endl;
	const int second = 5;
	const int SampleRate = 44100;
	const int N = SampleRate * 2 * second;
	short int buffer[N];

	WAVEFORMATEX wfex;
	wfex.wFormatTag = WAVE_FORMAT_PCM;
	wfex.nChannels = 2;
	wfex.wBitsPerSample = 16;
	wfex.nSamplesPerSec = SampleRate;
	wfex.nAvgBytesPerSec = SampleRate * wfex.nChannels * wfex.wBitsPerSample / 8;
	wfex.nBlockAlign = wfex.nChannels * wfex.wBitsPerSample / 8;
	wfex.cbSize = 0;

	HWAVEIN hWaveIn;
	waveInOpen(&hWaveIn, WAVE_MAPPER, &wfex, 0L, 0L, WAVE_FORMAT_DIRECT);

	WAVEHDR WaveHdr;
	WaveHdr.lpData = (LPSTR)buffer;
	WaveHdr.dwBufferLength = N * 2;
	WaveHdr.dwBytesRecorded = 0;
	WaveHdr.dwUser = 0L;
	WaveHdr.dwFlags = 0L;
	WaveHdr.dwLoops = 0L;
	waveInPrepareHeader(hWaveIn, &WaveHdr, sizeof(WAVEHDR));
	waveInAddBuffer(hWaveIn, &WaveHdr, sizeof(WAVEHDR));
	waveInStart(hWaveIn);
	while (waveInUnprepareHeader(hWaveIn, &WaveHdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING);
	waveInClose(hWaveIn);

	WAVHEADER wh;
	memcpy(wh.RIFF, "RIFF", 4);
	memcpy(wh.WAVE, "WAVE", 4);
	memcpy(wh.fmt, "fmt ", 4);
	memcpy(wh.data, "data", 4);
	wh.siz_wf = 16;
	wh.wFormatTag = WAVE_FORMAT_PCM;
	wh.nChannels = wfex.nChannels;
	wh.wBitsPerSample = wfex.wBitsPerSample;
	wh.nSamplesPerSec = SampleRate;
	wh.nAvgBytesPerSec = SampleRate * wh.nChannels * wh.wBitsPerSample / 8;
	wh.nBlockAlign = wh.nChannels * wh.wBitsPerSample / 8;
	wh.bytes = N * 2 + 36;
	wh.pcmbytes = N * 2;

	HANDLE hFile = CreateFile("audio.wav", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile)
	{
		DWORD dwWrite;
		WriteFile(hFile, &wh, sizeof(WAVHEADER), &dwWrite, NULL);
		WriteFile(hFile, WaveHdr.lpData, WaveHdr.dwBufferLength, &dwWrite, NULL);
		CloseHandle(hFile);
	}


	system("pause");
	return 0;
}
