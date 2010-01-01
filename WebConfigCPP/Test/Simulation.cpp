// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

#if 0
using System;
using System.Text;
using System.IO;
using System.Drawing;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace SampleApp
{
    public class Vec3
    {
        public float x, y, z;

        public Vec3()
        {
            this.x = 0;
            this.y = 0;
            this.z = 0;
        }

        public Vec3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public static Vec3 operator +(Vec3 v1, Vec3 v2)
        {
            return new Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
        }

        public static Vec3 operator *(Vec3 v, float scalar)
        {
            return new Vec3(v.x * scalar, v.y * scalar, v.z * scalar);
        }
    }

    /// <summary>
    /// The ball class
    /// </summary>
    public class Ball
    {
        float colorScale;
        Vec3 pos = new Vec3();
        Vec3 vel = new Vec3();

        static Font font = new Font("Arial", 8);
        static SolidBrush fontBrush = new SolidBrush(Color.Black);

        public Ball()
        {
            Simulation sim = Simulation.Instance;
            colorScale = sim.Rand.Next(0, 101) / 100.0f; // 0 to 1

            pos.x = sim.Rand.Next(100, 200);
            pos.y = sim.Rand.Next(100, 200);

            float speed = sim.Rand.Next(5, 11); // 5 to 10
            float angle = sim.Rand.Next(0, 360);
            vel.x = (float)Math.Cos((double)angle) * speed;
            vel.y = (float)Math.Sin((double)angle) * speed;
        }

        public void Draw(Graphics g)
        {
            Simulation sim = Simulation.Instance;

            // compute color
            Vec3 temp = sim.BallColor * (colorScale * 255);
            Color color = Color.FromArgb((int)temp.x, (int)temp.y, (int)temp.z);

            g.FillEllipse(new SolidBrush(Color.Black), pos.x, pos.y, sim.BallRadius + 0.5f, sim.BallRadius + 0.5f);
            g.FillEllipse(new SolidBrush(color), pos.x, pos.y, sim.BallRadius, sim.BallRadius);

            if (Simulation.Instance.showPos)
            {
                float off = sim.BallRadius;
                g.DrawString(string.Format("({0:0.0},{1:0.0})", pos.x, pos.y),
                    font, fontBrush, pos.x + off, pos.y + off, new StringFormat());
            }
        }

        public void Move(int screenWidth, int screenHeight)
        {
            Simulation sim = Simulation.Instance;

            pos = pos + vel * sim.BallSpeed;

            // Collide with top and bottom walls
            if (pos.y <= 0 || pos.y >= screenHeight - sim.BallRadius)
            {
                vel.y = -vel.y;
            }

            // Collide with left and right walls
            if (pos.x <= 0 || pos.x >= screenWidth - sim.BallRadius)
            {
                vel.x = -vel.x;
            }
        }
    }

    /// <summary>
    /// The simulation manager
    /// </summary>
    public class Simulation
    {
        #region singleton pattern
        // http://www.yoda.arachsys.com/csharp/singleton.html
        private static readonly Simulation instance = new Simulation();
        public static Simulation Instance { get { return instance; } }

        // Explicit static constructor to tell C# compiler
        // not to mark type as beforefieldinit
        static Simulation() { }

        private Simulation()
        {
        }
        #endregion

        int numRequested = 10;

        public bool showPos = false;
        public Random Rand = new Random();

        public int BallRadius = 10;
        public float BallSpeed = 1;
        public Vec3 BallColor = new Vec3(0, 1, 0);

        List<Ball> balls = new List<Ball>();

        public void Update(int width, int height)
        {
            foreach (Ball ball in balls)
            {
                ball.Move(width, height);
            }
        }

        public void Draw(Graphics g)
        {
            foreach (Ball ball in balls)
            {
                ball.Draw(g);
            }
        }

        public void Reset(int numBalls)
        {
            balls.Clear();
            for (int i = 0; i < numBalls; i++)
            {
                balls.Add(new Ball());
            }
        }

        void StartAnimation()
        {
            Simulation.Instance.Reset(numRequested);
        }

        public void Init()
        {
            WebConfig.FormSettings formSettings = WebConfig.Manager.Instance.GetForm("game");
            formSettings.AutoSubmit = true;
            formSettings.AutoSave = true;

            new WebConfig.InputButton("game/Reset", (val) => StartAnimation());

            new WebConfig.InputSliderInt("game/Number of Balls",
                () => numRequested, (val) => { numRequested = val; StartAnimation(); });
            new WebConfig.InputSliderInt("game/Ball Radius",
                () => BallRadius, (val) => { BallRadius = val; }).SetRange(0, 20, 0);
            new WebConfig.InputSliderFloat("game/Speed Factor",
                () => BallSpeed, (val) => { BallSpeed = val; }).SetRange(0.1f, 2, 1);

            int choice = 1;
            new WebConfig.InputBool("game/show pos", () => showPos, (val) => showPos = val);
            new WebConfig.InputSelect("game/choose", () => choice, (val) => choice = val).SetOptions("now", "later", "never");

            new WebConfig.InputSliderFloat("game/Color.Red",
                () => BallColor.x, (val) => { BallColor.x = val; }).SetRange(0, 1, 1);
            new WebConfig.InputSliderFloat("game/Color.Green",
                () => BallColor.y, (val) => { BallColor.y = val; }).SetRange(0, 1, 1);
            new WebConfig.InputSliderFloat("game/Color.Blue",
                () => BallColor.z, (val) => { BallColor.z = val; }).SetRange(0, 1, 1);

            StartAnimation();
        }
    }
}
#endif