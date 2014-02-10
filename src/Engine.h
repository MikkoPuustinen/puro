// puro - Audio sampler library
//
// Copyright (c) 2013 Olli Erik Keskinen
// All rights reserved.
//
// This code is released under The BSD 2-Clause License.
// See the file LICENSE.txt for information.

#pragma once

#include "Puro.h"

struct DropBundle {
	Drop* drop;
	uint32_t index; // index is stored outside of drop
};

class Engine {
	PuroBase* instance_;
	std::list<struct DropBundle> drops_in_use_;
public:
	Engine(PuroBase* instance_);
	~Engine();
	void AddDrop(Drop* drop);
	void GetAudioOutput(uint32_t n, float* buffer);
};
