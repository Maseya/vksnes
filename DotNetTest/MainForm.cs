namespace DotNetTest;

using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

using SnesRenderNet;

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

    private System.Timers.Timer Timer { get; }

    private Instance Instance
    {
        get;
    }

    private Surface Surface
    {
        get; set;
    }

    private CheckerboardPipeline CheckerboardPipeline
    {
        get; set;
    }

    private CheckerboardSurfaceFramebuffers CheckerboardSurfaceFramebuffers
    {
        get; set;
    }

    private SizeF CheckerboardSize
    {
        get; set;
    }

    private Color CheckerboardColor1
    {
        get;
        set;
    }

    private Color CheckerboardColor2
    {
        get;
        set;
    }

    private PalettePipeline PalettePipeline
    {
        get;
        set;
    }

    private PaletteSurfaceFramebuffers PaletteSurfaceFramebuffers
    {
        get; set;
    }

    private byte[] PaletteData
    {
        get;
        set;
    }

    private Size ViewSize
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
        Surface = new Surface(Instance, drawSurface);

        CheckerboardPipeline = new CheckerboardPipeline(Instance);
        CheckerboardSurfaceFramebuffers = new CheckerboardSurfaceFramebuffers(
            CheckerboardPipeline,
            Surface,
            2);

        PalettePipeline = new PalettePipeline(Instance);
        PaletteSurfaceFramebuffers = new PaletteSurfaceFramebuffers(
            PalettePipeline,
            Surface,
            1000001002);

        CheckerboardSize = new SizeF(4, 4);
        CheckerboardColor1 = Color.FromArgb(0, 200, 200, 200);
        CheckerboardColor2 = Color.FromArgb(0, 240, 240, 240);
        ViewSize = new Size(16, 16);

        PaletteData = File.ReadAllBytes(Environment.GetCommandLineArgs()[1]);

        StartTime = DateTime.Now;
        Timer.Start();
    }

    private void Timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
    {
        drawSurface.Invalidate();
    }

    private void UpdateFrame()
    {
        var frame = 60 * ElapsedTime.TotalMilliseconds / 1000;
        var angle = frame * Math.PI / 180;
        var transparency = (int)(255 * (0.5 * (Math.Cos(angle) + 1) / 2));
        CheckerboardColor1 = Color.FromArgb(transparency, CheckerboardColor1);
        CheckerboardColor2 = Color.FromArgb(transparency, CheckerboardColor2);
    }

    private void Draw(Surface surface)
    {
        UpdateFrame();

        if (!surface.IsSwapchainValid())
        {
            if (surface.TryRecreateSwapchain())
            {
                CheckerboardSurfaceFramebuffers.Dispose();
                CheckerboardSurfaceFramebuffers =
                    new CheckerboardSurfaceFramebuffers(
                        CheckerboardPipeline,
                        Surface,
                        2);

                PaletteSurfaceFramebuffers.Dispose();
                PaletteSurfaceFramebuffers =
                    new PaletteSurfaceFramebuffers(
                        PalettePipeline,
                        Surface);
            }
            else
            {
                return;
            }
        }

        if (!surface.TryGetSurfaceImage(out var image))
        {
            Draw(surface);
            return;
        }

        var checkerboardFramebuffer = CheckerboardSurfaceFramebuffers.
            GetFramebuffer(image);
        var paletteFramebuffer = PaletteSurfaceFramebuffers.
            GetFramebuffer(image);

        paletteFramebuffer.SetDependency(checkerboardFramebuffer);

        checkerboardFramebuffer.UpdatePattern(
            CheckerboardSize,
            CheckerboardColor1,
            CheckerboardColor2);
        paletteFramebuffer.UpdateView(ViewSize);
        paletteFramebuffer.UpdateColorData(PaletteData);

        checkerboardFramebuffer.Render(1);
        paletteFramebuffer.Render(1);

        if (!surface.Present(image))
        {
            Draw(surface);
        }
    }

    private void DrawSurface1_Paint(object sender, PaintEventArgs e)
    {
        Draw(Surface);
    }

    private void MainForm_Disposed(object sender, EventArgs e)
    {
        PaletteSurfaceFramebuffers.Dispose();
        PalettePipeline.Dispose();
        CheckerboardSurfaceFramebuffers.Dispose();
        CheckerboardPipeline.Dispose();
        Surface.Dispose();
    }
}
