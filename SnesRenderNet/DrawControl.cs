namespace SnesRenderNet
{
    using System.Windows.Forms;
    using System.ComponentModel;
    using static System.ComponentModel.DesignerSerializationVisibility;

    public class DrawControl : UserControl
    {
        public DrawControl()
        {
            ResizeRedraw = true;
            SetStyle(ControlStyles.UserPaint, true);
        }

        [Browsable(false)]
        [DesignerSerializationVisibility(Hidden)]
        public bool IsInDesignMode
        {
            get
            {
                return DesignMode
                    || LicenseManager.UsageMode == LicenseUsageMode.Designtime;
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            if (IsInDesignMode)
            {
                e?.Graphics.Clear(BackColor);
            }

            base.OnPaint(e);
        }

        protected override void OnPaintBackground(PaintEventArgs e)
        {
            //base.OnPaintBackground(e);
        }
    }
}
