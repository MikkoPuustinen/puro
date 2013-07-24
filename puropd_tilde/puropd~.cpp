#include <string.h>
#include <malloc.h>

#include "m_pd.h"
#include "../purobase/Puro.h"
#include "../purobase/Interpreter.h"
#include "../purobase/Engine.h"

////////////////////////////
//////// INIT CLASS ////////
////////////////////////////

static t_class *puropd_tilde_class;

typedef struct _puropd_tilde {
	t_object  x_obj;
	// init variables
	t_sample f;

   Puro* puro;
   Engine* engine;
   Interpreter* interp;

   float buf[64];

} t_puropd_tilde;

////////////////////////
//////// CREATE ////////
////////////////////////

void *puropd_tilde_new(t_floatarg f)
{
	t_puropd_tilde *x = (t_puropd_tilde *)pd_new(puropd_tilde_class);

	outlet_new(&x->x_obj, &s_signal);

	x->puro = new Puro();
	x->engine = x->puro->GetEngine();
	x->interp = x->puro->GetInterpreter();

  return (void *)x;
}

void puropd_tilde_float(t_puropd_tilde *x, t_floatarg f)
{
	//Complex *c = new Complex(f, 2);
	//as->AddFile(10, (char*)"/home/oek/file.wav");
	post(x->puro->HelloWorld());
}

void puropd_tilde_anything(t_puropd_tilde *x, t_symbol *s, int argc, t_atom *argv) {
	post("got any argc: %d", argc);
	post("symbol: %s", s->s_name);
	post("len: %u", strlen(s->s_name));

	char* command = s->s_name;

	///////////////////////////////
	// LOAD FILE
	///////////////////////////////
	if (!strcmp(command, "loadfile")) {
		if (argc == 2) {
			char* tag = (char*)malloc(10);
			atom_string(&argv[0], tag, 10);
			char* path = (char*)malloc(256);
			atom_string(&argv[1], path, 256);
			post("load file %s %s", tag, path);
			x->interp->LoadAudioMaterial(CharsToTag(tag), path);
		}
		return;
	}
	///////////////////////////////
	// ONSET
	///////////////////////////////
	if (argc == 1) {
		char* param = (char*)malloc(10);
		atom_string(&argv[0], param, 10);
		if (!strcmp(param, "onset")) {
			post("onset! tag: %s", command);
			x->interp->OnsetDropFromIdea(CharsToTag(command));
		}
		return;
	}
	///////////////////////////////
	// SET PARAMETERS
	///////////////////////////////
	if (argc > 1) {
		char* param = (char*)malloc(10);
		atom_string(&argv[0], param, 10);
		post("param: %s\nlen: %u", param, strlen(param));

		// SET MATERIAL
		if (!strcmp(param, "material")) {
			char* material_tag = (char*)malloc(9);
			atom_string(&argv[1], material_tag, 9);
			post("Set material of %s to %s", command, material_tag);
			x->interp->SetMaterial(CharsToTag(command), CharsToTag(material_tag));
			return;
		}
		// SET PLAYHEAD PASSAGE
		if (!strcmp(param, "playhead")) {
			float list[256];
			for (int i=1;i<argc;i++) {
				list[i-1] = atom_getfloat(&argv[i]);
			}
			x->interp->SetAudioPassage(CharsToTag(command), (uint16_t)argc-1, list);
			return;
		}
		// SET ENVELOPE PASSAGE
		if (!strcmp(param, "envelope")) {
			float list[256];
			for (int i=1;i<argc;i++) {
				list[i-1] = atom_getfloat(&argv[i]);
			}
			x->interp->SetEnvelopePassage(CharsToTag(command), (uint16_t)argc-1, list);
			return;
		}
	} // end of set parameter
}

t_int *puropd_tilde_perform(t_int *w)
{
	t_puropd_tilde *x = (t_puropd_tilde *)(w[1]);
	t_sample *out =    (t_sample *)(w[2]);
	uint32_t n =           (uint32_t)(w[3]);

	for (uint32_t i=0; i<n; i++) {
		x->buf[n] = 0;
	}

	x->engine->GetAudioOutput(n, &x->buf[0]);

	for (uint32_t i=0; i<n; i++) {
		out[i] = (t_sample)x->buf[i];
	//	out[i] = 0;
	}

	return (w+4); // <- THIS REMEMBER!!
}


void puropd_tilde_dsp(t_puropd_tilde *x, t_signal **sp)
{
  dsp_add(puropd_tilde_perform, 3, x,
          sp[0]->s_vec, sp[0]->s_n);
}

///////////////////////
//////// SETUP ////////
///////////////////////

extern "C" void puropd_tilde_setup(void) {
  puropd_tilde_class = class_new(gensym("puropd~"),	// NAME
        (t_newmethod)puropd_tilde_new,				// CONSTRUCTOR
        0, sizeof(t_puropd_tilde),					// DCONSTR, SIZE
        CLASS_DEFAULT,								// OUTLOOK
        (t_atomtype)0);											// ARGUMENTS

	class_addfloat(puropd_tilde_class, puropd_tilde_float);
	class_addanything(puropd_tilde_class, puropd_tilde_anything);

	class_addmethod(puropd_tilde_class,
			(t_method)puropd_tilde_dsp, gensym("dsp"), (t_atomtype)0);
}

