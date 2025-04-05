#include <M5Cardputer.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <Audio.h>

String data = "";
String settings[3];

Audio audio;

void setup() {
	auto cfg = M5.config();
	M5Cardputer.begin(cfg);
	M5Cardputer.Speaker.begin();

	SPI.begin(40, 39, 14, 12);

	if (!SD.begin(12)) {
		M5Cardputer.Display.println("Failed to mount SD card");
		return;
	}

	File file = SD.open("/settings.txt");
	if (!file) {
    M5Cardputer.Display.println("Failed to open settings file");
    return;
  }

	int index = 0;
	while (file.available() && index < 3) {
    settings[index++] = file.readStringUntil('\n');
		settings[index - 1].trim();
  }

	file.close();

	M5Cardputer.Display.printf("Connecting to '%s'", settings[0]);

	WiFi.begin(settings[0], settings[1]);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		M5Cardputer.Display.print(".");
	}

	M5Cardputer.Display.println();

	M5Cardputer.Display.println("Connected!");

	audio.setPinout(41, 43, 42);
	audio.setVolume(21);

	M5Cardputer.Display.println("Press any key to continue.");
}

void loop() {
	M5Cardputer.update();
	audio.loop();

	if (M5Cardputer.Keyboard.isChange()) {
		if (M5Cardputer.Keyboard.isPressed()) {
			Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

			for (auto i : status.word) {
				data += i;
			}

			if (status.del) {
				data.remove(data.length() - 1);
			}

			if (status.enter) {
				audio.connecttospeech(data.c_str(), settings[2].c_str());
			}

			M5Cardputer.Display.clear();

			M5Cardputer.Display.setCursor(0, 0);

			M5Cardputer.Display.println("Enter a text to say:");
			M5Cardputer.Display.print(data);
			M5Cardputer.Display.print("_");
		}
	}
}
