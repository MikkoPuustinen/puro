// puro - Audio sampler library
//
// Copyright (c) 2013 Olli Erik Keskinen
// All rights reserved.
//
// This code is released under The BSD 2-Clause License.
// See the file LICENSE.txt for information.

#include "Interpreter.h"
#include "Idea.h"
#include "MainFrame.h"

Interpreter::Interpreter(MainFrame* instance) {
	//std::cout << "Interpreter" << std::endl;
	instance_ = instance;
}
// TODO ACTUALLY INTERFACE THESE
void
Interpreter::SetMaterial(Tag idea, Tag material) {
	Idea* idea_to_use = instance_->GetIdea(idea);
	idea_to_use->SetMaterial(material);
}

void
Interpreter::SetAudioPassage(Tag idea, uint16_t n_data, float* data) {

	Idea* idea_to_use = instance_->GetIdea(idea);
	Passage* passage_to_use = idea_to_use->GetAudioPassage();
	FloatListToPassage(passage_to_use, n_data, data);
}

void
Interpreter::SetEnvelopePassage(Tag idea, uint16_t n_data, float* data) {

	Idea* idea_to_use = instance_->GetIdea(idea);
	Passage* passage_to_use = idea_to_use->GetEnvelopePassage();
	FloatListToPassage(passage_to_use, n_data, data);
}

void
Interpreter::OnsetDropFromIdea(Tag idea) {
	//std::cout << "Onset drop from idea" << std::endl;
	Idea* idea_to_use = instance_->GetIdea(idea);
	instance_->OnsetDrop(idea_to_use);
}
void
Interpreter::LoadAudioMaterial(Tag association, char* path_to_file) {
	instance_->LoadAudioMaterial(association, path_to_file);
}
