// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

#include "Simulation.h"
#include "WebConfigManager.h"
#include "WebConfigInput.h"

#include <vector>
#include <string>
#include <math.h>

#include "winbgi2.h"

using namespace std;

#define PI 3.1415926535f

namespace SampleApp
{
	class Math
	{
	public:

		static float Sin(float x) { return sinf(x); }
		static float Cos(float x) { return cosf(x); }
	};

	class Random
	{
	public:

		static int Next() { return rand(); }
		static int Next(int maxValue) { return rand() % maxValue; }
		static int Next(int minValue, int maxValue) { return minValue + (rand() % maxValue); }
	};

	class Vec3
	{
	public:

		float x, y, z;

		Vec3()
		{
			this->x = 0;
			this->y = 0;
			this->z = 0;
		}

		Vec3(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		Vec3 operator +(const Vec3& v) const
		{
			return Vec3(x + v.x, y + v.y, z + v.z);
		}

		Vec3 operator *(float scalar) const
		{
			return Vec3(x * scalar, y * scalar, z * scalar);
		}
	};

	/// <summary>
	/// The ball class
	/// </summary>
	class Ball
	{
		Vec3 pos;
		Vec3 vel;

	public:

		Ball()
		{
			const Simulation& sim = Simulation::Instance();

			pos.x = (float)WinBGI::getmaxx() / 2 + Random::Next(WinBGI::getmaxx() / 3);
			pos.y = (float)WinBGI::getmaxy() / 2 + Random::Next(WinBGI::getmaxy() / 3);

			float speed = (float)Random::Next(5, 11); // 5 to 10
			float angle = (float)Random::Next(0, 360) * PI / 180;
			vel.x = Math::Cos(angle) * speed;
			vel.y = Math::Sin(angle) * speed;
		}

		void Draw()
		{
			const Simulation& sim = Simulation::Instance();

			WinBGI::setcolor(sim.BallColor);
			WinBGI::setfillstyle(WinBGI::SOLID_FILL, sim.BallColor);
			WinBGI::fillellipse( (int)pos.x, (int)pos.y, 20, 20);

			if (sim.showPos)
			{
				char buffer[80];
				sprintf(buffer, "(%.2f,%.2f)", pos.x, pos.y);

				int off = sim.BallRadius;
				WinBGI::outtextxy((int)(pos.x + off), (int)(pos.y + off), buffer);
			}
		}

		void Move()
		{
			const Simulation& sim = Simulation::Instance();

			pos = pos + vel * sim.BallSpeed;

			int w = WinBGI::getmaxx() + 1;
			int h = WinBGI::getmaxy() + 1;

			// Collide with left and right walls
			if (pos.x <= 0 || pos.x >= w - sim.BallRadius)
			{
				vel.x = -vel.x;
			}

			// Collide with top and bottom walls
			if (pos.y <= 0 || pos.y >= h - sim.BallRadius)
			{
				vel.y = -vel.y;
			}
		}
	};

	Simulation::Simulation()
	{
		numRequested = 10;

		showPos = false;

		BallRadius = 10;
		BallSpeed = 1;
		BallColor = WinBGI::RED;
	}

	void Simulation::Update()
	{
		for (int i=0; i<balls.size(); ++i)
		{
			balls[i]->Move();
		}
	}

	void Simulation::Draw()
	{
		for (int i=0; i<balls.size(); ++i)
		{
			balls[i]->Draw();
		}
	}

	void Simulation::Reset()
	{
		for (int i=0; i<balls.size(); ++i)
		{
			delete balls[i];
		}
		balls.clear();

		for (int i = 0; i < numRequested; i++)
		{
			balls.push_back(new Ball());
		}
	}

	static void StartAnimation()
	{
		Simulation::Instance().Reset();
	}

	void Simulation::Init()
	{
		WebConfig::FormSettings* formSettings = WebConfig::Manager::Instance().GetFormSettings("game");
		formSettings->AutoSubmit = true;
		formSettings->AutoSave = true;

		new WebConfig::InputButton("game/Reset", StartAnimation);

		(new WebConfig::InputSliderInt("game/Number of Balls", numRequested))->SetCallback(StartAnimation);
		(new WebConfig::InputSliderInt("game/Ball Radius", BallRadius))->SetRange(0, 20, 0);
		(new WebConfig::InputSlider("game/Speed Factor", BallSpeed))->SetRange(0.1f, 2, 1);

		static int choice = 1;

		new WebConfig::InputBool("game/show pos", showPos);
		WebConfig::InputSelect* chooser = new WebConfig::InputSelect("game/choose", choice);
		chooser->AddOption("now");
		chooser->AddOption("later");
		chooser->AddOption("never");

		static Vec3 color;
		(new WebConfig::InputSlider("game/Color.Red", color.x))->SetRange(0, 1, 1);
		(new WebConfig::InputSlider("game/Color.Green", color.y))->SetRange(0, 1, 1);
		(new WebConfig::InputSlider("game/Color.Blue", color.z))->SetRange(0, 1, 1);

		StartAnimation();
	}
}
