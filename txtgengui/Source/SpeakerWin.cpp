//
//  Speaker.cpp
//  txtgen - App
//
//  Created by joerg piringer on 02.10.17.
//

#include "Speaker.hpp"

#include "utils.hpp"

#include <sapi.h>


namespace jp {
    class SpeakerWin : public Speaker {
		ISpVoice *pVoice = NULL;
		bool valid = false;
        
    public:
		SpeakerWin() {
			if (!FAILED(::CoInitialize(NULL))) {
				HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
				if (SUCCEEDED(hr)) {
					valid = true;
				}
			}
        }
        
        ~SpeakerWin() {
			if (valid) {
				pVoice->Release();
				pVoice = nullptr;

				::CoUninitialize();
			}
        }

        void speak(const std::string &str) override {
			if (valid) {
				HRESULT hr = pVoice->Speak(fromUTF8(str).c_str(), SPF_ASYNC | SPF_PURGEBEFORESPEAK, nullptr);
				pVoice->Resume();
			}
        }
        
        void stop() override {
			if (valid) {
				pVoice->Pause();
			}
        }
    };
}
        
std::shared_ptr<jp::Speaker> createSpeakerInstance() {
    return std::make_shared<jp::SpeakerWin>();
}
