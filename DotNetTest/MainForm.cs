namespace DotNetTest
{
    using System;
    using System.Windows.Forms;

    struct Vertex
    {
        public float x, y, r, g, b;

        public Vertex(float x, float y, float r, float g, float b)
        {
            this.x = x;
            this.y = y;
            this.r = r;
            this.g = g;
            this.b = b;
        }
    }

    public partial class MainForm : Form
    {
        public MainForm(Instance instance)
        {
            InitializeComponent();

            Timer = new System.Timers.Timer(5);
            Timer.Elapsed += Timer_Elapsed;

            Instance = instance;

            Disposed += MainForm_Disposed;
        }

        System.Timers.Timer Timer { get; }

        private Instance Instance
        {
            get;
        }

        private Surface Surface1
        {
            get; set;
        }

        private Surface Surface2
        {
            get; set;
        }

        private TestVertexBuffers VertexBuffers1
        {
            get; set;
        }

        private TestVertexBuffers VertexBuffers2
        {
            get; set;
        }

        private TestUniformBuffers UniformBuffers1
        {
            get; set;
        }

        private TestUniformBuffers UniformBuffers2
        {
            get; set;
        }

        private float Angle1
        {
            get;
            set;
        }

        private float Angle2
        {
            get;
            set;
        }

        private DateTime StartTime
        {
            get; set;
        }

        private TimeSpan ElapsedTime
        {
            get
            {
                return DateTime.Now - StartTime;
            }
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            Surface1 = Instance.CreateWin32Surface(drawSurface1);
            Surface2 = Instance.CreateWin32Surface(drawSurface2);

            VertexBuffers1 = Surface1.CreateTestVertexBuffers();
            VertexBuffers2 = Surface2.CreateTestVertexBuffers();

            UniformBuffers1 = Surface1.CreateTestUniformBuffers();
            UniformBuffers2 = Surface2.CreateTestUniformBuffers();

            var vertices1 = new Vertex[] {
                new Vertex(+0.0f, -0.5f, 1.0f, 1.0f, 0.0f),
                new Vertex(+0.5f, 0.5f, 0.0f, 1.0f, 1.0f),
                new Vertex(-0.5f, 0.5f, 1.0f, 0.0f, 1.0f) };

            var vertices2 = new Vertex[]{
                new Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f),
                new Vertex(+0.5f, -0.5f, 1.0f, 0.0f, 0.0f),
                new Vertex(+0.5f, +0.5f, 1.0f, 1.0f, 1.0f),
                new Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f),
                new Vertex(+0.5f, +0.5f, 1.0f, 1.0f, 1.0f),
                new Vertex(-0.5f, +0.5f, 0.0f, 1.0f, 1.0f)
            };

            VertexBuffers1.UpdateBuffer(vertices1);
            VertexBuffers2.UpdateBuffer(vertices2);

            StartTime = DateTime.Now;
            Timer.Start();
        }

        private void Timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            drawSurface1.Invalidate();
            drawSurface2.Invalidate();
            Angle1 = (float)(ElapsedTime.TotalMilliseconds / 800.0);
            Angle2 = -(float)(ElapsedTime.TotalMilliseconds / 400.0);
        }

        private void Draw(
            Surface surface,
            TestVertexBuffers vertexBuffers,
            TestUniformBuffers uniformBuffers,
            float angle)
        {
            _loop:
            using (var target = surface.AcquireRenderTarget())
            {
                if (!target)
                {
                    return;
                }

                Instance.RenderTestImageWithBuffers(
                    target,
                    vertexBuffers,
                    uniformBuffers,
                    angle);

                if (!surface.Present(target))
                {
                    goto _loop;
                }
            }
        }

        private void DrawSurface1_Paint(object sender, PaintEventArgs e)
        {
            Draw(Surface1, VertexBuffers1, UniformBuffers1, Angle1);
        }

        private void DrawSurface2_Paint(object sender, PaintEventArgs e)
        {
            Draw(Surface2, VertexBuffers2, UniformBuffers2, Angle2);
        }

        private void MainForm_Disposed(object sender, EventArgs e)
        {
            UniformBuffers2.Dispose();
            UniformBuffers1.Dispose();
            VertexBuffers2.Dispose();
            VertexBuffers1.Dispose();
            Surface2.Dispose();
            Surface1.Dispose();
        }
    }
}
