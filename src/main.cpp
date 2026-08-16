#include <Arduino.h>

#include "playback/MusicTrack.h"
#include "playback/PlaybackQueue.h"

PlaybackQueue queue;

void setup()
{
	Serial.begin(115200);
	delay(1000);

	Serial.println();
	Serial.println("===== PLAYBACK QUEUE TEST =====");

	MusicTrack track1(
		"/music/Daft Punk/01.mp3",
		"Give Life Back to Music",
		"Daft Punk",
		"Random Access Memories",
		1);

	MusicTrack track2(
		"/music/Daft Punk/02.mp3",
		"Game of Love",
		"Daft Punk",
		"Random Access Memories",
		2);

	MusicTrack track3(
		"/music/Daft Punk/03.mp3",
		"Giorgio by Moroder",
		"Daft Punk",
		"Random Access Memories",
		3);

	queue.add(track1);
	queue.add(track2);
	queue.add(track3);

	Serial.printf("Queue size: %u\n", queue.size());

	for (size_t i = 0; i < queue.size(); i++)
	{
		const MusicTrack *track = queue.get(i);

		if (track)
		{
			Serial.printf(
				"%u - %s - %s\n",
				track->trackNumber,
				track->artist.c_str(),
				track->title.c_str());
		}
	}

	Serial.println("===== TEST COMPLETE =====");
}

void loop()
{
}