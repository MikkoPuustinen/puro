/*
 * main.cpp
 *
 *  Created on: Jul 11, 2013
 *      Author: oek
 */

#include "Puro.h"
#include "Interpreter.h"
#include "Engine.h"
#include "Passage.h"

#include "ICST/common.h"
#include "ICST/AudioFile.h"


#define ARRLEN(arr) (sizeof(arr)/sizeof(arr[0]))


int main() {

	///////////////////////////////////////////////
	// PREPARE ENVIRONMENT
	///////////////////////////////////////////////

	Puro* puro = new Puro();
	Interpreter* interp = puro->GetInterpreter();
	Engine* engine = puro->GetEngine();

	Tag material = CharsToTag((char*)"lafille");

	interp->LoadAudioMaterial(material, (char*)"/home/oek/lafille.wav");


	///////////////////////////////////////////////
	// PREPARE UNIT 1
	///////////////////////////////////////////////
	{
		Tag idea = CharsToTag((char*)"u1");

		interp->SetMaterial(idea, material);

		// AUDIO
		std::cout << "AUDIO" << std::endl;
		float audio_list[] = { 5.0, 4.0, 9.0 }; // linear
		interp->SetAudioPassage(idea, ARRLEN(audio_list), audio_list);

		// ENVELOPE
		std::cout << "ENVELOPE" << std::endl;
		float envelope_list[5] = { 0, 0.5, 1.0, 1.0, 0 }; 	// trapezoid
		//float envelope_list[] = { 1.0, 1.0, 1.0 }; 	// const

		interp->SetEnvelopePassage(idea, ARRLEN(envelope_list), envelope_list);

		interp->OnsetDropFromIdea(idea);
	}
	///////////////////////////////////////////////
	// PREPARE UNIT 2
	///////////////////////////////////////////////
	{
		Tag idea = CharsToTag((char*)"u2");

		interp->SetMaterial(idea, material);

		// AUDIO
		std::cout << "AUDIO" << std::endl;
		float audio_list[] = { 10.0, 4.0, 14.0 }; // linear
		interp->SetAudioPassage(idea, ARRLEN(audio_list), audio_list);

		// ENVELOPE
		std::cout << "ENVELOPE" << std::endl;
		//float envelope_list[5] = { 0, 0.5, 1.0, 1.0, 0 }; 	// trapezoid
		float envelope_list[] = { 1.0, 1.0, 1.0 }; 	// const

		interp->SetEnvelopePassage(idea, ARRLEN(envelope_list), envelope_list);

		//interp->OnsetDropFromIdea(idea);
	}

	///////////////////////////////////////////////
	// PREPARE OUTPUT FILE
	///////////////////////////////////////////////

	uint32_t n = 1024;
	uint32_t blocks = 200;
	icstdsp::AudioFile* out_file = new icstdsp::AudioFile();
	out_file->Create(n*blocks);
	float* buffer = out_file->GetSafePt();

	for (unsigned k=0; k<n*blocks; k++)
		buffer[k] = 0;

	for (unsigned b=0; b<blocks; b++) {
		engine->GetAudioOutput(n, &buffer[b*n]);
	}

	out_file->SaveWave((char*)"/home/oek/outfile.wav");

	//for (unsigned k=0; k<n*blocks; k++)
	//	std::cout << buffer[k] << std::endl;

	std::cout << "Done!" << std::endl;
}
