#if 0
#include "PuroHeader.h"

// TODO
// - Random deviation parameters for granulator
// - ints to size_ts where appropriate

int main()
{
    const int n = 256;
    const int blockSize = 32;
    const int numChannels = 2;

    std::vector<float> left (n, 0.0f);
    std::vector<float> right (n, 0.0f);

    using Envelope = SineEnvelope<float>;
    //using AudioSource = NoiseSource<float>;
    using AudioSource = AudioBufferSource<float>;

    using Context = EnvelopeProcessorContext<float>;
    using Grain = EnvelopeProcessor<float, Context, AudioSource, Envelope>;
    using Sound = SoundObject<float, Grain, Context>;
    using Pool = DynamicPool<Sound, 4>;

    //using Engine = EngineTemplate<float, Grain, Pool, AudioObj, Context>;
    using Engine = SoundObjectEngine<float, Sound, Grain, Context, Pool>;
    using Controller = BufferedGranularController<float, Engine, Sound, AudioSource, Envelope>;

    Engine engine;
    Controller controller (engine);

    std::vector<float> fileVector;

    Buffer<float> fileBuffer (2, 1024, fileVector);

    for (int i=0; i<fileBuffer.size(); ++i)
    {
        const float r1 = ((float)std::rand() / (float)RAND_MAX) * 2 -1;
        fileBuffer.channel(0)[i] = r1;

        const float r2 = ((float)std::rand() / (float)RAND_MAX) * 2 -1;
        fileBuffer.channel(1)[i] = r2;
    }

    controller.setAudioBuffer(fileBuffer);

    for (int i = 0; i < n - blockSize; i += blockSize)
    {
        Buffer<float> buffer ( { &left[i], &right[i] }, numChannels, blockSize);

        controller.advance(blockSize);
        engine.addNextOutput(buffer);
    }


    std::cout << "\n    OUTPUT\n----------\n";

    for (int i=0; i<n; i++)
    {
        std::cout << i << ":\t" << left[i] << "\t" << right[i] << std::endl;
    }

    return 0;
}
#endif
