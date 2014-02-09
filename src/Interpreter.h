// puro - Audio sampler library
//
// Copyright (c) 2013 Olli Erik Keskinen
// All rights reserved.
//
// This code is released under The BSD 2-Clause License.
// See the file LICENSE.txt for information.

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include "Puro.h"

class Interpreter {
	PuroBase* instance_;
public:
	Interpreter(PuroBase* instance);
	void SetMaterial(Tag idea, Tag material);
	void SetSync(Tag idea);
	void SetAudioPassage(Tag idea, uint16_t n_data, float* data);
	void SetEnvelopePassage(Tag idea, uint16_t n_data, float* data);
	void OnsetDropFromIdea(Tag idea, Time relative=0);
	void LoadAudioMaterial(Tag association, char* path_to_file);

};

#endif /* INTERPRETER_H_ */
