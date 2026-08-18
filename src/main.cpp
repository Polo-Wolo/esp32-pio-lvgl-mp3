#include <Arduino.h>

#include "playback/playback_manager.h"

PlaybackManager playback;

void setup()
{
	Serial.begin(115200);

	delay(1000);

	Serial.println();
	Serial.println("===== PLAYBACK TEST =====");

	MusicTrack track1(
		"/Music/Daft Punk/Discovery/01.mp3",
		"One More Time",
		"Daft Punk",
		"Discovery",
		1);

	MusicTrack track2(
		"/Music/Daft Punk/Discovery/02.mp3",
		"Aerodynamic",
		"Daft Punk",
		"Discovery",
		2);

	MusicTrack track3(
		"/Music/Daft Punk/Discovery/03.mp3",
		"Digital Love",
		"Daft Punk",
		"Discovery",
		3);

	playback.add(track1);
	playback.add(track2);
	playback.add(track3);

	Serial.println("Initial current:");

	const MusicTrack *track = playback.current();

	if (track)
	{
		track->print();
	}

	Serial.println();
	Serial.println("Selecting index 2...");

	if (playback.setCurrent(2))
	{
		Serial.println("Selection successful.");
	}
	else
	{
		Serial.println("Selection failed.");
	}

	Serial.println();
	Serial.println("Current:");

	track = playback.current();

	if (track)
	{
		track->print();
	}

	Serial.println();
	Serial.println("Selecting invalid index 10...");

	if (playback.setCurrent(10))
	{
		Serial.println("Selection successful.");
	}
	else
	{
		Serial.println("Selection failed.");
	}

	Serial.println();
	playback.printQueue();
}

void loop()
{
}