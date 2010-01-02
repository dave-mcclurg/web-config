// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

#include <vector>
#include <assert.h>

namespace SampleApp
{
    /// <summary>
    /// The simulation manager
    /// </summary>
    class Simulation
    {
		/// <summary>
		/// private constructor
		/// </summary>
		Simulation();

		Simulation(const Simulation&)
		{
			assert(false);
		}

		Simulation& operator=(const Simulation&)
		{
			assert(false);
			return *this;
		}

	public:

		/// <summary>
		/// Get the singleton
		/// </summary>
        static Simulation& Instance()
		{
	        static Simulation instance;
			return instance;
		}

        int numRequested;
        bool showPos;
        int BallRadius;
        float BallSpeed;
        int BallColor;
		std::vector<class Ball*> balls;

        void Update();
        void Draw();
        void Reset(int numBalls);
        void StartAnimation();
        void Init();
    };
}
